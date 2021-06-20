#include "GlobalVariables.h"
#include "MiscFunctions.h"
#include "StepperControl.h"
#include "menu/UI-Main.h"
#include "menu/UI-Home.h"

namespace home_screen {
  #define num_btns 7
  gfxButton *btn_array[num_btns];


  gfxButton btn_Manual    =   btn.initButton("Manual", "fillRoundRect",  30,   120,  180,  80, 15, CUSTOM_BLUE,  true  );
  gfxButton btn_Auto      =   btn.initButton("Auto",   "fillRoundRect",  270,  120,  180,  80, 15, CUSTOM_GREEN, true  );
  gfxButton btn_Logo      =   btn.initBitmapButton(logo,        50,   5,    380,  105,  CUSTOM_YELLOW,  true  );
  gfxButton btn_Home      =   btn.initBitmapButton(home,        20,   230,  80,   80,   WHITE,          true  );
  gfxButton btn_Target    =   btn.initBitmapButton(target,      140,  230,  80,   80,   WHITE,          true  );
  gfxButton btn_Flash     =   btn.initBitmapButton(flashBulb,   260,  230,  80,   80,   WHITE,          true  );
  // gfxButton btn_Rotate    =   btn.initBitmapButton(rotate,      380,  230,  80,   80,   WHITE,          true  );
  gfxButton btn_Config    =   btn.initBitmapButton(cogWheel,    380,  230,  80, 80,     WHITE,          true  );


  void initHomeButtons() {
    btn_array[0] = &btn_Manual;
    btn_array[1] = &btn_Auto;
    btn_array[2] = &btn_Logo;
    btn_array[3] = &btn_Home;
    btn_array[4] = &btn_Target;
    btn_array[5] = &btn_Flash;
    // btn_array[6] = &btn_Rotate;
    btn_array[6] = &btn_Config;

    btn_Manual.addMomentary(func_Manual, 0);
    btn_Auto.addMomentary(func_Auto, 0);
    btn_Home.addMomentary(func_Home, 0);
    btn_Target.addMomentary(func_Target, 0);
    btn_Flash.addMomentary( func_Flash, 0);
    // btn_Rotate.addMomentary(func_Rotate, 0);
    btn_Config.addMomentary(func_Config, 0);

    btn_Manual.addBorder(4, WHITE);
    btn_Auto.addBorder(4, WHITE);
  }


  void populateHomeScreen() {
    setCurrentScreen("Home");
    // draw buttons
    for (int i=0; i < num_btns; i++) {
      btn_array[i]->drawButton();
    }
    // draw text
    btn_Auto.writeTextCentre(Lato_Black_34, WHITE);
    btn_Manual.writeTextCentre(Lato_Black_34, WHITE);
  }


  void checkHomeButtons(int touch_x, int touch_y) {
    for (int i=0; i < num_btns; i++) {
      if (btn_array[i]->isTactile()) {
        btn_array[i]->contains(touch_x, touch_y);
      }
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
      btn_Home.drawButton(CUSTOM_RED);
      runHomingSequence = true;
      homeRail();
      btn_Home.drawButton(WHITE);
    }
  }


  void func_Target(bool btnActive) {
    if (btnActive) {
      populateScreen("Target");
    }
  }


  void func_Flash(bool btnActive) {
    if (btnActive) {
      populateScreen("Flash");
    }
  }


  void func_Config(bool btnActive) {
    if (btnActive) {
      populateScreen("Config");
    }
  }


  void func_Rotate(bool btnActive) {
    if (btnActive) {
      rotateScreen(); // rotate screen, touch controls, and joystick
      populateScreen("Home");
    }
  }
}
