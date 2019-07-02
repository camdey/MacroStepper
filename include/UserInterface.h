#ifndef USERINTERFACE_H
#define USERINTERFACE_H

#include <Arduino.h>
#include "VariableDeclarations.h"

void autoConfigScreen();
void autoScreen();
void displayPosition();
void drawArrows();
void drawPlayPause(bool greyPlay, bool greyPause);
void flashScreen();
void manualScreen();
void startScreen();
void updateFlashValue();
void updateProgress(bool screenRefresh);
void updateValueField(String valueField, int textColour);

#endif
