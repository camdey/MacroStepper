#ifndef STEPPERCONTROL_H
#define STEPPERCONTROL_H

#include <Arduino.h>
#include "VariableDeclarations.h"

void autoStack();
void dryRun();
void homeRail();
void debugStallGuard();
bool detectEndStop();
void executeMovement(int stepDirection, unsigned long stepperDelay);
void goToStart();
void overshootPosition(int startPosition, int numberOfSteps);

#endif
