#ifndef GLOBALVARIABLES_H
#define GLOBALVARIABLES_H

#include <Arduino.h>
#include "VariableDeclarations.h"

void setArrowsEnabled(bool state);
bool areArrowsEnabled();
// void setShutterEnabled(bool enabled);
// bool isShutterEnabled();
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
void setRecursiveFilterValue(long val);
long getRecursiveFilterValue();
void setScreenRotated(bool rotated);
bool isScreenRotated();
void setRevsPerMinute(int rpm);
int getRevsPerMinute();
void setVideo360Active(bool active);
bool isVideo360Active();
void setVideo360Target(long target);
long getVideo360Target();
void setJoystickMaxVelocity(long velocity);
long getJoystickMaxVelocity();


#endif
