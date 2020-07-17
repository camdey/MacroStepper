#include "GlobalVariables.h"

bool arrowsEnabled          = false;
String currentScreen;
bool editShutterDelay       = false;     	// set shutter delay time
bool editStartPosition      = false;     	// set start point for auto mode
bool editEndPosition        = false;        // set end point for auto mode
bool editMovementDistance   = false;  		// set step distance in any mode
bool editFlashOnValue       = false;        // set flash on value
bool editFlashOffValue      = false;		// set flash off value
bool stepperEnabled         = true;         // current state of stepper motor
long lastMillis             = 0;            // store readings of millis() to use for checking conditions within loops every X milliseconds
long lastStepTime           = 0;
long prevStepperPosition    = 1;            // used for showing position of stepper if changed 
bool railHomed              = false;        // check whether the forward and rear limits of the linear rail have been set
long recursiveValue         = 51200;        // store filtered value of last joystick reading, initialize as 51200 since formula multiplies values by 100 to avoid floats
bool screenRotated          = false;        // check whether screen has been rotated or not
float stepSize              = 0.3125;       // distance travelled per movement in micrometres
bool testingFlash           = false;        // flag for testing flash threshold

 
// Set the state of the GUI arrows to on/off 
void setArrowsEnabled(bool state) {
  arrowsEnabled = state;
}


// Get the state of the GUI arrows
bool isArrowsEnabled() {
  return arrowsEnabled;
}


// Set the current screen displayed to the user
void setCurrentScreen(String screen) {
  currentScreen = screen;
}


// Get the current screen displayed to the user
String getCurrentScreen() {
  return currentScreen;
}


// allow editing of the shutter delay
void setEditShutterDelay(bool state) {
    editShutterDelay = state;
}


// check whether user can edit shutter delay
bool canEditShutterDelay() {
    return editShutterDelay;
}


// allow editing of the AutoStack start position
void setEditStartPosition(bool state) {
    editStartPosition = state;
}


// check whether user can edit AutoStack start position
bool canEditStartPosition() {
    return editStartPosition;
}


// allow editing of the AutoStack end position
void setEditEndPosition(bool state) {
    editEndPosition = state;
}


// check whether user can edit AutoStack end position
bool canEditEndPosition() {
    return editEndPosition;
}


// allow editing of the movement distance (used for step size)
void setEditMovementDistance(bool state) {
    editMovementDistance = state;
}


// check whether user can edit movement distance
bool canEditMovementDistance() {
    return editMovementDistance;
}


// allow editing of the flash "on" value (light reading when flash LED is on)
void setEditFlashOnValue(bool state) {
    editFlashOnValue = state;
}


// check whether user can edit flash on value
bool canEditFlashOnValue() {
    return editFlashOnValue;
}


// allow editing of the flash "off" value (light reading when flash LED is off)
void setEditFlashOffValue(bool state) {
    editFlashOffValue = state;
}


// check whether user can edit flash off value
bool canEditFlashOffValue() {
    return editFlashOffValue;
}


// Set the last millis() reading, useful for doing things within loops every X ms 
void setLastMillis(long millis) {
  lastMillis = millis;
}


// Get the last millis() reading, useful for doing things within loops every X ms
long getLastMillis() {
  return lastMillis;
}


// Set the last millis() reading when a step was taken during AutoStack
void setLastStepTime(long millis) {
  lastStepTime = millis;
}


// Get the last millis() reading when a step was taken during AutoStack
long getLastStepTime() {
  return lastStepTime;
}


// Set previous position of stepper motor
void setPreviousPosition(long position) {
  prevStepperPosition = position;
}


// Get previous position of stepper motor
long getPreviousPosition() {
  return prevStepperPosition;
}


// Set whether the rail has been homed or not
void setRailHomed(bool homed) {
    railHomed = homed;
}


// Check whether the rail has been homed or not
bool isRailHomed() {
    return railHomed;
}


// Set the last recursively filtered joystick reading
void setRecursiveFilterValue(long val) {
  recursiveValue = val;
}


// Get the last recursively filtered joystick reading
long getRecursiveFilterValue() {
  return recursiveValue;
}


// Set screen rotation
void setScreenRotated(bool rotated) {
    screenRotated = rotated;
}


// Check screen rotation
bool isScreenRotated() {
    return screenRotated;
}


// Set current state of stepper
void setStepperEnabled(bool state) {
    stepperEnabled = state;
}


// Check current state of stepper
bool isStepperEnabled() {
    return stepperEnabled;
}


// Set float value of distance travelled per movement in μm, called from calculateStepSize()
void setStepSize(float size) {
  stepSize = size;
}


// Get float value of distance travelled per movement in μm
float getStepSize() {
  return stepSize;
}


// Set when testing flash to prevent navigating away from screen
void setTestingFlash(bool state) {
    testingFlash = state;
}


// Check whether the flash is mid-test to prevent navigation away
bool isTestingFlash() {
    return testingFlash;
}
