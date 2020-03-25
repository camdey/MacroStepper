#include "TFT-Main.h"
#include "TFT-Home.h"
#include "TFT-Manual.h"
#include "gfxButton.h"


void initButtons(unsigned long toggleDebounce, unsigned long momentaryDebounce) {
  gfxT.setToggleDebounce(toggleDebounce);
  gfxT.setMomentaryDebounce(momentaryDebounce);

  initHomeButtons();
  initManualButtons();

}


void populateScreen(String screen) {
  if (screen == "Home") {
    populateHomeScreen();
  }
  else if (screen == "Manual") {
    populateManualScreen();
  }
}


void checkButtons(String screen) {
  TSPoint point = ts.getPoint();

  // reset pinModes for tft (otherwise can't draw!)
  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);

  int touch_x = map(point.y, 937, 140, 0, 480);
  int touch_y = map(point.x, 846, 148, 0, 320);
  int touch_z = point.z;

  if (screen == "Home") {
    checkHomeButtons(touch_x, touch_y, touch_z);
  }
  else if (screen == "Manual") {
    checkManualButtons(touch_x, touch_y, touch_z);
  }
}
