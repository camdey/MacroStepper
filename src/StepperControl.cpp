#include "DriverConfig.h"
#include "GlobalVariables.h"
#include "MiscFunctions.h"
#include "StepperControl.h"
#include "ShutterControl.h"
#include "UI-Main.h"
#include "UI-Auto.h"
#include "UI-AutoConfig.h"

/***********************************************************************
Runs an AutoStack procedure comprised of multiple Movements. The
function will first check whether the stage is in the Start position,
if not it will move to the Start. If the number of completed
Movements is less than the number of Movements required, it will
attempt to take a photo and move the stage one Movement. If no
Movement was taken due to delay, this function is called again and
another Movement attempted. When all Movements required are completed,
the autoStack will be completed and ceased to be called.
***********************************************************************/
void autoStack() {
  // wake stepper from sleep
  if (!isStepperEnabled()) {
    setStepperEnabled(true);
  }

  // move stepper to start if not already there
  if (goToStart) {
    if (driver.XACTUAL() != startPosition) {
      int distanceToStart = (driver.XACTUAL() - startPosition);
      // if current position is in front of start position
      if (distanceToStart > 0) {
        // move backwards to start postion
        // overshoot by 200 steps and return to start
        overshootPosition(startPosition, 200, -1);
      }
      // if current position is behind start position
      else if (distanceToStart < 0) {
        // move forward to start postion
        while (driver.XACTUAL() != startPosition) {
          // stepper.setSpeed(600);
          // stepper.moveTo(startPosition);
          // stepper.runSpeedToPosition();
        }
      }
    }
		goToStart = false;
		setNrMovementsCompleted(0);
		joystickState = false;
		setExecutedMovement(false);
		shutterTriggered = false;
    setLastStepTime(millis());
    // Serial.println("0. Start");
  }

  // take photo and increment progress for first step of each movement
  if (getNrMovementsCompleted() <= getNrMovementsRequired() && !hasExecutedMovement()) {

    // if shutter enabled, take photo
		if (shutterEnabled && !shutterTriggered) {

	    shutterTriggered = triggerShutter(); // take photo

      // pause autoStack if flash failing
      if (!shutterTriggered) {
        long failTime = millis();
        triggerFailed = true; // used to set one-off delay later
        // Serial.println("fail to trigger");

        while (!shutterTriggered) {
          shutterTriggered = triggerShutter();
          // if retry successful, break
          if (shutterTriggered) {
            // Serial.println("retry successful");
            break;
          }
          // go to flashScreen if can't trigger after 10 seconds
          else if (millis() - failTime >= 10000) {
            // Serial.println("go to flash screen");
            auto_screen::pauseStack();
            populateScreen("Flash"); // load screen for checking flash settings/functionality
            break;
          }
        }
      }
		}

    // only move if autoStack hasn't been paused by flash failure
    if (!autoStackPaused) {
  		// move stepper
  		executeMovement(1, shutterDelay*1000); // forward direction, shutterdelay
    }

		if (hasExecutedMovement()) {
			setNrMovementsCompleted(getNrMovementsCompleted()+1);
	    // make sure correct screen is displaying
	    if (getCurrentScreen() == "Auto") { // auto screen
	      auto_screen::updateProgress();
				auto_screen::estimateDuration();
	    }

			shutterTriggered = false; // reset shutter
			setExecutedMovement(false); // reset stepper move
		}

    if (triggerFailed) {
      // 10 second delay to prevent trying to take another photo immediately after
      delay(10000);
    }
  }
  // stop AutoStack sequence if end reached
  if (getNrMovementsCompleted() >= getNrMovementsRequired()) {
    if (getCurrentScreen() != "Auto") {
      populateScreen("Auto");           // go back to Auto screen if not already on it
    }
		auto_screen::estimateDuration();    // update estimate
    autoStackRunning = false;
    goToStart = true;
    joystickState = true;               // re-enable joystick control
    shutterTriggered = false;
    autoStackPaused = false;            // autoStack is completed but not paused
    setNrMovementsCompleted(0);         // reset completed movements count
		setExecutedMovement(false);
    auto_screen::pauseStack();          // reset PlayPause button to "paused" mode"
  }
}


void dryRun() {
  // // wake stepper from sleep
	// if (digitalRead(EN_PIN) == HIGH) {
  //   setStepperEnabled(true);
  // }

  // // move backwards to start
  // while (driver.XACTUAL() > startPosition) {
  //   // stepper.setSpeed(-600);
  //   // stepper.moveTo(startPosition);
  //   // stepper.runSpeedToPosition();
  // }
  // config_screen::displayPosition();

  // // step slow through procedure
  // while (driver.XACTUAL() < endPosition) {
  //   // stepper.setSpeed(100);
  //   // stepper.moveTo(endPosition);
  //   // stepper.runSpeedToPosition();
  // }
  // config_screen::displayPosition();

  // // return to start
  // if (driver.XACTUAL() > startPosition) {
  //   // overshoot by 200 steps and return to start
  //   overshootPosition(startPosition, 200, -1);
  // }

  // // stops motor instantly
  // // stepper.setSpeed(0);
  // setNrMovementsCompleted(0)
  // config_screen::displayPosition();
}


void homeRail() {
  bool hitRearStop, hitForwardStop = false;
  if (!isStepperEnabled()) {
    setStepperEnabled(true);
  }
	// set configuration for stallGuard
	configStallGuard();
  // reset positions
  setForwardEndStop(1);
  setBackwardEndStop(1);

  // move to rear end stop + 100,000 just to ensure it over-runs without decelerating
	driver.XTARGET(-maxRailPosition - 100000);
  hitRearStop = detectEndStop(); // wait until end stop detected
  // if endstop detected... 
  if (hitRearStop) {
    delay(100);
    driver.sg_stop(false);
    driver.sg_stop(true);

    Serial.print("Actual: "); Serial.println(driver.XACTUAL());

    driver.XACTUAL(0); // set rear end stop position as 0
    setBackwardEndStop(driver.XACTUAL()); // set backward position
  }
  // move to forward end stop
  driver.XTARGET(maxRailPosition + 100000);
  hitForwardStop = detectEndStop(); // wait until end stop detected
  // if endstop detected...
  if (hitForwardStop) {
    delay(100);
    driver.sg_stop(false);
    driver.sg_stop(true);

    Serial.print("Actual: "); Serial.println(driver.XACTUAL());

    driver.XACTUAL(maxRailPosition); // set front end stop position as 0
    setForwardEndStop(driver.XACTUAL()); // set forward position
  }

	// if back and forward position set, move to middle position
	if (getBackwardEndStop() == minRailPosition && getForwardEndStop() == maxRailPosition) {
    driver.XTARGET(192000);
    // wait to reach position before changing driver config after homing completed
    while (driver.XACTUAL() != driver.XTARGET()) {
      debugStallGuard(); // print to serial monitor
    }

    config_screen::setAutoStackStartPosition(); // update autoStack positions as rail has moved
    config_screen::setAutoStackEndPosition(); // update autoStack positions as rail has moved
		configStealthChop(); // set config for silentStep
    runHomingSequence = false;
    setRailHomed(true);
	}
}


void debugStallGuard() {
  if (millis() - getLastMillis() >= 100) {
    Serial.print(" | event_stop_sg: "); Serial.print(driver.event_stop_sg());
    Serial.print(" | sg_result: "); Serial.print(driver.sg_result());
    Serial.print(" | TSTEP: "); Serial.print(driver.TSTEP());
    Serial.print(" | CSACTUAL: "); Serial.print(driver.cs_actual());
    Serial.print(" | XACTUAL: "); Serial.println(driver.XACTUAL());
    setLastMillis(millis());
  }
}


bool detectEndStop() {
  while (driver.event_stop_sg() == 0) {
    debugStallGuard(); // print to serial monitor
  }
  return true;
}


void overshootPosition(int position, int numberOfSteps, int direction) {
  // int speed = 600 * direction;
  // int offsetPosition = position - numberOfSteps;

  // while (driver.XACTUAL() != offsetPosition) {
  //   // stepper.setSpeed(speed);
  //   // stepper.moveTo(offsetPosition); // move past start
  //   // stepper.runSpeedToPosition();
  // }
  // delay(50);
  // while (driver.XACTUAL() != position) {
  //   // stepper.setSpeed(-speed); // reverse direcyion
  //   // stepper.moveTo(position); // move to start
  //   // stepper.runSpeedToPosition();
  // }
}


/******************************************************************
Moves the stepper one Movement in a given direction. A Movement
is the total number of steps required to travel a given distance.
If there hasn't been a specified delay since the previous Movement,
this function returns false and the calling function should loop
until it receives a response of true. Prevents stall if rail has
been homed and will exit autoStack if running.
******************************************************************/
void executeMovement(int stepDirection, unsigned long stepperDelay) {
  // change to StealthChop is StallGuard is configured
  if (stallGuardConfigured) {
    configStealthChop();
  }

  // step after elapsed amount of time
  if ((millis() - getLastStepTime() > stepperDelay)) {
    // enable stepper if currently disabled
		if (!isStepperEnabled()) {
	    setStepperEnabled(true);
	  }

    int stepVelocity = stepDirection * getStepsPerMovement();
    long targetPosition = driver.XACTUAL() + stepVelocity;

    // set new target position
    driver.XTARGET(targetPosition);
    // while(driver.XACTUAL() != driver.XTARGET()) {
    //   Serial.print("ACTUAL: "); Serial.print(driver.XACTUAL());
    //   Serial.print(" | TARGET: "); Serial.println(driver.XTARGET());
    // }
    setExecutedMovement(true);  
    setLastStepTime(millis());
  }
  else {
		setExecutedMovement(false); // no step taken
	}
}