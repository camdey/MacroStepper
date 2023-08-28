#ifndef MISCFUNCTIONS_H
#define MISCFUNCTIONS_H

#include <Arduino.h>
#include "VariableDeclarations.h"

void rotateScreen();
void produceTone(int reps, int duration, int delay);
// void setMovementsRequired(TMC5160Stepper_Ext &stepper);
void calculateStepSize(TMC5160Stepper_Ext &stepper);
int valueCheck(int value, int min, int max);

#endif
