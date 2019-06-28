#ifndef USERINTERFACE_H
#define USERINTERFACE_H

#include <Arduino.h>
#include "VariableDeclarations.h"

void autoConfigScreen();
void autoScreen();
void displayPosition();
void drawArrows();
void drawPlayPause(bool greyPlay, bool greyPause);
void manualScreen();
void startScreen();
void updateProgress(bool screenRefresh);

#endif
