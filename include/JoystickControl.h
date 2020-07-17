#ifndef JOYSTICKCONTROL_H
#define JOYSTICKCONTROL_H

#include <Arduino.h>
#include "VariableDeclarations.h"

void calibrateJoyStick();
void joystickMotion(int xPos);
int readJoystick();
long calcVelocity(int xVal);

#endif
