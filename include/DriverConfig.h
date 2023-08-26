#ifndef DRIVERCONFIG_H
#define DRIVERCONFIG_H

#include <Arduino.h>
#include "VariableDeclarations.h"

void configStealthChop(TMC5160Stepper &driver);
void configStallGuard(TMC5160Stepper &driver);

#endif
