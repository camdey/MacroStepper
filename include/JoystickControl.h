#ifndef JOYSTICKCONTROL_H
#define JOYSTICKCONTROL_H

#include <Arduino.h>
#include "VariableDeclarations.h"

long calcVelocity(int xVal);
void calibrateJoyStick();
void joystickMotion(int xPos);
void printNewPositions(); 
int readJoystick();


#endif
