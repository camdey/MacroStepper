#ifndef TFTMAIN_H
#define TFTMAIN_H

#include <Arduino.h>
#include "VariableDeclarations.h"


void initButtons(unsigned long toggleDebounce, unsigned long momentaryDebounce);
void populateScreen(String screen);
void checkButtons(String screen);

#endif
