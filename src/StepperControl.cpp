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
    toggleStepper(true);
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
            drawPlayPause(true, false); // grey out play, highlight pause
            flashScreen(); // load screen for checking flash settings/functionality
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
	    if (activeScreen == 3) { // auto screen
	      updateProgress(0); // don't force refresh
				estimateDuration(0); // don't force refresh
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
		estimateDuration(1); // force refresh
    autoStackFlag = false;
    goToStart = true;
    completedMovements = 0;
    joystickState = true;
		stepperMoved = false;
		shutterTriggered = false;
    drawPlayPause(false, false); // reset play to green
  }
}

/******************************************************************
Take 10 readings form the joystick X axis during setup and average
the result. This becomes the resting value for the joystick. If
out of the estimated range (500, 524), set at 512 to be safe.
Add/substract 10 to this resting value to get an upper and lower
bound for detecting movement. Also record difference between the
resting value and ideal mid-point (511.5) to offset xPOS readings
by in other functions that use this variable.
******************************************************************/
void calibrateJoyStick() {
  int numReadings = 10;
  int sumReadings = 0;

  for (int xth_reading = 0; xth_reading < numReadings; xth_reading++) {
    readJoystick();
    sumReadings += xStickPos; // add reading to total
    delay(10); // wait 10ms
  }

  xPosResting = (sumReadings / numReadings) + 0.5; // "round" to nearest whole number

  // if xPosresting below/above these values, set at middle point to be safe
  if (xPosResting < 500 || xPosResting > 524) {
    xPosResting = 512;
  }
  xPosUpper = xPosResting + 10;
  xPosLower = xPosResting - 10;
  xPosDiff = 512 - xPosResting;
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


/******************************************************************
Function for moving the stepper via joystick control on the X axis.
Re-enables stepper if previously disabled and then checks to make
sure the joystick values are still in the operation range.  If the
rail has been homed, it will prevent user from allowing rail to
bump into endstops. Every 50ms it checks for new joystick input
values and also adjusts speed based on joystick input position.
For the first 5000 loops of the while() function, a modifier is
used to reduce the initial acceleration of the stepper to control
jerk. After 5000 steps, modifier is redundant and normal speed
control resumes.
******************************************************************/
void joystickControl() {
  // count nr of loops in while() clause
  long loopNr = 0;
  int stepperSpeed = 0;

  // wake stepper from sleep
	if (stepperDisabled == true) {
    toggleStepper(true);
  }
  // while joystick is operated
  while (xStickPos >= xPosUpper || xStickPos <= xPosLower) {
    // if rail min/max postions have been set...
    if (homedRail == true) {
      // don't allow movement if within 1000 steps of forward stop
      // values are inverse so high xStickPos = reverse
      if (xStickPos >= xPosUpper && (abs(minPosition - stepper.currentPosition()) <= 1000)) {
        break;
      }
      // don't allow movement if within 1000 steps of backward stop
      // values are inverse so low xStickPos = forward
      if (xStickPos <= xPosLower && (abs(maxPosition - stepper.currentPosition()) <= 1000)) {
        break;
      }
    }
    // move either -1, 0, or 1 steps
    int step_dir = 0;
    if (xStickPos >= xPosUpper) step_dir = 1;
    else if (xStickPos <= xPosLower) step_dir = 1;
    else step_dir = 0;
    stepper.move(step_dir);

    // check stick position again and set speed
    if (millis() % 50 == 0) {
       readJoystick();
       stepperSpeed = speedControl(loopNr);

       // prevent overflow
       if (abs(loopNr) > 2000000000) {
         loopNr = rampSteps;
       }
    }

    stepper.setSpeed(stepperSpeed);
    stepper.runSpeed();

    loopNr++;
  }

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


void readJoystick() {
  if (screenRotated == false) {
    xStickPos = analogRead(XSTICK_PIN);
  }
  else if (screenRotated == true) {
    xStickPos = map(analogRead(XSTICK_PIN), 0, 1023, 1023, 0);
  }

  // offset reading by difference between resting state and ideal middle point
  xStickPos -= xPosDiff;
}


int speedControl(long loopNr) {
  int jerkControl = rampSteps;
  int adjustedSpeed = 0;

  // if exceeded 5000 loops, jerkControl multiplier = 1
  if (loopNr >= jerkControl) {
    loopNr = jerkControl;
  }

  // adjust input range based on diff between resting point and ideal middle
  joyStickSpeed = map(xStickPos, (0 - xPosDiff), (1023 - xPosDiff), stepperMaxSpeed, -stepperMaxSpeed);

  // adjust speed based on jerkControl multiplier;
  float speedModifier = loopNr*1.00 / jerkControl*1.00;
  adjustedSpeed = joyStickSpeed * speedModifier;

  return adjustedSpeed;
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
		if (digitalRead(EN_PIN) == HIGH) {
	    toggleStepper(true);
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
