#include "GlobalVariables.h"
#include "ShutterControl.h"
#include "menu/UI-Main.h"
#include "menu/UI-Flash.h"
#include "menu/UI-Manual.h"
#include "menu/UI-Auto.h"

namespace flash_screen {

    #define num_btns 6
    gfxButton *btn_array[num_btns];

    // TODO maybe only have one button to set values and just record the max and min value
    gfxButton btn_FlashOff      =   btn.initButton("Off Value",     "fillRoundRect",    0,      20,     160,    80,     15,     CUSTOM_RED,     true);
    gfxButton btn_FlashOn       =   btn.initButton("On Value",      "fillRoundRect",    0,      120,    160,    80,     15,     CUSTOM_GREEN,   true);
    gfxButton btn_Threshold     =   btn.initButton("Threshold",     "fillRoundRect",    320,    20,     160,    80,     15,     CUSTOM_BLUE,    true);
    gfxButton btn_FlashTest     =   btn.initTransparentButton(          320,    120,    160,    80,             true);
    gfxButton btn_FlashSensor   =   btn.initBitmapButton(flashBulb,     200,    20,     80,     80, CUSTOM_RED, true);
    gfxButton btn_Back          =   btn.initBitmapButton(backArrow,     200,    220,    80,     80, WHITE,      true);

    void initFlashButtons() {
        btn_array[0] = &btn_FlashOff;
        btn_array[1] = &btn_FlashOn;
        btn_array[2] = &btn_Threshold;
        btn_array[3] = &btn_FlashTest;
        btn_array[4] = &btn_Back;
        btn_array[5] = &btn_FlashSensor;

        btn_FlashOff.addToggle(func_FlashOff,0);
        btn_FlashOn.addToggle(func_FlashOn, 0);
        btn_FlashTest.addMomentary(func_FlashTest, 0);
        btn_FlashSensor.addToggle(func_FlashSensor, 0);
        btn_Back.addMomentary(func_Back, 0);

        btn_FlashOff.addBorder(3, WHITE);
        btn_FlashOn.addBorder(3, WHITE);
        btn_Threshold.addBorder(3, WHITE);
    }


    void populateFlashScreen() {
        setCurrentScreen("Flash");
        isFlashReady(); // get latest values
        // draw buttons
        for (int i=0; i < num_btns; i++) {
            btn_array[i]->drawButton();
        }
        // draw text
        btn_FlashOff.writeTextTopCentre(Arimo_Regular_30,     WHITE);
        btn_FlashOff.writeTextBottomCentre(Arimo_Bold_30,     WHITE,    String(flashOffValue));
        btn_FlashOn.writeTextTopCentre(Arimo_Regular_30,        WHITE);
        btn_FlashOn.writeTextBottomCentre(Arimo_Bold_30,        WHITE,    String(flashOnValue));
        btn_Threshold.writeTextTopCentre(Arimo_Regular_30,    WHITE);
        btn_Threshold.writeTextBottomCentre(Arimo_Bold_30,    WHITE,    String(flashThreshold));
        btn_FlashTest.writeTextTopCentre(Arimo_Bold_30,         WHITE,    String("TEST"));
        btn_FlashTest.writeTextBottomCentre(Arimo_Bold_30,    WHITE,    String("FLASH"));
    }


    void checkFlashButtons(int touch_x, int touch_y) {
        for (int i=0; i < num_btns; i++) {
            if (btn_array[i]->isTactile()) {
                btn_array[i]->contains(touch_x, touch_y);
            }
        }
    }

    void func_FlashOff(bool btnActive) {
        if (btnActive) {
            setEditFlashOffValue(true);
            btn_FlashOff.writeTextBottomCentre( Arimo_Bold_30, YELLOW, String(flashOffValue));
            btn_Threshold.writeTextBottomCentre(Arimo_Bold_30, WHITE, String(flashThreshold));
        }
        else {
            setEditFlashOffValue(false);
            btn_FlashOff.writeTextBottomCentre( Arimo_Bold_30, WHITE, String(flashOffValue));
            btn_Threshold.writeTextBottomCentre(Arimo_Bold_30, WHITE, String(flashThreshold));
        }
    }


    void func_FlashOn(bool btnActive) {
        if (btnActive) {
            setEditFlashOnValue(true);
            btn_FlashOn.writeTextBottomCentre(Arimo_Bold_30, YELLOW, String(flashOnValue));
            btn_Threshold.writeTextBottomCentre(Arimo_Bold_30, WHITE, String(flashThreshold));
        }
        else {
            setEditFlashOnValue(false);
            btn_FlashOn.writeTextBottomCentre(Arimo_Bold_30, WHITE, String(flashOnValue));
            btn_Threshold.writeTextBottomCentre(Arimo_Bold_30, WHITE, String(flashThreshold));
        }
    }


    void func_FlashSensor(bool btnActive) {
        if (btnActive) {
            btn_FlashSensor.updateColour(CUSTOM_GREEN);
            btn_FlashSensor.drawButton();
            setFlashSensorEnabled(true);
            // auto_screen::stackStatus(newStep); // always reset in case switched mid-procedure
        }
        else if (!btnActive) {
            btn_FlashSensor.updateColour(CUSTOM_RED);
            btn_FlashSensor.drawButton();
            setFlashSensorEnabled(false);
            // auto_screen::stackStatus(newStep); // always reset in case switched mid-procedure
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
            btn_FlashTest.writeTextTopCentre(Arimo_Bold_30, YELLOW, String("TEST"));
            btn_FlashTest.writeTextBottomCentre(Arimo_Bold_30, YELLOW, String("FLASH"));

            // trigger shutter
            triggerShutter();
            // wait for result
            setLastMillis(millis());
            while (millis() - getLastMillis() <= 6500) {
                runFlashProcedure(false);
                if (getCurrentStage() == flashSuccessful) {
                    break;
                }
            }
            // reset stackProcedureStage
            auto_screen::stackStatus(newStep);

            // check result
            if (!hasShutterTriggered()) {
                btn_FlashTest.writeTextTopCentre(Arimo_Bold_30, CUSTOM_RED, String("TEST"));
                btn_FlashTest.writeTextBottomCentre(Arimo_Bold_30, CUSTOM_RED, String("FLASH"));
            }
            else if (hasShutterTriggered()) {
                btn_FlashTest.writeTextTopCentre(Arimo_Bold_30, CUSTOM_GREEN, String("TEST"));
                btn_FlashTest.writeTextBottomCentre(Arimo_Bold_30, CUSTOM_GREEN, String("FLASH"));
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
    void updateGodoxValue() {
        // get latest godoxValue reading
        isFlashReady();

        // if difference from previous reading > 1, updates value on screen
        if (abs(getGodoxValue() - flashOffValue) > 1 && canEditFlashOffValue()) {
            func_FlashOff(true);
            // set OFF value for flash
            flashOffValue = getGodoxValue();
            }
        if (abs(getGodoxValue() - flashOnValue) > 1 && canEditFlashOnValue()) {
            func_FlashOn(true);
            // set ON value for flash
            flashOnValue = getGodoxValue();
        }
    }

}
