#include "UI-Main.h"
#include "UI-Flash.h"
#include "ShutterControl.h"

namespace flash_screen {

  #define num_btns_Flash 8
  #define num_tchs_Flash 4
  gfxButton btn_array_Flash[num_btns_Flash];
  gfxTouch  tch_array_Flash[num_tchs_Flash];

  // TODO maybe only have one button to set values and just record the max and min value
  gfxButton btn_FlashOff   =   gfxB.initButton(       "fillRoundRect",    0,   20,    160,   80,   15,  CUSTOM_RED    );
  gfxButton btn_OffValue   =   gfxB.initButton(       "fillRoundRect",    0,   20,    160,   80,   15,  CUSTOM_RED    );
  gfxButton btn_FlashOn    =   gfxB.initButton(       "fillRoundRect",    0,  120,    160,   80,   15,  CUSTOM_GREEN  );
  gfxButton btn_OnValue    =   gfxB.initButton(       "fillRoundRect",    0,  120,    160,   80,   15,  CUSTOM_GREEN  );
  gfxButton btn_Threshold  =   gfxB.initButton(       "fillRoundRect",  320,   20,    160,   80,   15,  CUSTOM_BLUE   );
  gfxButton btn_ThreshVal  =   gfxB.initButton(       "fillRoundRect",  320,   20,    160,   80,   15,  CUSTOM_BLUE   );
  gfxButton btn_FlashTest  =   gfxB.initBlankButton(                    320,  120,    160,   80                       );
  gfxButton btn_Back       =   gfxB.initBitmapButton( backArrow,        220,  220,     80,   80,        WHITE         );
  gfxTouch  tch_FlashOff   =   gfxT.addToggle(    btn_OffValue,   func_FlashOff,  20 );
  gfxTouch  tch_FlashOn    =   gfxT.addToggle(    btn_OnValue,    func_FlashOn,   20 );
  gfxTouch  tch_FlashTest  =   gfxT.addMomentary( btn_FlashTest,  func_FlashTest, 20 );
  gfxTouch  tch_Back       =   gfxT.addMomentary( btn_Back,       func_Back,      20 );

  void initFlashButtons() {
    btn_array_Flash[0] = btn_FlashOff;
    btn_array_Flash[1] = btn_OffValue;
    btn_array_Flash[1].addBorder(3, WHITE);
    btn_array_Flash[2] = btn_FlashOn;
    btn_array_Flash[3] = btn_OnValue;
    btn_array_Flash[3].addBorder(3, WHITE);
    btn_array_Flash[4] = btn_Threshold;
    btn_array_Flash[5] = btn_ThreshVal;
    btn_array_Flash[5].addBorder(3, WHITE);
    btn_array_Flash[6] = btn_FlashTest;
    btn_array_Flash[7] = btn_Back;

    tch_array_Flash[0] = tch_FlashOff;
    tch_array_Flash[1] = tch_FlashOn;
    tch_array_Flash[2] = tch_FlashTest;
    tch_array_Flash[3] = tch_Back;
  }


  void populateFlashScreen() {
    setCurrentScreen("Flash");
    flashReady = flashStatus(); // get latest values
    // draw buttons
    for (int i=0; i < num_btns_Flash; i++) {
      btn_array_Flash[i].drawButton(tft);
    }
    // draw text
    btn_FlashOff.writeTextTopCentre(    tft, Arimo_Regular_30, String("Off Value"),   WHITE);
    btn_OffValue.writeTextBottomCentre( tft, Arimo_Bold_30, String(flashOffValue), WHITE);
    btn_FlashOn.writeTextTopCentre(     tft, Arimo_Regular_30, String("On Value"),    WHITE);
    btn_OnValue.writeTextBottomCentre(  tft, Arimo_Bold_30, String(flashOnValue),  WHITE);
    btn_Threshold.writeTextTopCentre(   tft, Arimo_Regular_30, String("Threshold"),   WHITE);
    btn_ThreshVal.writeTextBottomCentre(tft, Arimo_Bold_30, String(flashThreshold),WHITE);
    btn_FlashTest.writeTextTopCentre(   tft, Arimo_Bold_30, String("TEST"),  WHITE);
    btn_FlashTest.writeTextBottomCentre(tft, Arimo_Bold_30, String("FLASH"),  WHITE);
  }


  void checkFlashButtons(int touch_x, int touch_y, int touch_z) {
    // if screen pressed
    if (touch_z >= 50 && touch_z <= 1000) {
      for (int i=0; i < num_tchs_Flash; i++) {
        tch_array_Flash[i].checkButton("Flash", touch_x, touch_y);
      }
    }

    // TODO - something better so we don't check the entire array every loop??
    // else if screen not pressed, re-enable toggle
    // if (touch_z == 0) {
    //   for (int i=0; i < num_btns_Flash; i++) {
    //     if (tch_array_Flash[i].touchType == "toggle") {
    //       tch_array_Flash[i].setTouchReset(true);
    //     }
    //   }
    // }
  }

  void func_FlashOff(bool btnActive) {
    if (btnActive == true) {
      editFlashOffValue = true;
      btn_OffValue.writeTextBottomCentre( tft, Arimo_Bold_30, String(flashOffValue), YELLOW);
      btn_ThreshVal.writeTextBottomCentre(tft, Arimo_Bold_30, String(flashThreshold), WHITE);
    }
    else {
      editFlashOffValue = false;
      btn_OffValue.writeTextBottomCentre( tft, Arimo_Bold_30, String(flashOffValue), WHITE);
      btn_ThreshVal.writeTextBottomCentre(tft, Arimo_Bold_30, String(flashThreshold), WHITE);
    }
  }


  void func_FlashOn(bool btnActive) {
    if (btnActive == true) {
      editFlashOnValue = true;
      btn_OnValue.writeTextBottomCentre(tft, Arimo_Bold_30, String(flashOnValue), YELLOW);
      btn_ThreshVal.writeTextBottomCentre(tft, Arimo_Bold_30, String(flashThreshold), WHITE);
    }
    else {
      editFlashOnValue = false;
      btn_OnValue.writeTextBottomCentre(tft, Arimo_Bold_30, String(flashOnValue), WHITE);
      btn_ThreshVal.writeTextBottomCentre(tft, Arimo_Bold_30, String(flashThreshold), WHITE);
    }
  }


  void func_FlashTest(bool btnActive) {
    if (btnActive == true) {
      testFlash = true; // prevents leaving screen while testing flash
      // enable shutter
      if (shutterEnabled == false) {
        toggleShutter();
      }
      // print as yellow until result returned
      btn_FlashTest.writeTextTopCentre(tft, Arimo_Bold_30, String("TEST"), YELLOW);
      btn_FlashTest.writeTextBottomCentre(tft, Arimo_Bold_30, String("FLASH"), YELLOW);

      // trigger shutter
      shutterTriggered = triggerShutter();

      // check result
      if (shutterTriggered == false) {
        btn_FlashTest.writeTextTopCentre(tft, Arimo_Bold_30, String("TEST"), CUSTOM_RED);
        btn_FlashTest.writeTextBottomCentre(tft, Arimo_Bold_30, String("FLASH"), CUSTOM_RED);
      }
      else if (shutterTriggered == true) {
        btn_FlashTest.writeTextTopCentre(tft, Arimo_Bold_30, String("TEST"), CUSTOM_GREEN);
        btn_FlashTest.writeTextBottomCentre(tft, Arimo_Bold_30, String("FLASH"), CUSTOM_GREEN);
      }
      testFlash = false;
    }
  }


  void func_Back(bool btnActive) {
    if (btnActive == true && editFlashOnValue == false && editFlashOffValue == false && testFlash == false) {
      // go back to start screen
      populateScreen("Home");
    }
  }


  /***********************************************************************
  Updates the screen with light sensor readings of the Flash LED. Used
  to calibrate optimal trigger point for flash given lighting conditions
  at the time that may affect the light sensor.
  ***********************************************************************/
  void updateFlashValue() {
    // get latest flashValue reading
    flashReady = flashStatus();

    // if difference from previous reading > 1, updates value on screen
    if (abs(flashValue - flashOffValue) > 1 && editFlashOffValue == true) {
      func_FlashOff(true);
      // set OFF value for flash
      flashOffValue = flashValue;
      }
    if (abs(flashValue - flashOnValue) > 1 && editFlashOnValue == true) {
      func_FlashOn(true);
      // set ON value for flash
      flashOnValue = flashValue;
    }
  }

}
