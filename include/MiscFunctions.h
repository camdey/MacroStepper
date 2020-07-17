#ifndef MISCFUNCTIONS_H
#define MISCFUNCTIONS_H

#include <Arduino.h>
#include "VariableDeclarations.h"

void estimateDuration(bool screenRefresh);
void resetAutoStack();
void rotateScreen();
void setAutoStackPositions(bool setStart, bool setEnd);
void setMovementsRequired();
void calculateStepSize();
int valueCheck(int value, int min, int max);

#endif
