#include "UI-Main.h"
#include "UI-Home.h"
#include "MiscFunctions.h"
#include "StepperControl.h"

namespace home_screen {
  #define num_btns_Home 6
  #define num_tchs_Home 5
  gfxButton btn_array_Home[num_btns_Home];
  gfxTouch  tch_array_Home[num_tchs_Home];
  // draw logo
  // tft.drawBitmap(40, 0, logo, 240, 82, WHITE);

  gfxButton btn_Logo     =   gfxB.initBitmapButton( logo,              50,   5,   380,  105,    CUSTOM_RED         );
  gfxButton btn_Manual   =   gfxB.initButton(       "fillRoundRect",   30,  120,  180,  80, 15,  CUSTOM_BLUE   );
  gfxButton btn_Auto     =   gfxB.initButton(       "fillRoundRect",  270,  120,  180,  80, 15,  CUSTOM_GREEN  );
  gfxButton btn_Home     =   gfxB.initBitmapButton( target,            40,  230,   80,  80,     WHITE         );
  gfxButton btn_Flash    =   gfxB.initBitmapButton( flashBulb,        200,  230,   80,  80,     WHITE         );
  gfxButton btn_Rotate   =   gfxB.initBitmapButton( rotate,           360,  230,   80,  80,     WHITE         );
  gfxTouch  tch_Auto     =   gfxT.addMomentary( btn_Auto,    func_Auto,    20 );
  gfxTouch  tch_Manual   =   gfxT.addMomentary( btn_Manual,  func_Manual,  20 );
  gfxTouch  tch_Home     =   gfxT.addMomentary( btn_Home,    func_Home,    20 );
  gfxTouch  tch_Flash    =   gfxT.addMomentary( btn_Flash,   func_Flash,   20 );
  gfxTouch  tch_Rotate   =   gfxT.addMomentary( btn_Rotate,  func_Rotate,  20 );


  void initHomeButtons() {
    btn_array_Home[0] = btn_Manual;
    btn_array_Home[0].addBorder(4, WHITE);
    btn_array_Home[1] = btn_Auto;
    btn_array_Home[1].addBorder(4, WHITE);
    btn_array_Home[2] = btn_Logo;
    btn_array_Home[3] = btn_Home;
    btn_array_Home[4] = btn_Flash;
    btn_array_Home[5] = btn_Rotate;

    tch_array_Home[0] = tch_Manual;
    tch_array_Home[1] = tch_Auto;
    tch_array_Home[2] = tch_Home;
    tch_array_Home[3] = tch_Flash;
    tch_array_Home[4] = tch_Rotate;
  }


  void populateHomeScreen() {
    setCurrentScreen("Home");
    // draw buttons
    for (int i=0; i < num_btns_Home; i++) {
      btn_array_Home[i].drawButton(tft);
    }
    // draw text
    btn_Auto.writeTextCentre(   tft, Lato_Black_34, String("Auto"),   WHITE);
    btn_Manual.writeTextCentre( tft, Lato_Black_34, String("Manual"), WHITE);
  }


  void checkHomeButtons(int touch_x, int touch_y, int touch_z) {
    // if screen pressed
    if (touch_z >= 50 && touch_z <= 1000) {
      for (int i=0; i < num_tchs_Home; i++) {
        tch_array_Home[i].checkButton("Home", touch_x, touch_y);
      }
    }

    // TODO - something better so we don't check the entire array every loop??
    // else if screen not pressed, re-enable toggle
    // if (touch_z == 0) {
    //   for (int i=0; i < num_btns_Home; i++) {
    //     if (tch_array_Home[i].touchType == "toggle") {
    //       tch_array_Home[i].setTouchReset(true);
    //     }
    //   }
    // }
  }


  void func_Auto(bool btnActive) {
    if (btnActive == true) {
      populateScreen("Auto");
    }
  }


  void func_Manual(bool btnActive) {
    if (btnActive == true) {
      populateScreen("Manual");
    }
  }


  void func_Home(bool btnActive) {
    if (btnActive == true) {
      btn_Home.drawButton(tft, CUSTOM_RED);
      runHomingSequence = true;
      homeRail();
      btn_Home.drawButton(tft, WHITE);
    }
  }


  void func_Flash(bool btnActive) {
    if (btnActive == true) {
      populateScreen("Flash");
    }
  }


  void func_Rotate(bool btnActive) {
    if (btnActive == true) {
      rotateScreen(); // rotate screen, touch controls, and joystick
      populateScreen("Home");
    }
  }
}
