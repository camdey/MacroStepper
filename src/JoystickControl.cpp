#include "MiscFunctions.h"
#include "StepperControl.h"
#include "JoystickControl.h"
#include "UI-Main.h"
#include "UI-Manual.h"
#include "UI-AutoConfig.h"

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
  if (getCurrentScreen() == "Manual") {
    manual_screen::displayPosition();
  }
  else if (getCurrentScreen() == "AutoConfig") {
    config_screen::displayPosition();
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
