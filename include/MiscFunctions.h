#ifndef MISCFUNCTIONS_H
#define MISCFUNCTIONS_H

#include <Arduino.h>
#include "VariableDeclarations.h"

void estimateDuration(bool screenRefresh);
void resetAutoStack();
void setStepDistance();
void setAutoStackPositions(bool setStart, bool setEnd);

#endif
