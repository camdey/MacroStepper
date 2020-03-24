#ifndef TFTMANUAL_H
#define TFTMANUAL_H

#include <Arduino.h>
#include "VariableDeclarations.h"

void initManualButtons();
void func_StepDistance(bool btnActive);
void func_StepNumber();
void func_Position();
void func_Flash(bool btnActive);
void func_Reset(bool btnActive);
void func_Back(bool btnActive);
void func_ArrowUp(bool btnActive);
void func_ArrowDown(bool btnActive);

#endif
