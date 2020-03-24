#include "TFT-Main.h"
#include "TFT-Home.h"
#include "MiscFunctions.h"
#include "ShutterControl.h"
#include "StepperControl.h"
#include "TouchControl.h"
#include "UserInterface.h"
#include "gfxButton.h"

#define numButtons 5
gfxButton btn_Array[numButtons];
gfxTouch  tch_Array[numButtons];

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

  btn_Array[0] = btn_Manual;
  btn_Array[1] = btn_Auto;
  btn_Array[2] = btn_Home;
  btn_Array[3] = btn_Flash;
  btn_Array[4] = btn_Rotate;

  tch_Array[0] = tch_Manual;
  tch_Array[1] = tch_Auto;
  tch_Array[2] = tch_Home;
  tch_Array[3] = tch_Flash;
  tch_Array[4] = tch_Rotate;
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
