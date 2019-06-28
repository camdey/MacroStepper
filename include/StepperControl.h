#ifndef STEPPERFUNCTIONS_H
#define STEPPERFUNCTIONS_H

#include <Arduino.h>
#include "VariableDeclarations.h"

void autoStack();
void changeDirection();
void dryRun();
void homeRail();
void joyStick();
void stallDetection();
bool stepperStep(int stepDirection, unsigned long stepperDelay);
void toggleStepper(bool enable);

#endif
