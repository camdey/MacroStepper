#ifndef TOUCHCONTROL_H
#define TOUCHCONTROL_H

#include <Arduino.h>
#include "VariableDeclarations.h"

int arrowsTouch(int xPos, int yPos, bool moveStepper, int val);
void autoConfigScreenTouch(int xPos, int yPos);
void autoScreenTouch(int xPos, int yPos);
void flashScreenTouch(int xPos, int yPos);
void manualScreenTouch(int xPos, int yPos);
void startScreenTouch(int xPos, int yPos);
void touchScreen();

#endif
