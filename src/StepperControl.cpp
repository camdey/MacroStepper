#include "DriverConfig.h"
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
  if (stepperDisabled == true) {
    toggleStepper(true);
  }

  // move stepper to start if not already there
  if (goToStart == true) {
    if (stepper.currentPosition() != startPosition) {
      int distanceToStart = (stepper.currentPosition() - startPosition);
      // if current position is in front of start position
      if (distanceToStart > 0) {
        // move backwards to start postion
        // overshoot by 200 steps and return to start
        overshootPosition(startPosition, 200, -1);
      }
      // if current position is behind start position
      else if (distanceToStart < 0) {
        // move forward to start postion
        while (stepper.currentPosition() != startPosition) {
          stepper.setSpeed(600);
          stepper.moveTo(startPosition);
          stepper.runSpeedToPosition();
        }
      }
    }
		goToStart = false;
		completedMovements = 0;
		joystickState = false;
		stepperMoved = false;
		shutterTriggered = false;
    prevStepTime = millis();
    // Serial.println("0. Start");
  }

  // take photo and increment progress for first step of each movement
  if (completedMovements <= movementsRequired && stepperMoved == false) {

    // if shutter enabled, take photo
		if (shutterEnabled == true && shutterTriggered == false) {

	    shutterTriggered = triggerShutter(); // take photo

      // pause autoStack if flash failing
      if (shutterTriggered == false) {
        long failTime = millis();
        triggerFailed = true; // used to set one-off delay later
        // Serial.println("fail to trigger");

        while (shutterTriggered == false) {
          shutterTriggered = triggerShutter();
          // if retry successful, break
          if (shutterTriggered == true) {
            // Serial.println("retry successful");
            break;
          }
          // go to flashScreen if can't trigger after 10 seconds
          else if (millis() - failTime >= 10000) {
            // Serial.println("go to flash screen");
            pauseAutoStack = true; // pause autostack
            auto_screen::drawPlayPause(true, false); // grey out play, highlight pause
            populateScreen("Flash"); // load screen for checking flash settings/functionality
            break;
          }
        }
      }
		}

    // only move if autoStack hasn't been paused by flash failure
    if (pauseAutoStack != true) {
  		// move stepper
  		stepperMoved = stepMotor(1, shutterDelay*1000); // forward direction, shutterdelay
    }

		if (stepperMoved == true) {
			completedMovements++;
	    // make sure correct screen is displaying
	    if (getCurrentScreen() == "Auto") { // auto screen
	      auto_screen::updateProgress(0); // don't force refresh
				auto_screen::estimateDuration(0); // don't force refresh
	    }

			shutterTriggered = false; // reset shutter
			stepperMoved = false; // reset stepper move
		}

    if (triggerFailed == true) {
      // 10 second delay to prevent trying to take another photo immediately after
      delay(10000);
    }
  }
  // stop AutoStack sequence if end reached
  if (completedMovements >= movementsRequired) {
		auto_screen::estimateDuration(1); // force refresh
    autoStackFlag = false;
    goToStart = true;
    completedMovements = 0;
    joystickState = true;
		stepperMoved = false;
		shutterTriggered = false;
    auto_screen::drawPlayPause(false, false); // reset play to green
  }
}


void changeDirection() {
  if (directionFwd == true) {
    moveDist = -500000;
    directionFwd = false;
  }
  else if (directionFwd == false) {
    moveDist = 500000;
    directionFwd = true;
  }
  stepper.moveTo(moveDist);
}


void dryRun() {
  // wake stepper from sleep
	if (digitalRead(EN_PIN) == HIGH) {
    toggleStepper(true);
  }

  // move backwards to start
  while (stepper.currentPosition() > startPosition) {
    stepper.setSpeed(-600);
    stepper.moveTo(startPosition);
    stepper.runSpeedToPosition();
  }
  config_screen::displayPosition();

  // step slow through procedure
  while (stepper.currentPosition() < endPosition) {
    stepper.setSpeed(100);
    stepper.moveTo(endPosition);
    stepper.runSpeedToPosition();
  }
  config_screen::displayPosition();

  // return to start
  if (stepper.currentPosition() > startPosition) {
    // overshoot by 200 steps and return to start
    overshootPosition(startPosition, 200, -1);
  }

  // stops motor instantly
  stepper.setSpeed(0);
  targetFlag = true;
  completedMovements = 0;
  config_screen::displayPosition();
}


void homeRail() {
  if (stepperDisabled == true) {
    toggleStepper(true);
  }
	// set configuration for stallGuard
	if (stallGuardConfigured == false) {
		stallGuardConfig();
	}
  // reset positions
  fwdPosition = 1;
  bwdPosition = 1;
  // move to end of rail
	stepper.moveTo(moveDist);

	// if in fwd direction, reverse direction
	if (directionFwd == true) {
		changeDirection();
	}
	// while back position not set and forward position not set, run to -500000 or stall
	// after stall, change direction and run to 500000 or stall
  // 1mm pitch * 4 microsteps * 200 steps/rev * 73.1mm travel = 58,000 + steps
	while (bwdPosition != 0 || fwdPosition <= 50000) {
		stepper.run();
    // int sg_result = driver.sg_result();
    // if (sg_result <= 50) {
    //   Serial.print("SG Result: ");
    //   Serial.println(sg_result);
    // }
	}
	// if back and forward position set, move to middle position
	if (bwdPosition == 0 && fwdPosition > 10000) {
		runHomingSequence = false;
    // in case of hang-up on stall, set current position to fwdPosition before moving off
    stepper.setCurrentPosition(fwdPosition);
		stepper.moveTo(fwdPosition / 2);
		while (stepper.distanceToGo() != 0) {
			stepper.run();
		}
	}
  homedRail = true;
  // TODO - does this need to be reset to false at some point?
  firstFwdStall = true; // debouncer so that first position at stall is recorded
  // config silentStep after homing
  silentStepConfig();
}


void overshootPosition(int position, int numberOfSteps, int direction) {
  int speed = 600 * direction;
  int offsetPosition = position - numberOfSteps;

  while (stepper.currentPosition() != offsetPosition) {
    stepper.setSpeed(speed);
    stepper.moveTo(offsetPosition); // move past start
    stepper.runSpeedToPosition();
  }
  delay(50);
  while (stepper.currentPosition() != position) {
    stepper.setSpeed(-speed); // reverse direcyion
    stepper.moveTo(position); // move to start
    stepper.runSpeedToPosition();
  }
}


void stallDetection() {
  // Serial.print("DIAG: ");
  // Serial.println(digitalRead(DIAG1_PIN));

  // only continue if triggered during home sequence
  if (runHomingSequence == true) {
    toggleStepper(false); // disable stepper
    stepper.setSpeed(0); // set speed to 0 for reset
    if (directionFwd == false) { // if hit rear end stop, set position as 0
      stepper.setCurrentPosition(0); // set position for accelStepper
      bwdPosition = stepper.currentPosition(); // set variable position
    }
    else if (directionFwd == true && firstFwdStall == true) {
      fwdPosition = stepper.currentPosition(); // set forward position
    }
    toggleStepper(true); // enable stepper
    changeDirection(); // change direction
  }
}


/******************************************************************
Moves the stepper one Movement in a given direction. A Movement
is the total number of steps required to travel a given distance.
If there hasn't been a specified delay since the previous Movement,
this function returns false and the calling function should loop
until it receives a response of true. Prevents stall if rail has
been homed and will exit autoStack if running.
******************************************************************/
bool stepMotor(int stepDirection, unsigned long stepperDelay) {
  bool stallWarning = false;
  currentTime = millis();

  // trigger stall warning if approaching limits
  if (stepDirection == 1 && (abs(maxPosition - stepper.currentPosition()) <= 1000) && homedRail == true) {
    stallWarning = true;
    return true;
  }

  // trigger stall warning if approaching limits
  if (stepDirection == -1 && (abs(minPosition - stepper.currentPosition()) <= 1000) && homedRail == true) {
    stallWarning = true;
    return true;
  }

  // exit out of autoStack sequence if stall warning encountered
  if (autoStackFlag == true && stallWarning == true) {
    autoStackFlag = false;
  }

  // step after elapsed amount of time
  if ((currentTime - prevStepTime > stepperDelay) && stallWarning == false) {
    // wake stepper from sleep
		if (stepperDisabled == true) {
	    toggleStepper(true);
	  }

    int stepVelocity = stepDirection * stepsPerMovement;

    stepper.move(stepVelocity);
		stepper.setSpeed(200*stepDirection);
		while (stepper.distanceToGo() != 0) {
	    stepper.runSpeed();
		}

    prevStepTime = currentTime;
		return true;
  }
  else {
		return false; // no step taken
	}
}


void toggleStepper(bool enable) {
	if (enable == true) {
		// delay(10); // give time for last step to complete, may need to make optional as called during ISR
		stepper.enableOutputs();
		stepperDisabled = false;
		// delay(10); // breathing space
	}

	if (enable == false) {
		stepper.setSpeed(0);
		stepper.move(0);
		stepper.disableOutputs();
		stepperDisabled = true;
	}
}
