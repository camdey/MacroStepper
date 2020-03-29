#include "TFT-Main.h"
#include "TFT-Home.h"
#include "TFT-Flash.h"
#include "TFT-Manual.h"
#include "gfxButton.h"
using namespace manual_screen;
using namespace home_screen;
using namespace flash_screen;

// TODO - replaceText function for overwriting
// TODO - writeText align centre top for manual page buttons

void initButtons(unsigned long toggleDebounce, unsigned long momentaryDebounce) {
  gfxT.setToggleDebounce(toggleDebounce);
  gfxT.setMomentaryDebounce(momentaryDebounce);

  initHomeButtons();
  initFlashButtons();
  initManualButtons();

}


void populateScreen(String screen) {
  if (screen == "Home") {
    populateHomeScreen();
  }
  else if (screen == "Flash") {
    populateFlashScreen();
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
  else if (screen == "Flash") {
    checkFlashButtons(touch_x, touch_y, touch_z);
  }
  else if (screen == "Manual") {
    checkManualButtons(touch_x, touch_y, touch_z);
  }
}


/***********************************************************************
Generic function for updating various values in different fields.
Easier to update them all via one function so the code is contained in
one place. Some fields are referenced from several parts of the code.
***********************************************************************/
// void updateValueField(String valueField, int textColour) {
//   // rail position field in Manual screen
//   if (valueField == "Rail Position") {
//     tft.setTextColor(textColour, CUSTOM_BLUE);
//     tft.setFont(&Arimo_Bold_24);
//     tft.setCursor(10, 220);
//     tft.println(String(stepper.currentPosition()*(microstepDistance/1000), 5));
//   }
//   // step nr field in Manual screen
//   else if (valueField == "Step Nr") {
//     tft.setTextColor(textColour, CUSTOM_BLUE);
//     tft.setCursor(45, 140);
//     tft.setFont(&Arimo_Bold_30);
//     tft.println(manualMovementCount);
//   }
//   // step dist field in Manual and Auto screens
//   else if (valueField == "Step Dist") {
//     tft.setTextColor(textColour, CUSTOM_BLUE);
//     tft.setCursor(15, 60);
//     tft.setFont(&Arimo_Bold_24);
//     tft.println(String(distancePerMovement/1000, 5));
//   }
//   // start position in AutoConfig screen
//   else if (valueField == "Start Position") {
//     tft.setTextColor(textColour, BLACK);
//     tft.setCursor(35, 65);
//     tft.setFont(&Arimo_Regular_16);
//     tft.println(startPosition);
//   }
//   // end position in AutoConfig screen
//   else if (valueField == "End Position") {
//     tft.setTextColor(textColour, BLACK);
//     tft.setCursor(35, 145);
//     tft.setFont(&Arimo_Regular_16);
//     tft.println(endPosition);
//   }
//   // current position in AutoConfig screen
//   else if (valueField == "Current Position") {
//     tft.setTextColor(textColour, BLACK);
//     tft.setCursor(35, 225);
//     tft.setFont(&Arimo_Regular_16);
//     tft.println(stepper.currentPosition());
//   }
//   // flashValue from light sensor for calibrating OFF value
//   else if (valueField == "Flash Off") {
//     int16_t x, y;
//     uint16_t w, h;
//     tft.setFont(&Arimo_Regular_30);
//     tft.setTextColor(textColour, BLACK);
//     tft.getTextBounds(String(flashOffValue), 30, 75, &x, &y, &w, &h);
//     tft.fillRect(x, y, w, h, BLACK);
//     tft.setCursor(30, 75);
//     tft.println(flashValue);
//   }
//   // flashValue from light sensor for calibrating ON value
//   else if (valueField == "Flash On") {
//     int16_t x, y;
//     uint16_t w, h;
//     tft.setFont(&Arimo_Regular_30);
//     tft.setTextColor(textColour, BLACK);
//     tft.getTextBounds(String(flashOnValue), 30, 155, &x, &y, &w, &h);
//     tft.fillRect(x, y, w, h, BLACK);
//     tft.setCursor(30, 155);
//     tft.println(flashValue);
//   }
//   // update threshold value that determines if flash is ready to trigger or not
//   else if (valueField == "Threshold") {
//     int16_t x, y;
//     uint16_t w, h;
//     tft.setFont(&Arimo_Regular_30);
//     tft.setTextColor(textColour, BLACK);
//     tft.getTextBounds(String(prevFlashThreshold), 215, 75, &x, &y, &w, &h);
//     tft.fillRect(x, y, w, h, BLACK);
//     tft.setCursor(215, 75);
//     tft.println(flashThreshold);
//     prevFlashThreshold = flashThreshold;
//   }
//   // update text of TEST for when running flash test
//   else if (valueField == "Test Button") {
//     tft.setCursor(200, 140);
//     tft.setFont(&Lato_Black_34);
//     tft.setTextColor(textColour);
//     tft.println("TEST");
//   }
// }
