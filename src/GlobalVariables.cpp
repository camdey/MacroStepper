#include "GlobalVariables.h"
#include "StepperControl.h"
#include "MiscFunctions.h"

// ui
bool arrowsEnabled                  = false;
// bool shutterEnabled                 = false;            // if true, will try to take a photo during autoStack or manual movement
String currentScreen                = "Home";           // set current screen shown to user
bool editShutterDelay               = false;            // set shutter delay time
bool editStartPosition              = false;            // set start point for auto mode
bool editEndPosition                = false;            // set end point for auto mode
bool editMovementDistance           = false;            // set step distance in any mode
bool editFlashOnValue               = false;            // set flash on value
bool editFlashOffValue              = false;            // set flash off value
bool screenRotated                  = false;            // check whether screen has been rotated or not
// joystick
long joystickMaxVelocity            = 100000;           // VMAX when using joystick
long recursiveValue                 = 51200;            // store filtered value of last joystick reading, initialize as 51200 since formula multiplies values by 100 to avoid floats
// video360
int revsPerMinute                   = 50;               // revs per minute for video 360, stored as 10x higher to avoid floating point math
bool video360Active                 = false;            // is video 360 running?
long video360Target                 = 10000;            // set target number of steps for video 360

 
// Set the state of the GUI arrows to on/off 
void setArrowsEnabled(bool state) {
    arrowsEnabled = state;
}


// Get the state of the GUI arrows
bool areArrowsEnabled() {
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
