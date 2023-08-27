#include "DriverConfig.h"
#include "GlobalVariables.h"
#include "MiscFunctions.h"
#include "StepperControl.h"
#include "ShutterControl.h"
#include "menu/UI-Main.h"
#include "menu/UI-Auto.h"
#include "menu/UI-AutoConfig.h"
#include "menu/UI-Global.h"
#include "menu/UI-Photo360.h"

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
void autoStack(TMC5160Stepper_Ext &driver) {
  readyStealthChop(driver);
  
  if (isNewAutoStack) {                     // move to start position if beginning AutoStack
    setTargetVelocity(10000);               // reduce max velocity to minimize vibration
    goToStart(driver);
    setNrMovementsCompleted(0);
    driver.executedMovement(false);
    setShutterTriggered(false);
    setLastStepTime(millis());
    global::func_Reset(false);              // change reset button to red
    auto_screen::stackStatus(start);
  }

  if (getNrMovementsCompleted() <= getNrMovementsRequired() && !driver.executedMovement()) {
     // take photo if there's been >= 500ms since a movement was executed successfully (gives time for vibration to settle)
		if (isShutterEnabled() && !hasShutterTriggered() && millis() - getLastStepTime() >= 500) {
      // if flashProcedure idle or is successful, start new procedure or trigger flash if !isFlashSensorEnabled
      if (getCurrentStage() == start || getCurrentStage() == newStep) {
        triggerShutter();                   // take photo and trigger flash immediately or start flash procedure
      }
      else {
        runFlashProcedure(false);           // keep trying to fire flash
      }
      if (getCurrentStage() == flashUnresponsive) {
        auto_screen::stackStatus(newStep);  // reset flash procedure
        auto_screen::pauseStack();          // pause autostack so user can troubleshoot flash issue
        populateScreen("Flash");            // go to flashScreen if can't trigger after 10 seconds
      }
		}
    // only move if autoStack hasn't been paused by flash failure
    if (!autoStackPaused && (!isShutterEnabled() || hasShutterTriggered())) {
      unsigned long delay = getShutterDelay();
  		executeMovement(driver, 1, delay);
    }

		if (driver.executedMovement()) {
			incrementNrMovementsCompleted();
      auto_screen::stackStatus(newStep);
	    if (getCurrentScreen() == "Auto") {   // make sure correct screen is displaying
	      auto_screen::printAutoStackProgress();
				auto_screen::estimateDuration();
	    }
			setShutterTriggered(false);           // reset shutter triggered flag
			driver.executedMovement(false);       // reset executed movement flag
		}
  }
  if (getNrMovementsCompleted() >= getNrMovementsRequired()) {
    auto_screen::stackStatus(stackCompleted);
    terminateAutoStack(driver);                   // stop AutoStack sequence if end reached
  }
}


// print various diagnostics to serial monitor
void debugStallGuard(TMC5160Stepper_Ext &driver) {
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
bool detectEndStop(TMC5160Stepper_Ext &driver) {
  while (driver.event_stop_sg() == 0) {
    debugStallGuard(driver); // print to serial monitor
  }
  return true;
}


// run through the specified AutoStack procedure using the current start and end values
void dryRun(TMC5160Stepper_Ext &driver) {
  readyStealthChop(driver);

  // reduce stepper velocity
  setTargetVelocity(5000);
  // move to start position
  driver.XTARGET(getStartPosition());
  while (driver.XACTUAL() != driver.XTARGET()) {
    if (millis() - getLastMillis() >= 100) {
      autoconfig_screen::printPosition(); // update position
      setLastMillis(millis());
    }
  }

  // step slow through procedure
  setTargetVelocity(500);
  // move to end position
  driver.XTARGET(getEndPosition());
  while (driver.XACTUAL() != driver.XTARGET()) {
    if (millis() - getLastMillis() >= 100) {
      autoconfig_screen::printPosition(); // update position
      setLastMillis(millis());
    }
  }

  setTargetVelocity(5000);
  // return to start
  driver.XTARGET(getStartPosition());
  while (driver.XACTUAL() != driver.XTARGET()) {
    if (millis() - getLastMillis() >= 100) {
      autoconfig_screen::printPosition(); // update position
      setLastMillis(millis());
    }
  }
  // overshoot by 3200 steps/1mm and return to start
  overshootPosition(getStartPosition(), 3200);
  setTargetVelocity(STEALTH_CHOP_VMAX);
}


/******************************************************************
Moves the stepper one Movement in a given direction. A Movement
is the total number of steps required to travel a given distance.
If there hasn't been a specified delay since the previous Movement,
this function returns false and the calling function should loop
until it receives a response of true. Prevents stall if rail has
been homed and will exit autoStack if running.
******************************************************************/
void executeMovement(TMC5160Stepper_Ext &driver, int stepDirection, unsigned long stepperDelay) {
  readyStealthChop(driver);

  // step after elapsed amount of time
  if ((millis() - getLastStepTime() > stepperDelay)) {
    auto_screen::stackStatus(stepTaken);
    int stepVelocity = stepDirection * getStepsPerMovement();
    long targetPosition = driver.XACTUAL() + stepVelocity;
    // set new target position
    driver.XTARGET(targetPosition);
    Serial.print("Actual before: "); Serial.println(driver.XACTUAL());
    Serial.print("Target before: "); Serial.println(driver.XTARGET());
    // reduce speed
    setTargetVelocity(2000);
    // wait for stepper to reach target position
    while(driver.XACTUAL() != driver.XTARGET()) {
      Serial.print("Actual after: "); Serial.println(driver.XACTUAL());
      Serial.print("Target after: "); Serial.println(driver.XTARGET());
    }
    setTargetVelocity(STEALTH_CHOP_VMAX);
    driver.executedMovement(true);  
    setLastStepTime(millis());
  }
  else {
		driver.executedMovement(false);
    auto_screen::stackStatus(stepDelay);
	}
}


// execute a predetermined number of steps
void executeSteps(TMC5160Stepper_Ext &driver, long nrSteps) {
  readyStealthChop(driver);
  long targetPosition = driver.XACTUAL() + nrSteps;
  // set new target position
  driver.XTARGET(targetPosition);
  // wait for stepper to reach target position
  while(driver.XACTUAL() != driver.XTARGET()) {}
}


// find the forward and backward end stops of the linear rail using StallGuard
// moves stepper backwards until stall detected, records position
// moves stepper forwards until stall detected, records position and sets position as maximum value (384,000)
// moves stepper to middle of the rail (192,000)
void homeRail(TMC5160Stepper_Ext &driver) {
  bool hitRearStop, hitForwardStop = false;
  readyStallGuard(driver);
  // reset positions
  setForwardEndStop(1);
  setBackwardEndStop(1);

  // move to rear end stop + 100,000 just to ensure it over-runs without decelerating
	driver.XTARGET(-MAX_RAIL_POSITION - 100000);
  hitRearStop = detectEndStop(driver); // wait until end stop detected
  // if endstop detected... 
  if (hitRearStop) {
    delay(100);
    driver.sg_stop(false);
    driver.sg_stop(true);

    // Serial.print("Actual: "); Serial.println(driver1.XACTUAL());

    driver1.XACTUAL(0); // set rear end stop position as 0
    setBackwardEndStop(driver.XACTUAL()); // set backward position
  }
  // move to forward end stop
  driver.XTARGET(MAX_RAIL_POSITION + 100000);
  hitForwardStop = detectEndStop(driver); // wait until end stop detected
  // if endstop detected...
  if (hitForwardStop) {
    delay(100);
    driver.sg_stop(false);
    driver.sg_stop(true);

    // Serial.print("Actual: "); Serial.println(driver1.XACTUAL());

    driver.XACTUAL(MAX_RAIL_POSITION); // set front end stop position as 0
    setForwardEndStop(driver.XACTUAL()); // set forward position
  }

	// if back and forward position set, move to middle position
	if (getBackwardEndStop() == MIN_RAIL_POSITION && getForwardEndStop() == MAX_RAIL_POSITION) {
    driver.XTARGET(192000);
    // wait to reach position before changing driver config after homing completed
    while (driver.XACTUAL() != driver.XTARGET()) {
      debugStallGuard(driver); // print to serial monitor
    }

    setStartPosition(driver.XACTUAL());  // update autoStack positions as rail has moved
    setEndPosition(driver.XACTUAL());    // update autoStack positions as rail has moved
		configStealthChop(driver);           // set config for silentStep
    runHomingSequence = false;
    setRailHomed(true);
	}
}

// move stepper to start for AutoStack sequence
// if current position in front of start, overshoot to remove backlash and return
// otherwise move forward to start
void goToStart(TMC5160Stepper_Ext &driver) {
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


void overshootPosition(TMC5160Stepper_Ext &driver, int startPosition, int numberOfSteps) {
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
void terminateAutoStack(TMC5160Stepper_Ext &driver) {
  if (getCurrentScreen() != "Auto") {
    populateScreen("Auto");                   // go back to Auto screen if not already on it
  }
  auto_screen::resetStack();                  // update button and reset button bitmap
  auto_screen::estimateDuration();            // update estimate
  global::btn_Reset.updateColour(BLACK);      // change reset button back to black
  setShutterTriggered(false);
  setNrMovementsCompleted(0);                 // reset completed movements count
  driver.executedMovement(false);
  produceTone(4, 300, 200);                   // sound 4 tones for 300ms separated by a 200ms delay
  // change max velocity back to normal
  setTargetVelocity(STEALTH_CHOP_VMAX);
}


void video360(TMC5160Stepper_Ext &driver, long nrSteps) {
  // change to StealthChop if StallGuard is configured
  readyStealthChop(driver);
  // update target velocity as calling configStealthChop resets this on the driver register but not the function
  setTargetVelocity(getTargetVelocity());
  // set position
  driver.XTARGET(driver.XACTUAL()+nrSteps);
}


void photo360(TMC5160Stepper_Ext &driver) {
    setTargetVelocity(360); // approx. 5rpm
    // 0 - if start of new photo360, set speed, take first photo, and enable stepper
    if (getCurrentStage() == start && isNewPhoto360 && photo360Initiated) {
        // Serial.print("STARTED: "); Serial.println(millis());
        // reset target to be safe
        driver.XTARGET(driver.XACTUAL());
        // change to StealthChop if StallGuard is configured
        readyStealthChop(driver);
        setCurrentStage(pullShutter);
        global::func_Reset(false); // change reset button to red
    }

    if (photo360Initiated && !photo360Paused) {
      // if not waiting for shutter and we are at the target, trigger flash
      // STEP 1: trigger photo
      if (getCurrentStage() == pullShutter && hasReachedTargetPosition()) {
        // wait for delay, this should be skipped on the first photo
        if (millis() - getLastPhoto360Step() >= getPhoto360Delay()) {
          // take photo
          // Serial.print("PULL SHUTTER: "); Serial.println(millis());
          digitalWrite(SONY_PIN, HIGH);
          setFlashTriggerTime(millis());
          // set this at the start of the "loop" (pull, release, move)
          setLastPhoto360Step();
          setCurrentStage(releaseShutter);
        }
      }

      // if shutter hasn't triggered yet, keep retrying until 800ms has passed
      if (getCurrentStage() == releaseShutter) {
        if (millis() - getFlashTriggerTime() >= 800) {
          // Serial.print("RELEASE SHUTTER: "); Serial.println(millis());
          digitalWrite(SONY_PIN, LOW);
          // increment nr completed 360 photos
          setNrCompleted360Photos(getNrCompleted360Photos()+1);
          if (getCurrentScreen() == "Photo360") {
            photo_screen::printPhoto360Progress();
          }
          setCurrentStage(newStep);
        }
      }

      // STEP 2: move to new target if time since last step has passed the delay period
      if (getCurrentStage() == newStep) {
        // wait for delay
        if (millis() - getLastPhoto360Step() >= getPhoto360Delay()/2) {
          // Serial.print("STEP TO TARGET: "); Serial.println(millis());
          // calculate number of microsteps to move for each photo, will truncate down to an int
          int nr360Steps = (ORBIS_MOTOR_STEPS*NR_MICROSTEPS) / getNr360Photos();
          int remainder = (ORBIS_MOTOR_STEPS*NR_MICROSTEPS) % getNr360Photos();
          // if there is a remainder, add a 1 step to the same number of photos as the size of the remainder
          // e.g. if remainder is 80 steps, add 1 step to the last 80 photos in the photo360 procedure
          if (getNrCompleted360Photos() > getNr360Photos()-remainder) {
            nr360Steps++;
          }
          // if not clockwise, reverse direction of travel
          if (!driver.rotateClockwise()) {
            nr360Steps = nr360Steps*-1;
          }
          driver.XTARGET(driver.XACTUAL() + nr360Steps);
          setCurrentStage(stepTaken);
        }
      }

      // STEP 3: if arrived at target, take photo and loop back through steps
      if (getCurrentStage() == stepTaken && hasReachedTargetPosition()) {
        // Serial.print("AT TARGET: "); Serial.println(millis());
        setCurrentStage(pullShutter);
      }

      // photo360 completed, previous `pullShutter` state will be overwritten by the reset to idle
      if (getNrCompleted360Photos() == getNr360Photos()) {
        // reset flags
        photo_screen::resetPhoto360();
        // Serial.print("COMPLETED: "); Serial.println(millis());
      }
    }
}


void readyStallGuard(TMC5160Stepper_Ext &driver) {
  // enable stepper if disabled
	if (!driver.stepperEnabled()) {
    driver.stepperEnabled(true);
  }
  // change to StallGuard is StealthChop is configured
  if (!driver.stallGuardActive()) {
    configStallGuard(driver);
  }
}


void readyStealthChop(TMC5160Stepper_Ext &driver) {
  // enable stepper if disabled
	if (!driver.stepperEnabled()) {
    driver.stepperEnabled(true);
  }
  // change to StealthChop is StallGuard is configured
  if (driver.stallGuardActive()) {
    configStealthChop(driver);
  }
}