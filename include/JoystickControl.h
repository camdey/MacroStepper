#ifndef JOYSTICKCONTROL_H
#define JOYSTICKCONTROL_H

#include <Arduino.h>
#include "VariableDeclarations.h"

void calibrateJoyStick();
void joystickMotion(int xPos);
int readJoystick();
long calcVelocity(int xVal);
void setRecursiveFilterValue(long val);
long getRecursiveFilterValue();

#endif
