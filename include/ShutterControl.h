#ifndef SHUTTERCONTROL_H
#define SHUTTERCONTROL_H

#include <Arduino.h>
#include "VariableDeclarations.h"

bool isFlashReady();
void triggerShutter();
void runFlashProcedure(bool restart);
enum fireFlashStep {
    start,
    isFlashAvailable,
    enableShutter,
    isFlashUnavailable,
    disableShutter,
    isSuccessful,
    isUnresponsive
};
fireFlashStep flashStep = start;

#endif
