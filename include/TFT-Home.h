#ifndef TFTHOME_H
#define TFTHOME_H

#include <Arduino.h>
#include "VariableDeclarations.h"

void initHomeButtons();
void func_Auto(bool btnActive);
void func_Manual(bool btnActive);
void func_Home(bool btnActive);
void func_Flash(bool btnActive);
void func_Rotate(bool btnActive);

#endif
