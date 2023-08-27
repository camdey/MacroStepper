#ifndef GLOBALVARIABLES_H
#define GLOBALVARIABLES_H

#include <Arduino.h>
#include "VariableDeclarations.h"

void setArrowsEnabled(bool state);
bool areArrowsEnabled();
void setShutterEnabled(bool enabled);
bool isShutterEnabled();
void setCurrentScreen(String screen);
String getCurrentScreen();
void setEditShutterDelay(bool state);
bool canEditShutterDelay();
void setEditStartPosition(bool state);
bool canEditStartPosition();
void setEditEndPosition(bool state);
bool canEditEndPosition();
void setEditMovementDistance(bool state);
bool canEditMovementDistance();
void setEditFlashOnValue(bool state);
bool canEditFlashOnValue();
void setEditFlashOffValue(bool state);
bool canEditFlashOffValue();
// void setExecutedMovement(bool executed);
// bool hasExecutedMovement();
void setFlashAvailable(bool available);
bool isFlashAvailableOld();
void setGodoxValue(int value);
int getGodoxValue();
void setGodoxFilterValue(long value);
long getGodoxFilterValue();
void setForwardEndStop(long position);
long getForwardEndStop();
void setBackwardEndStop(long position);
long getBackwardEndStop();
void setShutterTriggered(bool triggered);
bool hasShutterTriggered();
void setStartPosition(long position);
long getStartPosition();
void setEndPosition(long position);
long getEndPosition();
// bool hasReachedTargetPosition();
void setLastMillis(long millis);
long getLastMillis();
void setFlashTriggerTime(long millis);
long getFlashTriggerTime();
void setLastStepTime(long millis);
long getLastStepTime();
void setNrMovementsCompleted(int nrMovements);
int getNrMovementsCompleted();
void incrementNrMovementsCompleted();
void setNrMovementsRequired(int nrMovements);
int getNrMovementsRequired();
// void setRailHomed(bool homed);
// bool isRailHomed();
void setRecursiveFilterValue(long val);
long getRecursiveFilterValue();
void setScreenRotated(bool rotated);
bool isScreenRotated();
void setShutterDelay(int delay);
void incrementShutterDelay();
void decrementShutterDelay();
int getShutterDelay();
String getShutterDelaySeconds();
// void setStepperEnabled(bool enable);
// bool isStepperEnabled();
// void setStepsPerMovement(int nrSteps);
// void incrementStepsPerMovement();
// void decrementStepsPerMovement();
// int getStepsPerMovement();
// void setStepSize(float size);
// float getStepSize();
// void setTargetVelocity(long velocity);
// long getTargetVelocity();
void setTestingFlash(bool state);
bool isTestingFlash();
void setFlashSensorEnabled(bool enabled);
bool isFlashSensorEnabled();
void setRevsPerMinute(int rpm);
int getRevsPerMinute();
void setVideo360Active(bool active);
bool isVideo360Active();
void setVideo360Target(long target);
long getVideo360Target();
void setJoystickMaxVelocity(long velocity);
long getJoystickMaxVelocity();
void incrementNr360Photos();
void decrementNr360Photos();
int getNr360Photos();
void setPhoto360Delay(long delay);
long getPhoto360Delay();
void setNrCompleted360Photos(int nrPhotos);
int getNrCompleted360Photos();
void setLastPhoto360Step();
long getLastPhoto360Step();
// void setStepperDirCW (bool clockwise);
// bool isStepperDirCW();

enum stages {
    idle,
    start,
    isFlashAvailable,
    pullShutter,
    isFlashUnavailable,
    releaseShutter,
    flashSuccessful,
    flashUnresponsive,
    stepTaken,
    stepDelay,
    newStep,
    stackCompleted
};
void setCurrentStage(stages stage);
stages getCurrentStage();

#endif
