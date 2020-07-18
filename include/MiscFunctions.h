#ifndef MISCFUNCTIONS_H
#define MISCFUNCTIONS_H

#include <Arduino.h>
#include "VariableDeclarations.h"

void rotateScreen();
void setMovementsRequired();
void calculateStepSize();
int valueCheck(int value, int min, int max);

#endif
