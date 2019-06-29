#ifndef SHUTTERCONTROL_H
#define SHUTTERCONTROL_H

#include <Arduino.h>
#include "VariableDeclarations.h"

bool flashStatus();
void setShutterDelay();
void toggleShutter();
bool triggerShutter();

#endif
