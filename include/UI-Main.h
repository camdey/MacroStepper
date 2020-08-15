#ifndef UIMAIN_H
#define UIMAIN_H

#include <Arduino.h>
#include "VariableDeclarations.h"


void initButtons(unsigned long toggleDebounce, unsigned long momentaryDebounce);
void populateScreen(String screen);
void checkButtons(String screen);

#endif
