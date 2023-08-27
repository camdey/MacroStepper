#ifndef STEPPERCONTROL_H
#define STEPPERCONTROL_H

#include <Arduino.h>
#include "VariableDeclarations.h"

void autoStack(TMC5160Stepper_Ext &driver);
void dryRun(TMC5160Stepper_Ext &driver);
void homeRail(TMC5160Stepper_Ext &driver);
void debugStallGuard(TMC5160Stepper_Ext &driver);
bool detectEndStop(TMC5160Stepper_Ext &driver);
void executeMovement(TMC5160Stepper_Ext &driver, int stepDirection, unsigned long stepperDelay);
void executeSteps(TMC5160Stepper_Ext &driver, long nrSteps);
void goToStart(TMC5160Stepper_Ext &driver);
void overshootPosition(int startPosition, int numberOfSteps);
void terminateAutoStack(TMC5160Stepper_Ext &driver);
void video360(TMC5160Stepper_Ext &driver, long nrSteps);
void photo360(TMC5160Stepper_Ext &driver);
void readyStallGuard(TMC5160Stepper_Ext &driver);
void readyStealthChop(TMC5160Stepper_Ext &driver);

#endif
