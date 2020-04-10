#ifndef UIAUTO_H
#define UIAUTO_H

#include <Arduino.h>
#include "VariableDeclarations.h"

namespace auto_screen {
  void initAutoButtons();
  void populateAutoScreen();
  void checkAutoButtons(int touch_x, int touch_y, int touch_z);
  void func_StepDistance(bool btnActive);
  void func_Flash(bool btnActive);
  void func_Config(bool btnActive);
  void func_Back(bool btnActive);
  void func_Play(bool btnActive);
  void func_Pause(bool btnActive);
  void func_ArrowUp(bool btnActive);
  void func_ArrowDown(bool btnActive);
  void drawPlayPause(bool greyPlay, bool greyPause);
  void estimateDuration(bool screenRefresh);
  void updateProgress(bool screenRefresh);
}
#endif
