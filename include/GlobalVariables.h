#ifndef GLOBALVARIABLES_H
#define GLOBALVARIABLES_H

#include <Arduino.h>
#include "VariableDeclarations.h"

void setArrowsEnabled(bool state);
bool isArrowsEnabled();
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
void setLastMillis(long millis);
long getLastMillis();
void setLastStepTime(long millis);
long getLastStepTime();
void setPreviousPosition(long position);
long getPreviousPosition();
void setRailHomed(bool homed);
bool isRailHomed();
void setRecursiveFilterValue(long val);
long getRecursiveFilterValue();
void setScreenRotated(bool rotated);
bool isScreenRotated();
void setStepperEnabled(bool state);
bool isStepperEnabled();
void setStepSize(float size);
float getStepSize();
void setTestingFlash(bool state);
bool isTestingFlash();

#endif