#ifndef STEPPERCONTROL_H
#define STEPPERCONTROL_H

#include <Arduino.h>
#include "VariableDeclarations.h"
#include "StepperConfig.h"

void autoStack(TMC5160Stepper_Ext &stepper);
void dryRun(TMC5160Stepper_Ext &stepper);
void homeRail(TMC5160Stepper_Ext &stepper);
void debugStallGuard(TMC5160Stepper_Ext &stepper);
bool detectEndStop(TMC5160Stepper_Ext &stepper);
void executeMovement(TMC5160Stepper_Ext &stepper, int stepDirection, unsigned long stepperDelay);
void executeSteps(TMC5160Stepper_Ext &stepper, long nrSteps);
void goToStart(TMC5160Stepper_Ext &stepper);
void overshootPosition(TMC5160Stepper_Ext &stepper, int startPosition, int numberOfSteps);
void terminateAutoStack(TMC5160Stepper_Ext &stepper);
void video360(TMC5160Stepper_Ext &stepper, long nrSteps);
void photo360(TMC5160Stepper_Ext &stepper);
void readyStallGuard(TMC5160Stepper_Ext &stepper);
void readyStealthChop(TMC5160Stepper_Ext &stepper);

#endif
