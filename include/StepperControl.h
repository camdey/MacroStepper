#ifndef STEPPERCONTROL_H
#define STEPPERCONTROL_H

#include <Arduino.h>
#include "VariableDeclarations.h"

void autoStack();
void dryRun();
void homeRail();
void debugStallGuard();
bool detectEndStop();
void overshootPosition(int position, int numberOfSteps, int direction);
void executeMovement(int stepDirection, unsigned long stepperDelay);

#endif
