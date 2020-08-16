#ifndef UIMAIN_H
#define UIMAIN_H

#include <Arduino.h>
#include "VariableDeclarations.h"


void initButtons(unsigned long toggleDebounce, unsigned long momentaryDebounce);
void populateScreen(String screen);
void readTouchScreen(String screen);

#endif
