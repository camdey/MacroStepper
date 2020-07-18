#ifndef UIAUTOCONFIG_H
#define UIAUTOCONFIG_H

#include <Arduino.h>
#include "VariableDeclarations.h"

namespace config_screen {
  void initAutoConfigButtons();
  void populateAutoConfigScreen();
  void checkAutoConfigButtons(int touch_x, int touch_y);
  void func_Start(bool btnActive);
  void func_End(bool btnActive);
  void func_Config(bool btnActive);
  void func_Run(bool btnActive);
  void func_Delay(bool btnActive);
  void func_Reset(bool btnActive);
  void func_Back(bool btnActive);
  void func_ArrowUp(bool btnActive);
  void func_ArrowDown(bool btnActive);
  void setAutoStackStartPosition();
  void setAutoStackEndPosition();
  void setShutterDelay();
  void displayPosition();
}
#endif
