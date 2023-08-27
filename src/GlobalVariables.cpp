#include "GlobalVariables.h"
#include "StepperConfig.h"
#include "MiscFunctions.h"

bool arrowsEnabled              = false;
bool shutterEnabled             = false;        // if true, will try to take a photo during autoStack or manual movement
String currentScreen            = "Home";       // set current screen shown to user
bool editShutterDelay           = false;        // set shutter delay time
bool editStartPosition          = false;        // set start point for auto mode
bool editEndPosition            = false;        // set end point for auto mode
bool editMovementDistance       = false;        // set step distance in any mode
bool editFlashOnValue           = false;        // set flash on value
bool editFlashOffValue          = false;        // set flash off value
bool flashAvailable             = false;        // check whether flash is available to trigger or not
int godoxValue                  = 0;            // light sensor reading from the godox flash LED
long godoxFilterValue           = 10000;        // recursive filter value of light sensor reading from the godox flash LED
long ForwardEndStopPosition     = 1;            // position of the forward end stop (opposite end from stepper)
long BackwardEndStopPosition    = 1;            // position of the backward end stop (same end as stepper)
long startPosition              = 0;            // starting position of an AutoStack sequence
long endPosition                = 0;            // end position of an AutoStack sequence
long lastMillis                 = 0;            // store readings of millis() to use for checking conditions within loops every X milliseconds
long flashTriggerTime           = 0;            // last millis time flash was triggered successfully
long lastStepTime               = 0;            // last millis time a step was executed
// bool executedMovement           = false;        // whether stepper was moved when moveStepper() called
int nrCompletedMovements        = 0;            // number of completed movements (multiples of steps) within an AutoStack procedure
int nrMovementsRequired         = 0;            // number of movements (multiples of steps) required for an AutoStack procedure
// bool railHomed                  = false;        // check whether the forward and rear limits of the linear rail have been set
long recursiveValue             = 51200;        // store filtered value of last joystick reading, initialize as 51200 since formula multiplies values by 100 to avoid floats
bool screenRotated              = false;        // check whether screen has been rotated or not
int shutterDelay                = 500;          // delay in milliseconds between a movement and taking a photo via the shutter pin
bool shutterTriggered           = true;         // shutter successfully triggered or not
// bool stepperEnabled             = true;         // current state of stepper motor
// float stepSize                  = 5.0000;       // distance travelled per movement in micrometres, default to 5um
// int stepsPerMovement            = 16;           // number of microsteps to travel a specified distance, default to 16 (1 full step or 5um)
// long targetVelocity             = 200000;       // target velocity = VMAX for TMC5160
bool testingFlash               = false;        // flag for testing flash threshold
bool flashSensorEnabled         = false;        // is flash bulb enabled, or only take photos without flash?
int revsPerMinute               = 50;           // revs per minute for video 360, stored as 10x higher to avoid floating point math
bool video360Active             = false;        // is video 360 running?
long video360Target             = 10000;        // set target number of steps for video 360
long joystickMaxVelocity        = 100000;       // VMAX when using joystick
int nr360PhotosArrayIndex       = 11;           // index for number of photoCountArray
int nr360Photos                 = 72;           // nr photos taken in a 360 photo
long photo360Delay              = 1000;         // delay in milliseconds between each photo in a photo360 procedure
int nrCompleted360Photos        = 0;            // nr of photos taken in a 360 procedure
long lastPhoto360Step           = 0;            // millis of last photo taken for photo360
// bool stepperDirCW               = true;         // direction of the stepper motor, is it clockwise?

stages currentStage = idle;
#define photoCountArraySize 20 // need to ensure we don't go out of bounds
int photoCountArray[] = {6,12,18,24,30,36,42,48,54,60,66,72,78,84,90,96,102,108,114,120}; // possible values for nr360Photos


void setCurrentStage(stages stage) {
  currentStage = stage;
}


stages getCurrentStage() {
  return currentStage;
}

 
// Set the state of the GUI arrows to on/off 
void setArrowsEnabled(bool state) {
  arrowsEnabled = state;
}


// Get the state of the GUI arrows
bool areArrowsEnabled() {
  return arrowsEnabled;
}


// Set camera state, if true program will try to trigger the camera shutter after a step
void setShutterEnabled(bool enabled) {
    shutterEnabled = enabled;
}


// Check if camera is enabled, if true program will try to trigger the camera shutter after a step
bool isShutterEnabled() {
    return shutterEnabled;
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


// // Set whether the stepper moved when moveStepper() called
// void setExecutedMovement(bool executed) {
//     executedMovement = executed;
// }


// // Check whether the stepper moved when moveStepper() was last called
// bool hasExecutedMovement() {
//     return executedMovement;
// }


// Set flash availability
void setFlashAvailable(bool available) {
    flashAvailable = available;
}


// Check if flash is available to trigger or not
bool isFlashAvailableOld() {
    return flashAvailable;
}


// Set the light sensor reading from the godox pin
void setGodoxValue(int value) {
    godoxValue = value;
}


// Get the last light sensor reading from the godox pin
int getGodoxValue() {
    return godoxValue;
}


// Set the recursive filter value of the light sensor reading from the godox pin
void setGodoxFilterValue(long value) {
    godoxFilterValue = value;
}


// Get the recursive filter value of the last light sensor reading from the godox pin
long getGodoxFilterValue() {
    return godoxFilterValue;
}


// set the stepper position for the forward/front end stop
void setForwardEndStop(long position) {
    ForwardEndStopPosition = position;
}


// get the stepper position for the forward/front end stop
long getForwardEndStop() {
    return ForwardEndStopPosition;
}


// set the stepper position for the backward/rear end stop
void setBackwardEndStop(long position) {
    BackwardEndStopPosition = position;
}


// get the stepper position for the backward/rear end stop
long getBackwardEndStop() {
    return BackwardEndStopPosition;
}


// set whether shutter was successfully triggered or not
void setShutterTriggered(bool triggered) {
  shutterTriggered = triggered;
}


// check whether the shutter was triggered when called
bool hasShutterTriggered() {
  return shutterTriggered;
}


// set the start position for AutoStack, also updates movementsRequired
void setStartPosition(long position) {
    startPosition = position;
    // temporary, change to generic instance later
    setMovementsRequired(stepper1); // update movementsRequired as calculation has changed
}


// get the start position for AutoStack
long getStartPosition() {
    return startPosition;
}


// set the end position for AutoStack, also updates movementsRequired
void setEndPosition(long position) {
    endPosition = position;
    // temporary, change to generic instance later
    setMovementsRequired(stepper1); // update movementsRequired as calculation has changed
}


// get the end position for AutoStack
long getEndPosition() {
    return endPosition;
}


// check if the stepper's current position is the same as the target position
// bool hasReachedTargetPosition() {
//     bool hasReachedTarget = (stepper1.XACTUAL() == stepper1.XTARGET());
//     return hasReachedTarget;
// }


// Set the last millis() reading, useful for doing things within loops every X ms 
void setLastMillis(long millis) {
  lastMillis = millis;
}


// Get the last millis() reading, useful for doing things within loops every X ms
long getLastMillis() {
  return lastMillis;
}


// Set the last millis() reading when flash triggering began
void setFlashTriggerTime(long millis) {
  flashTriggerTime = millis;
}


// Get the last millis() reading when flash triggering began
long getFlashTriggerTime() {
  return flashTriggerTime;
}


// Set the last millis() reading when a step was taken during AutoStack
void setLastStepTime(long millis) {
  lastStepTime = millis;
}


// Get the last millis() reading when a step was taken during AutoStack
long getLastStepTime() {
  return lastStepTime;
}


// set how many movements have been completed in an AutoStack procedure
void setNrMovementsCompleted(int nrMovements) {
  nrCompletedMovements = nrMovements;
}


// increment how many movements have been completed in an AutoStack procedure by 1
void incrementNrMovementsCompleted() {
  nrCompletedMovements++;
}


// get how many movements have been completed in an AutoStack procedure
int getNrMovementsCompleted() {
  return nrCompletedMovements;
}


// set how many movements are required for a specified AutoStack procedure
void setNrMovementsRequired(int nrMovements) {
  nrMovementsRequired = nrMovements;
}


// get how many movements are required for a specified AutoStack procedure
int getNrMovementsRequired() {
  return nrMovementsRequired;
}


// // Set whether the rail has been homed or not
// void setRailHomed(bool homed) {
//     railHomed = homed;
// }


// // Check whether the rail has been homed or not
// bool isRailHomed() {
//     return railHomed;
// }


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


// Set delay between a movement and taking a photo via shutter pin
void setShutterDelay(int delay) {
  shutterDelay = valueCheck(delay, 500, 5900);
}


// Increment delay between a movement and taking a photo via shutter pin
void incrementShutterDelay() {
  shutterDelay+=100;
  shutterDelay = valueCheck(shutterDelay, 500, 5900);
}


// Decrement delay between a movement and taking a photo via shutter pin
void decrementShutterDelay() {
  shutterDelay-=100;
  shutterDelay = valueCheck(shutterDelay, 500, 5900);
}


// Get delay between a movement and taking a photo via shutter pin
int getShutterDelay() {
  return shutterDelay;
}

// Get delay in seconds in a displayable String format between a movement and taking a photo via shutter pin
String getShutterDelaySeconds() {
  int delay = getShutterDelay();
  int thousandths = delay*0.001;
  int hundreths = (delay-(thousandths*1000))*0.01;
  return String(thousandths) + '.' + String(hundreths);
}


// Set current state of stepper
// void setStepperEnabled(bool enable) {
// 	if (enable) {
// 		delay(10);
// 		digitalWrite(EN_1_PIN, LOW); // enable
// 		delay(10);
// 	}

// 	if (!enable) {
// 		delay(10);
// 		digitalWrite(EN_1_PIN, HIGH); // disable
// 		delay(10);
// 	}
//   stepperEnabled = enable;
// }


// // Check current state of stepper
// bool isStepperEnabled() {
//     return stepperEnabled;
// }


// // Set number of microsteps taken per movement
// void setStepsPerMovement(int nrSteps) {
//   stepsPerMovement = nrSteps;
// }


// // Increment number of microsteps taken per movement by 1
// void incrementStepsPerMovement() {
//   stepsPerMovement++;
// }


// // Decrement number of microsteps taken per movement by 1
// void decrementStepsPerMovement() {
//   stepsPerMovement--;
// }


// // Get number of microsteps taken per movement
// int getStepsPerMovement() {
//   return stepsPerMovement;
// }


// // Set float value of distance travelled per movement in μm, called from calculateStepSize(stepper1)
// void setStepSize(float size) {
//   stepSize = size;
// }


// // Get float value of distance travelled per movement in μm
// float getStepSize() {
//   return stepSize;
// }


// Set when testing flash to prevent navigating away from screen
void setTestingFlash(bool state) {
    testingFlash = state;
}


// Check whether the flash is mid-test to prevent navigation away
bool isTestingFlash() {
    return testingFlash;
}


// // set target velocity for stepper (VMAX)
// void setTargetVelocity(long velocity) {
//   stepper1.VMAX(velocity);
//   targetVelocity = velocity;
// }


// // get target velocity for stepper (VMAX)
// long getTargetVelocity() {
//   return targetVelocity;
// }


void setFlashSensorEnabled(bool enabled) {
  flashSensorEnabled = enabled;
}

bool isFlashSensorEnabled() {
  return flashSensorEnabled;
}


void setRevsPerMinute(int rpm) {
  revsPerMinute = rpm;
}


int getRevsPerMinute() {
  return revsPerMinute;
}


void setVideo360Active(bool active) {
  video360Active = active;
}


bool isVideo360Active() {
  return video360Active;
}


void setVideo360Target(long target) {
  video360Target = target;
}


long getVideo360Target() {
  return video360Target;
}


void setJoystickMaxVelocity(long velocity) {
  joystickMaxVelocity = velocity;
}


long getJoystickMaxVelocity() {
  return joystickMaxVelocity;
}


void incrementNr360Photos() {
  // minus one as array index starts at 0
  if (nr360PhotosArrayIndex < photoCountArraySize-1) {
    nr360PhotosArrayIndex++;
  }
}


void decrementNr360Photos() {
  if (nr360PhotosArrayIndex > 0) {
    nr360PhotosArrayIndex--;
  }
}


int getNr360Photos() {
  nr360Photos = photoCountArray[nr360PhotosArrayIndex];
  return nr360Photos;
}


void setPhoto360Delay(long delay) {
  photo360Delay = delay;
  // 500ms is min time needed for pulling shutter
  if (photo360Delay < 500) {
    photo360Delay = 500;
  }
}


long getPhoto360Delay() {
  return photo360Delay;
}


void setNrCompleted360Photos(int nrPhotos) {
  nrCompleted360Photos = nrPhotos;
}


int getNrCompleted360Photos() {
  return nrCompleted360Photos;
}


void setLastPhoto360Step() {
  lastPhoto360Step = millis();
}


long getLastPhoto360Step() {
  return lastPhoto360Step;
}


// void setStepperDirCW (bool clockwise) {
//   stepperDirCW = clockwise;
// }


// bool isStepperDirCW() {
//   return stepperDirCW;
// }