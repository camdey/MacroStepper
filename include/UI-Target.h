#ifndef UITARGET_H
#define UITARGET_H

#include "Arduino.h"

namespace target_screen {
  void initTargetButtons();
  void populateTargetScreen();
  void checkTargetButtons(int touch_x, int touch_y);
  void func_Distance(bool active);
  void func_Input(char* label);
  void incrementIndex();
  void decrementIndex();
  void func_Back(bool active);
  void func_Run(bool active);
}
#endif
