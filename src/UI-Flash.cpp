#include "GlobalVariables.h"
#include "ShutterControl.h"
#include "UI-Main.h"
#include "UI-Flash.h"

namespace flash_screen {

  #define num_btns 8
  #define num_tchs 4
  gfxButton *btn_array[num_btns];
  gfxTouch  *tch_array[num_tchs];

  // TODO maybe only have one button to set values and just record the max and min value
  gfxButton btn_FlashOff   =   gfxB.initButton(       "fillRoundRect",    0,   20,    160,   80,   15,  CUSTOM_RED    );
  gfxButton btn_OffValue   =   gfxB.initButton(       "fillRoundRect",    0,   20,    160,   80,   15,  CUSTOM_RED    );
  gfxButton btn_FlashOn    =   gfxB.initButton(       "fillRoundRect",    0,  120,    160,   80,   15,  CUSTOM_GREEN  );
  gfxButton btn_OnValue    =   gfxB.initButton(       "fillRoundRect",    0,  120,    160,   80,   15,  CUSTOM_GREEN  );
  gfxButton btn_Threshold  =   gfxB.initButton(       "fillRoundRect",  320,   20,    160,   80,   15,  CUSTOM_BLUE   );
  gfxButton btn_ThreshVal  =   gfxB.initButton(       "fillRoundRect",  320,   20,    160,   80,   15,  CUSTOM_BLUE   );
  gfxButton btn_FlashTest  =   gfxB.initBlankButton(                    320,  120,    160,   80                       );
  gfxButton btn_Back       =   gfxB.initBitmapButton( backArrow,        220,  220,     80,   80,        WHITE         );
  gfxTouch  tch_FlashOff   =   gfxT.addToggle(    btn_OffValue,   func_FlashOff,  0 );
  gfxTouch  tch_FlashOn    =   gfxT.addToggle(    btn_OnValue,    func_FlashOn,   0 );
  gfxTouch  tch_FlashTest  =   gfxT.addMomentary( btn_FlashTest,  func_FlashTest, 0 );
  gfxTouch  tch_Back       =   gfxT.addMomentary( btn_Back,       func_Back,      0 );

  void initFlashButtons() {
    btn_array[0] = &btn_FlashOff;
    btn_array[1] = &btn_OffValue;
    btn_array[1]->addBorder(3, WHITE);
    btn_array[2] = &btn_FlashOn;
    btn_array[3] = &btn_OnValue;
    btn_array[3]->addBorder(3, WHITE);
    btn_array[4] = &btn_Threshold;
    btn_array[5] = &btn_ThreshVal;
    btn_array[5]->addBorder(3, WHITE);
    btn_array[6] = &btn_FlashTest;
    btn_array[7] = &btn_Back;

    tch_array[0] = &tch_FlashOff;
    tch_array[1] = &tch_FlashOn;
    tch_array[2] = &tch_FlashTest;
    tch_array[3] = &tch_Back;
  }


  void populateFlashScreen() {
    setCurrentScreen("Flash");
    flashReady = flashStatus(); // get latest values
    // draw buttons
    for (int i=0; i < num_btns; i++) {
      btn_array[i]->drawButton(tft);
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


  void checkFlashButtons(int touch_x, int touch_y) {
    for (int i=0; i < num_tchs; i++) {
      tch_array[i]->checkButton("Flash", touch_x, touch_y);
    }
  }

  void func_FlashOff(bool btnActive) {
    if (btnActive) {
      setEditFlashOffValue(true);
      btn_OffValue.writeTextBottomCentre( tft, Arimo_Bold_30, String(flashOffValue), YELLOW);
      btn_ThreshVal.writeTextBottomCentre(tft, Arimo_Bold_30, String(flashThreshold), WHITE);
    }
    else {
      setEditFlashOffValue(false);
      btn_OffValue.writeTextBottomCentre( tft, Arimo_Bold_30, String(flashOffValue), WHITE);
      btn_ThreshVal.writeTextBottomCentre(tft, Arimo_Bold_30, String(flashThreshold), WHITE);
    }
  }


  void func_FlashOn(bool btnActive) {
    if (btnActive) {
      setEditFlashOnValue(true);
      btn_OnValue.writeTextBottomCentre(tft, Arimo_Bold_30, String(flashOnValue), YELLOW);
      btn_ThreshVal.writeTextBottomCentre(tft, Arimo_Bold_30, String(flashThreshold), WHITE);
    }
    else {
      setEditFlashOnValue(false);
      btn_OnValue.writeTextBottomCentre(tft, Arimo_Bold_30, String(flashOnValue), WHITE);
      btn_ThreshVal.writeTextBottomCentre(tft, Arimo_Bold_30, String(flashThreshold), WHITE);
    }
  }


  void func_FlashTest(bool btnActive) {
    if (btnActive) {
     setTestingFlash(true); // prevents leaving screen while testing flash
      // enable shutter
      if (!isShutterEnabled()) {
        setShutterEnabled(true);
      }
      // print as yellow until result returned
      btn_FlashTest.writeTextTopCentre(tft, Arimo_Bold_30, String("TEST"), YELLOW);
      btn_FlashTest.writeTextBottomCentre(tft, Arimo_Bold_30, String("FLASH"), YELLOW);

      // trigger shutter
      triggerShutter();

      // check result
      if (!hasShutterTriggered()) {
        btn_FlashTest.writeTextTopCentre(tft, Arimo_Bold_30, String("TEST"), CUSTOM_RED);
        btn_FlashTest.writeTextBottomCentre(tft, Arimo_Bold_30, String("FLASH"), CUSTOM_RED);
      }
      else if (hasShutterTriggered()) {
        btn_FlashTest.writeTextTopCentre(tft, Arimo_Bold_30, String("TEST"), CUSTOM_GREEN);
        btn_FlashTest.writeTextBottomCentre(tft, Arimo_Bold_30, String("FLASH"), CUSTOM_GREEN);
      }
      setTestingFlash(false);
    }
  }


  void func_Back(bool btnActive) {
    if (btnActive && !canEditFlashOnValue() && !canEditFlashOffValue() && !isTestingFlash()) {
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
    if (abs(flashValue - flashOffValue) > 1 && canEditFlashOffValue()) {
      func_FlashOff(true);
      // set OFF value for flash
      flashOffValue = flashValue;
      }
    if (abs(flashValue - flashOnValue) > 1 && canEditFlashOnValue()) {
      func_FlashOn(true);
      // set ON value for flash
      flashOnValue = flashValue;
    }
  }

}
