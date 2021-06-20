#ifndef UIGLOBAL_H
#define UIGLOBAL_H

#include <Arduino.h>
#include "VariableDeclarations.h"

namespace global {
  void initGlobalButtons();
  void func_Flash(bool btnActive);
  void func_Reset(bool btnActive);
  extern gfxButton btn_Flash;
  extern gfxButton btn_Reset;
}
#endif
