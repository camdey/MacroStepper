#ifndef STEPPERFUNCTIONS_H
#define STEPPERFUNCTIONS_H

#include <Arduino.h>
#include "VariableDeclarations.h"

void autoStack();
void calibrateJoyStick();
void changeDirection();
void dryRun();
void homeRail();
void joystickControl();
void readJoystick();
int speedControl(long loopNr);
void stallDetection();
bool stepMotor(int stepDirection, unsigned long stepperDelay);
void toggleStepper(bool enable);

#endif
