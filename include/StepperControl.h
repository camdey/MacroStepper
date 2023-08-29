#ifndef STEPPERCONTROL_H
#define STEPPERCONTROL_H

#include <Arduino.h>
#include "VariableDeclarations.h"
#include "StepperConfig.h"


void homeRail(TMC5160Stepper_Ext &stepper);
void debugStallGuard(TMC5160Stepper_Ext &stepper);
bool detectEndStop(TMC5160Stepper_Ext &stepper);
void executeMovement(TMC5160Stepper_Ext &stepper, int stepDirection, unsigned long stepperDelay);
void executeSteps(TMC5160Stepper_Ext &stepper, long nrSteps);
void video360(TMC5160Stepper_Ext &stepper, long nrSteps);
void photo360(TMC5160Stepper_Ext &stepper);

#endif
