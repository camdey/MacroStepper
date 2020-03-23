#ifndef TFTMANUAL_H
#define TFTMANUAL_H

#include <Arduino.h>
#include "VariableDeclarations.h"

void manual_StepDistance(bool btnActive);
void manual_StepNumber();
void manual_Position();
void manual_Flash(bool btnActive);
void manual_Reset(bool btnActive);
void manual_Back(bool btnActive);

#endif
