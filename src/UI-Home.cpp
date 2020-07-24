#include "GlobalVariables.h"
#include "MiscFunctions.h"
#include "StepperControl.h"
#include "UI-Main.h"
#include "UI-Home.h"

namespace home_screen {
  #define num_btns 6
  #define num_tchs 5
  gfxButton *btn_array[num_btns];
  gfxTouch  *tch_array[num_tchs];
  // draw logo
  // tft.drawBitmap(40, 0, logo, 240, 82, WHITE);

  gfxButton btn_Logo     =   gfxB.initBitmapButton( logo,              50,   5,   380,   105,     CUSTOM_YELLOW );
  gfxButton btn_Manual   =   gfxB.initButton(       "fillRoundRect",   30,  120,  180,    80, 15, CUSTOM_BLUE   );
  gfxButton btn_Auto     =   gfxB.initButton(       "fillRoundRect",  270,  120,  180,    80, 15, CUSTOM_GREEN  );
  gfxButton btn_Home     =   gfxB.initBitmapButton( target,            40,  230,   80,    80,     WHITE         );
  gfxButton btn_Flash    =   gfxB.initBitmapButton( flashBulb,        200,  230,   80,    80,     WHITE         );
  gfxButton btn_Rotate   =   gfxB.initBitmapButton( rotate,           360,  230,   80,    80,     WHITE         );
  gfxTouch  tch_Auto     =   gfxT.addMomentary( btn_Auto,    func_Auto,    0 );
  gfxTouch  tch_Manual   =   gfxT.addMomentary( btn_Manual,  func_Manual,  0 );
  gfxTouch  tch_Home     =   gfxT.addMomentary( btn_Home,    func_Home,    0 );
  gfxTouch  tch_Flash    =   gfxT.addMomentary( btn_Flash,   func_Flash,   0 );
  gfxTouch  tch_Rotate   =   gfxT.addMomentary( btn_Rotate,  func_Rotate,  0 );


  void initHomeButtons() {
    btn_array[0] = &btn_Manual;
    btn_array[1] = &btn_Auto;
    btn_array[2] = &btn_Logo;
    btn_array[3] = &btn_Home;
    btn_array[4] = &btn_Flash;
    btn_array[5] = &btn_Rotate;

    tch_array[0] = &tch_Manual;
    tch_array[1] = &tch_Auto;
    tch_array[2] = &tch_Home;
    tch_array[3] = &tch_Flash;
    tch_array[4] = &tch_Rotate;

    btn_array[0]->addBorder(4, WHITE);
    btn_array[1]->addBorder(4, WHITE);
  }


  void populateHomeScreen() {
    setCurrentScreen("Home");
    // draw buttons
    for (int i=0; i < num_btns; i++) {
      btn_array[i]->drawButton(tft);
    }
    // draw text
    btn_Auto.writeTextCentre(   tft, Lato_Black_34, String("Auto"),   WHITE);
    btn_Manual.writeTextCentre( tft, Lato_Black_34, String("Manual"), WHITE);
  }


  void checkHomeButtons(int touch_x, int touch_y) {
    for (int i=0; i < num_tchs; i++) {
      tch_array[i]->checkButton("Home", touch_x, touch_y);
    }
  }


  void func_Auto(bool btnActive) {
    if (btnActive) {
      populateScreen("Auto");
    }
  }


  void func_Manual(bool btnActive) {
    if (btnActive) {
      populateScreen("Manual");
    }
  }


  void func_Home(bool btnActive) {
    if (btnActive) {
      btn_Home.drawButton(tft, CUSTOM_RED);
      runHomingSequence = true;
      homeRail();
      btn_Home.drawButton(tft, WHITE);
    }
  }


  void func_Flash(bool btnActive) {
    if (btnActive) {
      populateScreen("Flash");
    }
  }


  void func_Rotate(bool btnActive) {
    if (btnActive) {
      rotateScreen(); // rotate screen, touch controls, and joystick
      populateScreen("Home");
    }
  }
}
