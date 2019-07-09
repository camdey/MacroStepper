#include "DriverConfig.h"
#include "MiscFunctions.h"
#include "StepperControl.h"
#include "ShutterControl.h"
#include "UserInterface.h"

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
  if (digitalRead(EN_PIN) == HIGH) {
    toggleStepper(1);
  }
  // move stepper to start if not already there
  if (goToStart == true) {
    while (stepper.currentPosition() != startPosition) {
      // move to start
      stepper.setSpeed(-600);
      stepper.moveTo(startPosition);
      stepper.runSpeedToPosition();
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
      // Serial.println("1. Ready");
	    shutterTriggered = triggerShutter(); // take photo
      if (shutterTriggered == true){
        // Serial.println("2. Photo Taken");
      }
		}

		// move stepper
		stepperMoved = stepMotor(1, shutterDelay*1000); // forward direction, shutterdelay

		if (stepperMoved == true) {
      // Serial.println("3. Step Taken");
			completedMovements++;
	    // make sure correct screen is displaying
	    if (activeScreen == 3) { // auto screen
	      updateProgress(0); // don't force refresh
				estimateDuration(0); // don't force refresh
	    }

			shutterTriggered = false; // reset shutter
			stepperMoved = false; // reset stepper move
		}
  }
  // stop AutoStack sequence if end reached
  if (completedMovements >= movementsRequired) {
		estimateDuration(1); // force refresh
    autoStackFlag = false;
    goToStart = true;
    completedMovements = 0;
    joystickState = true;
		stepperMoved = false;
		shutterTriggered = false;
    drawPlayPause(0, 0); // reset play to green
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
  // Serial.print("Current Pos: ");
  // Serial.println(stepper.currentPosition());
}

void dryRun() {
  // wake stepper from sleep
	if (digitalRead(EN_PIN) == HIGH) {
    toggleStepper(1);
  }

  // move to start
  while (stepper.currentPosition() > startPosition) {
    stepper.setSpeed(-600);
    stepper.moveTo(startPosition);
    stepper.runSpeedToPosition();
  }
  displayPosition();

  // step slow through procedure
  while (stepper.currentPosition() < endPosition) {
    stepper.setSpeed(100);
    stepper.moveTo(endPosition);
    stepper.runSpeedToPosition();
  }
  displayPosition();

  // return to start
  while (stepper.currentPosition() > startPosition) {
    stepper.setSpeed(-600);
    stepper.moveTo(startPosition);
    stepper.runSpeedToPosition();
  }

  // stops motor instantly
  stepper.setSpeed(0);
  targetFlag = true;
  completedMovements = 0;
  displayPosition();
}

void homeRail() {
  if (stepperDisabled == true) {
    toggleStepper(1);
  }
	// set configuration for stallGuard
	if (stallGuardConfigured == false) {
		stallGuardConfig();
	}
  // reset positions
  fwdPosition = 0;
  bwdPosition = 0;
  // move to end of rail
	stepper.moveTo(moveDist);

	// if in fwd direction, reverse direction
	if (directionFwd == true) {
		changeDirection();
	}
	// while back position not set and forward position not set, run to -50000 or stall
	// after stall, change direction and run to 50000 or stall
	while (bwdPosition != 0 || fwdPosition <= 9999) {
		stepper.run();
    // int sg_result = driver.sg_result();
    // if (sg_result <= 50) {
    //   Serial.print("SG Result: ");
    //   Serial.println(sg_result);
    // }
	}
	// if back and forward position set, move to middle position
	if (bwdPosition == 0 && fwdPosition > 10000) {
		bootFlag = false;
    // in case of hang-up on stall, set current position to fwdPosition before moving off
    stepper.setCurrentPosition(fwdPosition);
		stepper.moveTo(fwdPosition / 2);
		while (stepper.distanceToGo() != 0) {
			stepper.run();
		}
	}
  homedRail = true;
  firstFwdStall = true; // reset first stall check in case homeRail run again
  // config silentStep after homing
  silentStepConfig();
}

void joyStick() {
  // wake stepper from sleep
	if (stepperDisabled == true) {
    toggleStepper(1);
  }
  // while joystick is operated
  stepper.setAcceleration(500);
  while (xStickPos >= xStickUpper || xStickPos <= xStickLower) {
    // if rail min/max postions have been set...
    if (homedRail == true) {
      // don't allow movement if within 1000 steps of forward stop
      // values are inverse so high xStickPos = reverse
      if (xStickPos >= xStickUpper && (abs(minPosition - stepper.currentPosition()) <= 1000)) {
        break;
      }
      // don't allow movement if within 1000 steps of backward stop
      // values are inverse so low xStickPos = forward
      if (xStickPos <= xStickLower && (abs(maxPosition - stepper.currentPosition()) <= 1000)) {
        break;
      }
    }
    // move either -1, 0, or 1 steps
    stepper.move(map(xStickPos, 0, 1023, 1, -1));
		// xStickMid = resting stable point of joystick§
    joyStickSpeed = map((xStickPos - xStickMid), -xStickMid, xStickMid, 2000, -2000);
    stepper.setSpeed(joyStickSpeed);
    stepper.runSpeed();

    // check stick position again
    if (millis() % 50 == 0) {
       xStickPos = analogRead(XSTICK_PIN);
    }
  }
  stepper.setAcceleration(2000);

  // // check start/end position adjustment
  if (editStartPosition == true && arrowsActive == true) {
    if (prevStartPosition != startPosition) {
      prevStartPosition = startPosition;
    }
    setAutoStackPositions(true, false); //set start but not end position
  }
  if (editEndPosition == true && arrowsActive == true) {
    if (prevEndPosition != endPosition) {
      prevEndPosition = endPosition;
    }
    setAutoStackPositions(false, true); //set end but not start position
  }
  if (activeScreen == 2 || activeScreen == 4) {
    displayPosition();
  }
  // update prev position
  prevStepperPosition = stepper.currentPosition();
}

void stallDetection() {
  Serial.print("DIAG: ");
  Serial.println(digitalRead(DIAG_PIN));

  if (bootFlag == true) {
    toggleStepper(0);
    stepper.setSpeed(0);
    if (directionFwd == false) {
      stepper.setCurrentPosition(0);
      bwdPosition = stepper.currentPosition();
    }
    if (directionFwd == true && firstFwdStall == true) {
      fwdPosition = stepper.currentPosition();
    }
    toggleStepper(1);
    changeDirection();
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
		if (digitalRead(EN_PIN) == HIGH) {
	    toggleStepper(1);
	  }

    int stepVelocity = stepDirection * stepsPerMovement;

    stepper.move(stepVelocity);
		stepper.setSpeed(200*stepDirection);
		while (stepper.distanceToGo() != 0) {
	    stepper.runSpeed();
			// stepDue = true;
		}

    prevStepTime = currentTime;
		return true;
  } else {
			return false;
	}
}

void toggleStepper(bool enable) {
	if (enable == 1) {
		delay(10); // give time for last step to complete
		stepper.enableOutputs();
		stepperDisabled = false;
		delay(10); // breathing space
	}

	if (enable == 0) {
		stepper.setSpeed(0);
		stepper.move(0);
		stepper.disableOutputs();
		stepperDisabled = true;
	}
}