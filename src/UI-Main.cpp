#include "GlobalVariables.h"
#include "UI-Main.h"
#include "UI-Home.h"
#include "UI-Flash.h"
#include "UI-Manual.h"
#include "UI-Auto.h"
#include "UI-AutoConfig.h"

using namespace manual_screen;
using namespace home_screen;
using namespace flash_screen;
using namespace auto_screen;
using namespace config_screen;


void initButtons(unsigned long toggleDebounce, unsigned long momentaryDebounce) {
  gfxB.setBackgroundColour(BLACK);
  gfxT.setToggleDebounce(toggleDebounce);
  gfxT.setMomentaryDebounce(momentaryDebounce);

  initHomeButtons();
  initFlashButtons();
  initManualButtons();
  initAutoButtons();
  initAutoConfigButtons();

}


void populateScreen(String screen) {
  tft.fillScreen(BLACK);
  delay(20); // see if this solves the superimposition of two different screens
  tft.fillScreen(BLACK);
  if (screen == "Home") {
    populateHomeScreen();
  }
  else if (screen == "Flash") {
    populateFlashScreen();
  }
  else if (screen == "Manual") {
    populateManualScreen();
  }
  else if (screen == "Auto") {
    populateAutoScreen();
  }
  else if (screen == "AutoConfig") {
    populateAutoConfigScreen();
  }
}


void checkButtons(String screen) {
  TSPoint point = ts.getPoint();

  // reset pinModes for tft (otherwise can't draw!)
  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);

  int touch_x;
  int touch_y;
  int touch_z = point.z;

  if (!isScreenRotated()) {
    touch_x = map(point.y, TS_MINY, TS_MAXY, 0, tft.width());    // rotate & scale to TFT boundaries
    touch_y = map(point.x, TS_MINX, TS_MAXX, 0, tft.height());   //   ... USB port at upper left
  }
  else if (isScreenRotated()) {
    touch_x = map(point.y, TS_MINY, TS_MAXY, tft.width(), 0);    // rotate & scale to TFT boundaries
    touch_y = map(point.x, TS_MINX, TS_MAXX, tft.height(), 0);   //   ... USB port at lower right
  }
  
  if (touch_z >= 50 && touch_z <= 1000) {
    if ((touch_x > 0 && touch_x <= tft.width()) && (touch_y > 0 && touch_y <= tft.height())) {
      if (screen == "Home") {
        checkHomeButtons(touch_x, touch_y, touch_z);
      }
      else if (screen == "Flash") {
        checkFlashButtons(touch_x, touch_y, touch_z);
      }
      else if (screen == "Manual") {
        checkManualButtons(touch_x, touch_y, touch_z);
      }
      else if (screen == "Auto") {
        checkAutoButtons(touch_x, touch_y, touch_z);
      }
      else if (screen == "AutoConfig") {
        checkAutoConfigButtons(touch_x, touch_y, touch_z);
      }
    }
  }
  else if (touch_z == 0 && gfxT.getToggleFlag()) {
    // if toggle active, reset flag to false when
    // no touch is recorded
    gfxT.setToggleFlag(false);
  }
}

