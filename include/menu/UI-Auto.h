#ifndef UIAUTO_H
#define UIAUTO_H

#include <Arduino.h>
#include "VariableDeclarations.h"
#include "GlobalVariables.h"


namespace auto_screen {
  void initAutoButtons();
  void populateAutoScreen();
  void checkAutoButtons(int touch_x, int touch_y);
  void func_StepDistance(bool btnActive);
  void func_Config(bool btnActive);
  void func_Back(bool btnActive);
  void func_PlayPause(bool btnActive);
  void func_ArrowUp(bool btnActive);
  void func_ArrowDown(bool btnActive);
  void pauseStack();
  void resetStack();
  void estimateDuration();
  void printAutoStackProgress();
  void hideArrows(bool hide);
  void stackStatus(stackProcedureEnum stage);
}

#endif
