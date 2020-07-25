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
  // move to start position if beginning AutoStack
  if (isNewAutoStack) {
    // reduce max velocity to minimize vibration
    setTargetVelocity(10000);
    goToStart();
    setNrMovementsCompleted(0);
    setExecutedMovement(false);
    setShutterTriggered(false);
    setLastStepTime(millis());
  }

  // take photo and increment progress for first step of each movement
  if (getNrMovementsCompleted() <= getNrMovementsRequired() && !hasExecutedMovement()) {
    // if shutter enabled, take photo
		if (isShutterEnabled() && !hasShutterTriggered()) {
	    triggerShutter(); // take photo
      // pause autoStack if flash failing
      if (!hasShutterTriggered()) {
        long failTime = millis();
        triggerFailed = true; // used to set one-off delay later
        // retry triggering shutter
        while (!hasShutterTriggered()) {
          triggerShutter();
          // if retry successful, break
          if (hasShutterTriggered()) {
            break;
          }
          // go to flashScreen if can't trigger after 10 seconds
          else if (millis() - failTime >= 10000) {
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
  		executeMovement(1, getShutterDelay()*1000); // forward direction, shutterdelay
    }

		if (hasExecutedMovement()) {
			incrementNrMovementsCompleted();
	    // make sure correct screen is displaying
	    if (getCurrentScreen() == "Auto") { // auto screen
	      auto_screen::printAutoStackProgress();
				auto_screen::estimateDuration();
	    }

			setShutterTriggered(false); // reset shutter triggered flag
			setExecutedMovement(false); // reset executed movement flag
		}

    if (triggerFailed) {
      triggerFailed = false;
      // 10 second delay to prevent trying to take another photo immediately after
      delay(10000);
    }
  }
  // stop AutoStack sequence if end reached
  if (getNrMovementsCompleted() >= getNrMovementsRequired()) {
    terminateAutoStack();
  }
}


// print various diagnostics to serial monitor
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


// run stepper until stall triggered
bool detectEndStop() {
  while (driver.event_stop_sg() == 0) {
    debugStallGuard(); // print to serial monitor
  }
  return true;
}


// run through the specified AutoStack procedure using the current start and end values
void dryRun() {
  // enable stepper if disabled
	if (!isStepperEnabled()) {
    setStepperEnabled(true);
  }
    // change to StealthChop is StallGuard is configured
  if (stallGuardConfigured) {
    configStealthChop();
  }

  // reduce stepper velocity
  setTargetVelocity(10000);
  // move to start position
  driver.XTARGET(getStartPosition());
  while (driver.XACTUAL() != driver.XTARGET()) {
    if (millis() - getLastMillis() >= 100) {
      config_screen::printPosition(); // update position
      setLastMillis(millis());
    }
  }

  // step slow through procedure
  setTargetVelocity(2000);
  // move to end position
  driver.XTARGET(getEndPosition());
  while (driver.XACTUAL() != driver.XTARGET()) {
    if (millis() - getLastMillis() >= 100) {
      config_screen::printPosition(); // update position
      setLastMillis(millis());
    }
  }

  setTargetVelocity(10000);
  // return to start
  driver.XTARGET(getStartPosition());
  while (driver.XACTUAL() != driver.XTARGET()) {
    if (millis() - getLastMillis() >= 100) {
      config_screen::printPosition(); // update position
      setLastMillis(millis());
    }
  }
  // overshoot by 3200 steps/1mm and return to start
  overshootPosition(getStartPosition(), 3200);
  setTargetVelocity(stealthChopMaxVelocity);
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
    // wait for stepper to reach target position
    while(driver.XACTUAL() != driver.XTARGET()) {}
    setExecutedMovement(true);  
    setLastStepTime(millis());
  }
  else {
		setExecutedMovement(false); // no step taken
	}
}


// find the forward and backward end stops of the linear rail using StallGuard
// moves stepper backwards until stall detected, records position
// moves stepper forwards until stall detected, records position and sets position as maximum value (384,000)
// moves stepper to middle of the rail (192,000)
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

    // Serial.print("Actual: "); Serial.println(driver.XACTUAL());

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

    // Serial.print("Actual: "); Serial.println(driver.XACTUAL());

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

    setStartPosition(driver.XACTUAL());   // update autoStack positions as rail has moved
    setEndPosition(driver.XACTUAL());     // update autoStack positions as rail has moved
		configStealthChop();  // set config for silentStep
    runHomingSequence = false;
    setRailHomed(true);
	}
}

// move stepper to start for AutoStack sequence
// if current position in front of start, overshoot to remove backlash and return
// otherwise move forward to start
void goToStart() {
  if (driver.XACTUAL() != getStartPosition()) {
    int distanceToStart = (driver.XACTUAL() - getStartPosition());
    // if current position is in front of start position
    if (distanceToStart > 0) {
      // move backwards to start postion
      // overshoot by 3200 steps/1mm and return to start to remove backlash
      overshootPosition(getStartPosition(), 3200);
    }
    // if current position is behind start position
    else if (distanceToStart < 0) {
      // move forward to start postion
      driver.XTARGET(getStartPosition());
      // wait for stepper to reach position
      while (driver.XACTUAL() != driver.XTARGET()) {}
    }
  }
  isNewAutoStack = false;
}


void overshootPosition(int startPosition, int numberOfSteps) {
  int offsetPosition = startPosition - numberOfSteps;
  // overshoot start position
  driver.XTARGET(offsetPosition);
  // wait for stepper to reach position
  while (driver.XACTUAL() != driver.XTARGET()) {}

  delay(50);

  // move to start
  driver.XTARGET(startPosition);
  // wait for stepper to reach position
  while (driver.XACTUAL() != driver.XTARGET()) {}
}

// clean up variables etc after completing AutoStack sequence
void terminateAutoStack() {
  if (getCurrentScreen() != "Auto") {
    populateScreen("Auto");           // go back to Auto screen if not already on it
  }
  auto_screen::estimateDuration();    // update estimate
  autoStackInitiated = false;
  isNewAutoStack = true;
  setShutterTriggered(false);
  autoStackPaused = false;            // autoStack is completed but not paused
  setNrMovementsCompleted(0);         // reset completed movements count
  setExecutedMovement(false);
  auto_screen::pauseStack();          // reset PlayPause button to "paused" mode"
  produceTone(4, 300, 200);           // sound 4 tones for 300ms separated by a 200ms delay
  // change max velocity back to normal
  setTargetVelocity(stealthChopMaxVelocity);
}
