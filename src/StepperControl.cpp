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
void autoStack() {
  if (!isStepperEnabled()) {
    setStepperEnabled(true);                // wake stepper from sleep
  }
  
  if (isNewAutoStack) {                     // move to start position if beginning AutoStack
    setTargetVelocity(10000);               // reduce max velocity to minimize vibration
    goToStart();
    setNrMovementsCompleted(0);
    setExecutedMovement(false);
    setShutterTriggered(false);
    setLastStepTime(millis());
    global::func_Reset(false);              // change reset button to red
    auto_screen::stackStatus(stack::start);
  }

  if (getNrMovementsCompleted() <= getNrMovementsRequired() && !hasExecutedMovement()) {
     // take photo if there's been >= 500ms since a movement was executed successfully (gives time for vibration to settle)
		if (isShutterEnabled() && !hasShutterTriggered() && millis() - getLastStepTime() >= 500) {
      // if flashProcedure idle or is successful, start new procedure or trigger flash if !isFlashSensorEnabled
      if (getStackStage() == stack::start || getStackStage() == stack::newStep) {
        triggerShutter();                   // take photo and trigger flash immediately or start flash procedure
      }
      else {
        runFlashProcedure(false);           // keep trying to fire flash
      }
      if (getStackStage() == stack::flashUnresponsive) {
        auto_screen::stackStatus(stack::newStep);  // reset flash procedure
        auto_screen::pauseStack();          // pause autostack so user can troubleshoot flash issue
        populateScreen("Flash");            // go to flashScreen if can't trigger after 10 seconds
      }
		}
    // only move if autoStack hasn't been paused by flash failure
    if (!autoStackPaused && (!isShutterEnabled() || hasShutterTriggered())) {
      unsigned long delay = getShutterDelay()*1000;
  		executeMovement(1, delay);
    }

		if (hasExecutedMovement()) {
			incrementNrMovementsCompleted();
      auto_screen::stackStatus(stack::newStep);
	    if (getCurrentScreen() == "Auto") {   // make sure correct screen is displaying
	      auto_screen::printAutoStackProgress();
				auto_screen::estimateDuration();
	    }
			setShutterTriggered(false);           // reset shutter triggered flag
			setExecutedMovement(false);           // reset executed movement flag
		}
  }
  if (getNrMovementsCompleted() >= getNrMovementsRequired()) {
    auto_screen::stackStatus(stack::stackCompleted);
    terminateAutoStack();                   // stop AutoStack sequence if end reached
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
void executeMovement(int stepDirection, unsigned long stepperDelay) {
  // change to StealthChop is StallGuard is configured
  if (stallGuardConfigured) {
    configStealthChop();
  }

  // step after elapsed amount of time
  if ((millis() - getLastStepTime() > stepperDelay)) {
    auto_screen::stackStatus(stack::stepTaken);
    // enable stepper if currently disabled
		if (!isStepperEnabled()) {
	    setStepperEnabled(true);
	  }

    int stepVelocity = stepDirection * getStepsPerMovement();
    long targetPosition = driver.XACTUAL() + stepVelocity;

    // set new target position
    driver.XTARGET(targetPosition);
    // reduce speed
    setTargetVelocity(2000);
    // wait for stepper to reach target position
    while(driver.XACTUAL() != driver.XTARGET()) {}
    setTargetVelocity(STEALTH_CHOP_VMAX);
    setExecutedMovement(true);  
    setLastStepTime(millis());
  }
  else {
		setExecutedMovement(false); // no step taken
    auto_screen::stackStatus(stack::stepDelay);
	}
}


// execute a predetermined number of steps
void executeSteps(long nrSteps) {
  // change to StealthChop is StallGuard is configured
  if (stallGuardConfigured) {
    configStealthChop();
  }
  if (!isStepperEnabled()) {
    setStepperEnabled(true);
  }

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
	driver.XTARGET(-MAX_RAIL_POSITION - 100000);
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
  driver.XTARGET(MAX_RAIL_POSITION + 100000);
  hitForwardStop = detectEndStop(); // wait until end stop detected
  // if endstop detected...
  if (hitForwardStop) {
    delay(100);
    driver.sg_stop(false);
    driver.sg_stop(true);

    // Serial.print("Actual: "); Serial.println(driver.XACTUAL());

    driver.XACTUAL(MAX_RAIL_POSITION); // set front end stop position as 0
    setForwardEndStop(driver.XACTUAL()); // set forward position
  }

	// if back and forward position set, move to middle position
	if (getBackwardEndStop() == MIN_RAIL_POSITION && getForwardEndStop() == MAX_RAIL_POSITION) {
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
    populateScreen("Auto");                   // go back to Auto screen if not already on it
  }
  auto_screen::resetStack();                  // update button and reset button bitmap
  auto_screen::estimateDuration();            // update estimate
  global::btn_Reset.updateColour(BLACK);      // change reset button back to black
  setShutterTriggered(false);
  setNrMovementsCompleted(0);                 // reset completed movements count
  setExecutedMovement(false);
  produceTone(4, 300, 200);                   // sound 4 tones for 300ms separated by a 200ms delay
  // change max velocity back to normal
  setTargetVelocity(STEALTH_CHOP_VMAX);
}


void video360(long nrSteps) {
  // change to StealthChop if StallGuard is configured
  if (stallGuardConfigured) {
    configStealthChop();
  }
  if (!isStepperEnabled()) {
    setStepperEnabled(true);
  }
  // update target velocity as calling configStealthChop resets this on the driver register but not the function
  setTargetVelocity(getTargetVelocity());
  // set position
  driver.XTARGET(driver.XACTUAL()+nrSteps);
}


void photo360() {
    // 0 - if start of new photo360, set speed, take first photo, and enable stepper
    if (getPhoto360Stage() == orbis::start && isNewPhoto360 && photo360Initiated) {
        Serial.print("STARTED: "); Serial.println(millis());
        setTargetVelocity(360); // approx. 5rpm
        // reset target to be safe
        driver.XTARGET(driver.XACTUAL());
        // change to StealthChop if StallGuard is configured
        if (stallGuardConfigured) {
          configStealthChop();
        }
        if (!isStepperEnabled()) {
          setStepperEnabled(true);
        }
        isNewPhoto360 = false;
        setPhoto360Stage(orbis::pullShutter);
    }

    if (photo360Initiated && !photo360Paused) {
      // if not waiting for shutter and we are at the target, trigger flash
      // STEP 1: trigger photo
      if (getPhoto360Stage() == orbis::pullShutter && driver.XACTUAL() == driver.XTARGET()) {
        // wait for delay, this should be skipped on the first photo
        if (millis() - getLastPhoto360Step() >= getPhoto360Delay()) {
          // take photo
          Serial.print("PULL SHUTTER: "); Serial.println(millis());
          digitalWrite(SONY_PIN, HIGH);
          // setWaitingForShutter(true);
          setFlashTriggerTime(millis());
          // set this at the start of the "loop" (pull, release, move)
          setLastPhoto360Step();
          setPhoto360Stage(orbis::releaseShutter);
        }
      }

      // if shutter has triggered yet, keep retrying until 800ms has passed
      if (getPhoto360Stage() == orbis::releaseShutter) {
        if (millis() - getFlashTriggerTime() >= 800) {
          Serial.print("RELEASE SHUTTER: "); Serial.println(millis());
          digitalWrite(SONY_PIN, LOW);
          setWaitingForShutter(false);
          // increment nr completed 360 photos
          setNrCompleted360Photos(getNrCompleted360Photos()+1);
          if (getCurrentScreen() == "Photo360") {
            photo_screen::printPhoto360Progress();
          }
          setPhoto360Stage(orbis::moveStepper);
        }
      }

      // STEP 2: move to new target if time since last step has passed the delay period
      if (getPhoto360Stage() == orbis::moveStepper) {
        // wait for delay
        if (millis() - getLastPhoto360Step() >= getPhoto360Delay()/2) {
          Serial.print("STEP TO TARGET: "); Serial.println(millis());
          // calculate number of microsteps to move for each photo, will round up to an int
          int nr360Steps = (ORBIS_MOTOR_STEPS*NR_MICROSTEPS) / getNr360Photos();
          // if there is a remainder, keep this value as the last step will be by this distance
          Serial.println((ORBIS_MOTOR_STEPS*NR_MICROSTEPS) % getNr360Photos());
          driver.XTARGET(driver.XACTUAL() + nr360Steps);
          // wait for stepper to reach position
          while (driver.XACTUAL() != driver.XTARGET()) {}
          Serial.print("AT TARGET: "); Serial.println(millis());
          setPhoto360Stage(orbis::pullShutter);
        }
      }

      // photo360 completed
      if (getNrCompleted360Photos() == getNr360Photos()) {
        // reset flags
        photo_screen::resetPhoto360();
        Serial.print("COMPLETED: "); Serial.println(millis());
      }
    }
}