#ifndef TOUCHCONTROL_H
#define TOUCHCONTROL_H

#include <Arduino.h>
#include "VariableDeclarations.h"

int arrowsTouch(TSPoint &point, bool moveStepper, int val);
void autoScreenTouch(TSPoint &point);
void manualScreenTouch(TSPoint &point);
void startScreenTouch(TSPoint &point);
void touchScreen();

#endif
