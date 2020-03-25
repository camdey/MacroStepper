#include "TFT-Main.h"
#include "TFT-Home.h"
#include "MiscFunctions.h"
#include "ShutterControl.h"
#include "StepperControl.h"
#include "TouchControl.h"
#include "UserInterface.h"
#include "gfxButton.h"

#define num_btns_Home 5
gfxButton btn_array_Home[num_btns_Home];
gfxTouch  tch_array_Home[num_btns_Home];


gfxButton btn_Auto     =   gfxB.initButton(       "main", "fillRoundRect",  170,  100,  130,  60,   8, CUSTOM_GREEN );
gfxButton btn_Manual   =   gfxB.initButton(       "main", "fillRoundRect",  20,   100,  130,  60,   8, CUSTOM_BLUE  );
gfxButton btn_Home     =   gfxB.initBitmapButton( "main", house,            30,   190,  50,   42,      WHITE        );
gfxButton btn_Flash    =   gfxB.initBitmapButton( "main", flash,            145,  190,  40,   42,      WHITE        );
gfxButton btn_Rotate   =   gfxB.initBitmapButton( "main", rotate_icon,      250,  190,  45,   39,      WHITE        );
gfxTouch  tch_Auto     =   gfxT.addMomentary( btn_Auto,    func_Auto,    "btn_Auto",    20  );
gfxTouch  tch_Manual   =   gfxT.addMomentary( btn_Manual,  func_Manual,  "btn_Manual",  20  );
gfxTouch  tch_Home     =   gfxT.addMomentary( btn_Home,    func_Home,    "btn_Home",    20  );
gfxTouch  tch_Flash    =   gfxT.addMomentary( btn_Flash,   func_Flash,   "btn_Flash",   20  );
gfxTouch  tch_Rotate   =   gfxT.addMomentary( btn_Rotate,  func_Rotate,  "btn_Rotate",  20  );


void initHomeButtons() {
  gfxT.setToggleDebounce(250);
  gfxT.setMomentaryDebounce(100);

  btn_array_Home[0] = btn_Manual;
  btn_array_Home[1] = btn_Auto;
  btn_array_Home[2] = btn_Home;
  btn_array_Home[3] = btn_Flash;
  btn_array_Home[4] = btn_Rotate;

  tch_array_Home[0] = tch_Manual;
  tch_array_Home[1] = tch_Auto;
  tch_array_Home[2] = tch_Home;
  tch_array_Home[3] = tch_Flash;
  tch_array_Home[4] = tch_Rotate;
}


void populateHomeScreen() {
  // draw logo
  tft.drawBitmap(40, 0, logo, 240, 82, WHITE);
  // draw buttons
  for (int i=0; i < num_btns_Home; i++) {
    btn_array_Home[i].drawButton(tft);
  }
  // draw text
  btn_Auto.writeText(   tft, Arimo_Bold_30, String("Auto"),   BLACK, "c");
  btn_Manual.writeText( tft, Arimo_Bold_30, String("Manual"), BLACK, "c");
}


void checkHomeButtons(int touch_x, int touch_y, int touch_z) {
  // if screen pressed
  if (touch_z >= 100 && touch_z <= 1000) {
    for (int i=0; i < num_btns_Home; i++) {
      tch_array_Home[i].checkButton("home", touch_x, touch_y);
    }
  }

  // TODO - something better so we don't check the entire array every loop??
  // else if screen not pressed, re-enable toggle
  if (touch_z == 0) {
    for (int i=0; i < num_btns_Home; i++) {
      if (tch_array_Home[i].touchType == "toggle") {
        tch_array_Home[i].toggleCoolOff();
      }
    }
  }
}


void func_Auto(bool btnActive) {
  if (btnActive == true) {
    autoScreen();
  }
  else {
    btn_Auto.drawButton(tft, CUSTOM_GREEN);
    btn_Auto.writeText(tft, Arimo_Bold_30, String("Auto"), BLACK, "c");
  }
}


void func_Manual(bool btnActive) {
  if (btnActive == true) {
    manualScreen();
  }
  else {
    btn_Manual.drawButton(tft, CUSTOM_BLUE);
    btn_Manual.writeText(tft, Arimo_Bold_30, String("Manual"), BLACK, "c");
  }
}


void func_Home(bool btnActive) {
  if (btnActive == true) {
    btn_Home.drawButton(tft, CUSTOM_RED);
    runHomingSequence = true;
    homeRail();
    btn_Home.drawButton(tft, WHITE);
  }
  else {
    btn_Home.drawButton(tft, WHITE);
  }
}


void func_Flash(bool btnActive) {
  if (btnActive == true) {
    flashScreen();
  }
  else {
    btn_Flash.drawButton(tft, WHITE);
  }
}


void func_Rotate(bool btnActive) {
  if (btnActive == true) {
    rotateScreen(); // rotate screen, touch controls, and joystick
    startScreen();
  }
  else {
    btn_Rotate.drawButton(tft, WHITE);
  }
}
