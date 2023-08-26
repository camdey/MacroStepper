#include "GlobalVariables.h"
#include "DriverConfig.h"
#include "MiscFunctions.h"
#include "StepperControl.h"
#include "JoystickControl.h"
#include "menu/UI-Main.h"
#include "menu/UI-Manual.h"
#include "menu/UI-AutoConfig.h"


// calculate new velocity (VMAX) value while using joystick motion
// velocity value is a log-transformation of the joystick position
long calcVelocity(int xPos) {
  long velocity;
  int xAdj;

  if (xPos < 512) {
    xAdj = xPos + 1; // prevent taking log of 0
  }
  else if (xPos >= 512) {
    xAdj = (1024 - xPos);
  }
  xAdj = round(log(xAdj)*100); // multiply by 100 as value will be truncated to integer
  // 624 = log(512)*100 rounded
  // Serial.print("joystick vel: "); Serial.println(getJoystickMaxVelocity());
  velocity = map(xAdj, 0, 624, getJoystickMaxVelocity(), 0);

  return velocity;
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
  xStickDiff = 0; // ensure reset to 0 when this function is called

  for (int xth_reading = 0; xth_reading < numReadings; xth_reading++) {
    int xStickPos = readJoystick();
    sumReadings += xStickPos; // add reading to total
    delay(20); // wait 20ms
  }

  xStickResting = (sumReadings / numReadings) + 0.5; // "round" to nearest whole number

  // if xStickresting below/above these values, set at middle point to be safe
  if (xStickResting < 500 || xStickResting > 524) {
    xStickResting = 512;
  }
  xStickUpper = xStickResting + 10;
  xStickLower = xStickResting - 10;
  xStickDiff = 512 - xStickResting;
  // Serial.print("xStick Calib. Diff: "); Serial.println(xStickDiff);
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
void joystickMotion(int xPos) {
  long velocity = calcVelocity(xPos);

  readyStealthChop();
  // use maxIn of 1024 and maxOut of 2 due to how map() is calculated. 
  int dir = map(xPos, 0, 1024, 0, 2); // 511 = 0, 512 = 1
  
  while ((xPos >= xStickUpper || xPos <= xStickLower) && isJoystickBtnActive) {
    if (isRailHomed()) {                              // don't allow movement if within 2mm of endstops
      if (dir == 0 && driver1.XACTUAL() <= SAFE_ZONE_BUFFER) {
        Serial.print("WARNING: hit SAFE_ZONE_BUFFER (within 2mm of rear end stop): "); Serial.println(driver1.XACTUAL());
         produceTone(1, 250, 0);
        break;
      }
      else if (dir == 1 && MAX_RAIL_POSITION - driver1.XACTUAL() <= SAFE_ZONE_BUFFER) {
        Serial.print("WARNING: hit SAFE_ZONE_BUFFER (within 2mm of front end stop): "); Serial.println(driver1.XACTUAL());
        produceTone(1, 250, 0);
        break;
      }
    }
  
    if (millis() - getLastMillis() >= 100) {
      velocity = calcVelocity(xPos);
      // Serial.print(" xPos: "); Serial.print(xPos);
      // Serial.print(" | currentPos: "); Serial.print(driver1.XACTUAL());
      // Serial.print(" | targetPos: "); Serial.print(driver1.XTARGET());
      // Serial.print(" | VACTUAL: "); Serial.print(driver1.VACTUAL());
      // Serial.print(" | velocity: "); Serial.println(velocity);

      isJoystickBtnActive = !digitalRead(ZSTICK_PIN); // check if button still pressed

      printNewPositions();                            // print new positions on the display
      setLastMillis(millis());
    }
    // set target to move stepper
    if (dir == 0) {
      driver1.XTARGET(-800000);
    }
    else if (dir == 1) {
      driver1.XTARGET(800000);
    }
  
    setTargetVelocity(velocity);
    xPos = readJoystick();
  }
  setTargetVelocity(0);                               // bring stepper to a stop
  while (driver1.VACTUAL() != 0) {}                    // wait for stepper to decelerate
  printNewPositions();                                // print final positions now that stepper has stopped
  driver1.XTARGET(driver1.XACTUAL());                   // reset target to actual
  setTargetVelocity(STEALTH_CHOP_VMAX);               // reset VMAX to stealthChop default
}


// Print new rail positions depending on what screen and buttons are active
// Called during joystick movement and after joystick movement when stepper has stopped
// otherwise the position values will be out of sync across different fields
void printNewPositions() {
  if (getCurrentScreen() == "Manual") {             // if on Manual screen, print new positon
    manual_screen::printPosition();
  }
  else if (getCurrentScreen() == "AutoConfig") {    // else if Auto Config screen, print new position
    autoconfig_screen::printPosition();

    if (canEditStartPosition()) {
    autoconfig_screen::updateStartPosition();           // if editing Start position, set start but not end position
    }
    else if (canEditEndPosition()) {
      autoconfig_screen::updateEndPosition();           // if editing End position, set end but not start position
    }
  }
}


// read value from XSTICK_PIN and use recursive filtering to minimize noise
// the previous filtered value from the joystick is used with a weight constant
int readJoystick() {
  long val, adjVal;
  int weight = 40;

  // val = (w × XSTICK_PIN + (100 – w) × prevVal)
  // multiply values by 100 to avoid floating point math, the prevVal part of the formula needs as much precesion as possible
  adjVal = weight * (analogRead(XSTICK_PIN)*100) + (100 - weight) * getRecursiveFilterValue();
  setRecursiveFilterValue(adjVal/100);
  val = round(adjVal*1.00 / 10000);

  if (isScreenRotated()) {
    val = map(val, 0, 1023, 1023, 0);
  }
  // offset reading by difference between resting state and ideal middle point
  // val -= xStickDiff;
  return val;
}
