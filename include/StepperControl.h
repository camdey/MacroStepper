#ifndef STEPPERCONTROL_H
#define STEPPERCONTROL_H

#include <Arduino.h>
#include "VariableDeclarations.h"

void autoStack();
void changeDirection();
void dryRun();
void homeRail();
bool detectEndStop();
void overshootPosition(int position, int numberOfSteps, int direction);
void stallDetection();
bool stepMotor(int stepDirection, unsigned long stepperDelay);
void toggleStepper(bool enable);

#endif
