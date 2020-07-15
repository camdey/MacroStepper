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
  Serial.print("xStick Calib. Diff: "); Serial.println(xStickDiff);
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
  unsigned long velocityCheck = millis();

  // use maxIn of 1024 and maxOut of 2 due to how map() is calculated. 
  int dir = map(xPos, 0, 1024, 0, 2); // 511 = 0, 512 = 1
  
  // wake stepper from sleep
  if (stepperDisabled == true) {
    toggleStepper(true);
  }
  
  while (xPos >= xStickUpper || xPos <= xStickLower) {
    long currentPos = driver.XACTUAL();
    long targetPos = driver.XTARGET();
  
    if (homedRail == true) {
      // don't allow movement if within 2mm of endstops
      // values are inverse so high xStickPos = reverse
      if (dir == 0 && driver.XACTUAL() <= safeZone) {
        Serial.print("WARNING: hit safeZone (within 2mm of rear end stop): "); Serial.println(driver.XACTUAL());
        break;
      }
      else if (dir == 1 && maxRailPosition - driver.XACTUAL() <= safeZone) {
        Serial.print("WARNING: hit safeZone (within 2mm of front end stop): "); Serial.println(driver.XACTUAL());
        break;
      }
    }
  
    if (millis() - getLastMillis() >= 100) {
      velocity = calcVelocity(xPos);
      Serial.print(" xPos: "); Serial.print(xPos);
      Serial.print(" | TSTEP: "); Serial.print(driver.TSTEP());
      Serial.print(" | currentPos: "); Serial.print(currentPos);
      Serial.print(" | targetPos: "); Serial.print(targetPos);
      Serial.print(" | velocity: "); Serial.println(velocity);
      setLastMillis(millis());
    }
    if (dir == 0) {
      driver.XTARGET(-800000);
    }
    else if (dir == 1) {
      driver.XTARGET(800000);
    }
  
    driver.VMAX(velocity);
    xPos = readJoystick();
  }
  driver.XTARGET(driver.XACTUAL());
  driver.VMAX(0);
  
  // // check start/end position adjustment
  if (editStartPosition == true && arrowsActive == true) {
    if (prevStartPosition != startPosition) {
      prevStartPosition = startPosition;
    }
    config_screen::setAutoStackPositions(true, false); //set start but not end position
  }
  if (editEndPosition == true && arrowsActive == true) {
    if (prevEndPosition != endPosition) {
      prevEndPosition = endPosition;
    }
    config_screen::setAutoStackPositions(false, true); //set end but not start position
  }
  if (getCurrentScreen() == "Manual") {
    manual_screen::displayPosition();
  }
  else if (getCurrentScreen() == "AutoConfig") {
    config_screen::displayPosition();
  }
  // update prev position
  prevStepperPosition = driver.XACTUAL();
}

// IS THIS CAUSING ISSUES WITH forward vs reverse spead - observed one direction had minimum of -7 and went to fullpower, the other direction had max of 1016 and had 2/3 power.
// Corresponding log val is 2.08, which is 2/3 of the max 6.24
int readJoystick() {
  int val;
  val = analogRead(XSTICK_PIN);
  if (screenRotated == true) {
    val = map(val, 0, 1023, 1023, 0);
  }
  // offset reading by difference between resting state and ideal middle point
  val -= xStickDiff;
  return val;
}


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
  velocity = map(xAdj, 0, 624, joystickMaxVelocity, 0);

  return velocity;
}
