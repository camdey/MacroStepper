#ifndef JOYSTICKCONTROL_H
#define JOYSTICKCONTROL_H

#include <Arduino.h>
#include "VariableDeclarations.h"

void calibrateJoyStick();
void joystickControl();
void readJoystick();
int speedControl(long loopNr);

#endif
