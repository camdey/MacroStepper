#include "DriverConfig.h"
#include "MiscFunctions.h"
#include "StepperControl.h"
#include "ShutterControl.h"
#include "UserInterface.h"

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
		movementProgress = false;
		joystickState = false;
		stepperMoved = false;
		shutterTriggered = false;
    // stepper.move(0);
    // stepper.setSpeed(0);
  }
  // take photo and increment progress for first step of each movement
  if (movementProgress <= numMovements && stepperMoved == false) {
		// if shutter enabled, take photo
		if (shutterState == true && shutterTriggered == false) {
	    shutterTriggered = triggerShutter(); // take photo
		}

		// move stepper
		stepperMoved = stepperStep(1, shutterDelay*1000); // forward direction, shutterdelay

		if (stepperMoved == true) {
			movementProgress++;
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
  if (movementProgress >= numMovements) {
		estimateDuration(1); // force refresh
    autoStackFlag = false;
    goToStart = true;
    movementProgress = false;
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
  movementProgress = false;
  displayPosition();
}

void homeRail() {
	// set configuration for stallGuard
	if (stallGuardConfigured == false) {
		stallGuardConfig();
	}
	stepper.moveTo(moveDist);
	// if in fwd direction, reverse direction
	if (directionFwd == true) {
		changeDirection();
	}
	// while back position not set and forward position not set, run to -50000 or stall
	// after stall, change direction and run to 50000 or stall
	while (bwdPosition != 0 || fwdPosition <= 9999) {
		stepper.run();
	}
	// if back and forward position set, move to middle position
	if (bwdPosition == 0 && fwdPosition > 10000) {
		bootFlag = false;
		stepper.moveTo(fwdPosition / 2);
		while (stepper.distanceToGo() != 0) {
			stepper.run();
		}
	}
}

void joyStick() {
  // wake stepper from sleep
	if (stepperDisabled == true) {
    toggleStepper(1);
  }
  // while joystick is operated
  while (xStickPos >= xStickUpper || xStickPos <= xStickLower) {
    // move either -1, 0, or 1 steps
    stepper.move(map(xStickPos, 0, 1023, 1, -1));
		// xStickMid = resting stable point of joystick§
    joystickSpeed = map((xStickPos - xStickMid), -xStickMid, xStickMid, 2000, -2000);

    stepper.setSpeed(joystickSpeed);
    stepper.runSpeed();

    // check stick position again
    if (millis() % 50 == 0) {
       xStickPos = analogRead(XSTICK_PIN);
    }
  }

  // // check start/end position adjustment
  if (editStartPosition == true && arrowsActive == true) {
    if (prevStartPosition != startPosition) {
      prevStartPosition = startPosition;
    }
    setAutoStackPositions(1, 0); //set start but not end position
  }
  if (editEndPosition == true && arrowsActive == true) {
    if (prevEndPosition != endPosition) {
      prevEndPosition = endPosition;
    }
    setAutoStackPositions(0, 1); //set end but not start position
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
    if (directionFwd == true) {
      fwdPosition = stepper.currentPosition();
    }
    toggleStepper(1);
    changeDirection();
  }
}

bool stepperStep(int stepDirection, unsigned long stepperDelay) {
  currentTime = millis();

  // step after elapsed amount of time
  if (currentTime - prevStepTime > stepperDelay) {
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
