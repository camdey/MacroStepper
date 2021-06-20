#ifndef UIVIDEO_H
#define UIVIDEO_H

#include <Arduino.h>
#include "VariableDeclarations.h"
#include "GlobalVariables.h"


namespace video_screen {
  void initVideoButtons();
  void populateVideoScreen();
  void checkVideoButtons(int touch_x, int touch_y);
  void func_Speed(bool btnActive);
  void func_Direction(bool btnActive);
  void func_Back(bool btnActive);
  void func_PlayPause(bool btnActive);
  void func_ArrowUp(bool btnActive);
  void func_ArrowDown(bool btnActive);
}

#endif
