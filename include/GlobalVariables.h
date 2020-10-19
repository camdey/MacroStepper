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
void setExecutedMovement(bool executed);
bool hasExecutedMovement();
void setFlashAvailable(bool available);
bool isFlashAvailable();
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
bool hasReachedTargetPosition();
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
void setRailHomed(bool homed);
bool isRailHomed();
void setRecursiveFilterValue(long val);
long getRecursiveFilterValue();
void setScreenRotated(bool rotated);
bool isScreenRotated();
void setShutterDelay(int delay);
void incrementShutterDelay();
void decrementShutterDelay();
int getShutterDelay();
void setStepperEnabled(bool enable);
bool isStepperEnabled();
void setStepsPerMovement(int nrSteps);
void incrementStepsPerMovement();
void decrementStepsPerMovement();
int getStepsPerMovement();
void setStepSize(float size);
float getStepSize();
void setTargetVelocity(long velocity);
long getTargetVelocity();
void setTestingFlash(bool state);
bool isTestingFlash();
void setBulbEnabled(bool enabled);
bool isBulbEnabled();

#endif
