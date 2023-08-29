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
void setFlashAvailable(bool available);
bool isFlashAvailableOld();
void setGodoxValue(int value);
int getGodoxValue();
void setGodoxFilterValue(long value);
long getGodoxFilterValue();
void setShutterTriggered(bool triggered);
bool hasShutterTriggered();
// void setStartPosition(long position);
// long getStartPosition();
// void setEndPosition(long position);
// long getEndPosition();
// void setLastMillis(long millis);
// long getLastMillis();
void setFlashTriggerTime(long millis);
long getFlashTriggerTime();
// void setLastStepTime(long millis);
// long getLastStepTime();
// void setNrMovementsCompleted(int nrMovements);
// int getNrMovementsCompleted();
// void incrementNrMovementsCompleted();
// void setNrMovementsRequired(int nrMovements);
// int getNrMovementsRequired();
void setRecursiveFilterValue(long val);
long getRecursiveFilterValue();
void setScreenRotated(bool rotated);
bool isScreenRotated();
void setShutterDelay(int delay);
void incrementShutterDelay();
void decrementShutterDelay();
int getShutterDelay();
String getShutterDelaySeconds();
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

// enum stages {
//         idle,
//         start,
//         isFlashAvailable,
//         pullShutter,
//         isFlashUnavailable,
//         releaseShutter,
//         flashSuccessful,
//         flashUnresponsive,
//         stepTaken,
//         stepDelay,
//         newStep,
//         stackCompleted
// };
// void setCurrentStage(stages stage);
// stages getCurrentStage();

#endif
