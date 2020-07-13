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
void joystick(int xVal, bool dir) {
  // long velocity = calcVelocity(xVal);
  // unsigned long velocityCheck = millis();
  //
  // // wake stepper from sleep
  // if (stepperDisabled == true) {
  //   toggleStepper(true);
  // }
  //
  // while (xVal >= xPosUpper || xVal <= xPosLower) {
  //   long currentPos = driver.XACTUAL();
  //   long targetPos = driver.XTARGET();
  //
  //   if (homedRail == true) {
  //     // don't allow movement if within 1000 steps of forward stop
  //     // values are inverse so high xStickPos = reverse
  //     if (xVal >= xPosUpper && (abs(minPosition - driver.XACTUAL()) <= 1000)) {
  //       break;
  //     }
  //     // don't allow movement if within 1000 steps of backward stop
  //     // values are inverse so low xStickPos = forward
  //     if (xVal <= xPosLower && (abs(maxPosition - driver.XACTUAL()) <= 1000)) {
  //       break;
  //     }
  //   }
  //
  //   if (millis() - velocityCheck >= 100) {
  //     velocity = calcVelocity(xVal);
  //     Serial.print(" xVal: "); Serial.print(xVal);
  //     Serial.print(" | TSTEP: "); Serial.print(driver.TSTEP());
  //     Serial.print(" | currentPos: "); Serial.print(currentPos);
  //     Serial.print(" | targetPos: "); Serial.print(targetPos);
  //     Serial.print(" | velocity: "); Serial.println(velocity);
  //     velocityCheck = millis();
  //   }
  //   if (dir == 1) {
  //     driver.XTARGET(800000);
  //   }
  //   else if (dir == 0) {
  //     driver.XTARGET(-800000);
  //   }
  //
  //   driver.VMAX(velocity);
  //   xVal = readJoystick();
  // }
  // driver.XTARGET(driver.XACTUAL());
  // driver.VMAX(0);
  //
  // // // check start/end position adjustment
  // if (editStartPosition == true && arrowsActive == true) {
  //   if (prevStartPosition != startPosition) {
  //     prevStartPosition = startPosition;
  //   }
  //   config_screen::setAutoStackPositions(true, false); //set start but not end position
  // }
  // if (editEndPosition == true && arrowsActive == true) {
  //   if (prevEndPosition != endPosition) {
  //     prevEndPosition = endPosition;
  //   }
  //   config_screen::setAutoStackPositions(false, true); //set end but not start position
  // }
  // if (getCurrentScreen() == "Manual") {
  //   manual_screen::displayPosition();
  // }
  // else if (getCurrentScreen() == "AutoConfig") {
  //   config_screen::displayPosition();
  // }
  // // update prev position
  // prevStepperPosition = driver.XACTUAL();
}


int readJoystick() {
  int val;

  if (screenRotated == false) {
    val = analogRead(XSTICK_PIN);
  }
  else if (screenRotated == true) {
    val = map(analogRead(XSTICK_PIN), 0, 1023, 1023, 0);
  }

  // offset reading by difference between resting state and ideal middle point
  val -= xPosDiff;
  return val;
}


long calcVelocity(int xVal) {
  long velocity;
  long xAdj;

  if (xVal < 512) {
    xAdj = xVal + 1; // prevent taking log of 0
  }
  else if (xVal >= 512) {
    xAdj = (1024 - xVal);
  }
  xAdj = log(xAdj)*100; // multiply by 100 as value will be truncated to integer
  velocity = map(xAdj, 0, 624, joystickMaxVelocity, 0);

  return velocity;
}
