#ifndef JOYSTICKCONTROL_H
#define JOYSTICKCONTROL_H

#include <Arduino.h>
#include "VariableDeclarations.h"

void calibrateJoyStick();
void joystickControl();
int readJoystick();
long calcVelocity(int xVal);

#endif
