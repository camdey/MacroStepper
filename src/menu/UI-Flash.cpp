#include "AutoStack.h"
#include "CameraControl.h"
#include "UserInterface.h"
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
    gfxButton btn_FlashTest     =   btn.initTransparentButton(              320,    120,    160,    80, true);
    gfxButton btn_FlashSensor   =   btn.initBitmapButton(flashBulb,         200,    20,     80,     80, CUSTOM_RED, BLACK,  true);
    gfxButton btn_Back          =   btn.initRGBBitmapButton(rgb_back_80,    200,    220,    80,     80, true);

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
        ui.activeScreen(routines::ui_Flash);
        camera.checkFlashAvailability(); // get latest values
        // draw buttons
        for (int i=0; i < num_btns; i++) {
            btn_array[i]->drawButton();
        }
        // draw text
        btn_FlashOff.writeTextTopCentre(Arimo_Regular_30,   WHITE);
        btn_FlashOff.writeTextBottomCentre(Arimo_Bold_30,   WHITE,  String(camera.flashOffValue()));
        btn_FlashOn.writeTextTopCentre(Arimo_Regular_30,    WHITE);
        btn_FlashOn.writeTextBottomCentre(Arimo_Bold_30,    WHITE,  String(camera.flashOnValue()));
        btn_Threshold.writeTextTopCentre(Arimo_Regular_30,  WHITE);
        btn_Threshold.writeTextBottomCentre(Arimo_Bold_30,  WHITE,  String(camera.flashThreshold()));
        btn_FlashTest.writeTextTopCentre(Arimo_Bold_30,     WHITE,  String("TEST"));
        btn_FlashTest.writeTextBottomCentre(Arimo_Bold_30,  WHITE,  String("FLASH"));
    }


    void checkFlashButtons(int touch_x, int touch_y) {
        for (int i=0; i < num_btns; i++) {
            if (btn_array[i]->isTactile() && !btn_array[i]->isHidden()) {
                btn_array[i]->contains(touch_x, touch_y);
            }
        }
    }

    void func_FlashOff(bool btnActive) {
        if (btnActive) {
            ui.canEdit(routines::btn_flashOff, true);
            ui.canEdit(routines::btn_shutterDelay, true);
            btn_FlashOff.writeTextBottomCentre( Arimo_Bold_30, YELLOW, String(camera.flashOffValue()));
            btn_Threshold.writeTextBottomCentre(Arimo_Bold_30, WHITE, String(camera.flashThreshold()));
        }
        else {
            ui.canEdit(routines::btn_flashOff, false);
            btn_FlashOff.writeTextBottomCentre( Arimo_Bold_30, WHITE, String(camera.flashOffValue()));
            btn_Threshold.writeTextBottomCentre(Arimo_Bold_30, WHITE, String(camera.flashThreshold()));
        }
    }


    void func_FlashOn(bool btnActive) {
        if (btnActive) {
            ui.canEdit(routines::btn_flashOn, true);
            btn_FlashOn.writeTextBottomCentre(Arimo_Bold_30, YELLOW, String(camera.flashOnValue()));
            btn_Threshold.writeTextBottomCentre(Arimo_Bold_30, WHITE, String(camera.flashThreshold()));
        }
        else {
            ui.canEdit(routines::btn_flashOn, false);
            btn_FlashOn.writeTextBottomCentre(Arimo_Bold_30, WHITE, String(camera.flashOffValue()));
            btn_Threshold.writeTextBottomCentre(Arimo_Bold_30, WHITE, String(camera.flashThreshold()));
        }
    }


    void func_FlashSensor(bool btnActive) {
        if (btnActive) {
            btn_FlashSensor.updateColour(CUSTOM_GREEN);
            btn_FlashSensor.drawButton();
            camera.flashSensorEnabled(true);
            // auto_screen::stackStatus(newStep); // always reset in case switched mid-procedure
        }
        else if (!btnActive) {
            btn_FlashSensor.updateColour(CUSTOM_RED);
            btn_FlashSensor.drawButton();
            camera.flashSensorEnabled(false);
            // auto_screen::stackStatus(newStep); // always reset in case switched mid-procedure
        }
    }


    void func_FlashTest(bool btnActive) {
        if (btnActive) {
            camera.isTestingFlash(true); // prevents leaving screen while testing flash
            // enable shutter
            if (!camera.shutterEnabled()) {
                camera.shutterEnabled(true);
            }
            // print as yellow until result returned
            btn_FlashTest.writeTextTopCentre(Arimo_Bold_30, YELLOW, String("TEST"));
            btn_FlashTest.writeTextBottomCentre(Arimo_Bold_30, YELLOW, String("FLASH"));

            // trigger shutter
            camera.triggerShutter(true);
            while (millis() - camera.lastFlashMillis() <= 6500) {
                camera.triggerShutter(false);
                if (camera.status() == routines::shutterCompleted) {
                    break;
                }
            }

            // check result
            if (!camera.photoTaken()) {
                btn_FlashTest.writeTextTopCentre(Arimo_Bold_30, CUSTOM_RED, String("TEST"));
                btn_FlashTest.writeTextBottomCentre(Arimo_Bold_30, CUSTOM_RED, String("FLASH"));
                // reset stackProcedureStage if active
                if (stack.status() != routines::inactive) {
                    stack.status(routines::newShutter);
                }
            }
            else if (camera.photoTaken()) {
                btn_FlashTest.writeTextTopCentre(Arimo_Bold_30, CUSTOM_GREEN, String("TEST"));
                btn_FlashTest.writeTextBottomCentre(Arimo_Bold_30, CUSTOM_GREEN, String("FLASH"));
                if (stack.status() != routines::inactive) {
                    stack.status(routines::newMovement);
                }
            }
            camera.isTestingFlash(false);
        }
    }


    void func_Back(bool btnActive) {
        if (btnActive && !ui.canEdit(routines::btn_flashOn) && !ui.canEdit(routines::btn_flashOff) && !camera.isTestingFlash()) {
            // go back to start screen
            ui.populateScreen(ui.previousScreen());
        }
    }


    /***********************************************************************
    Updates the screen with light sensor readings of the Flash LED. Used
    to calibrate optimal trigger point for flash given lighting conditions
    at the time that may affect the light sensor.
    ***********************************************************************/
    void updateFlashSensorValue() {
        // get latest flashSensorValue reading
        camera.checkFlashAvailability();

        // if difference from previous reading > 1, updates value on screen
        if (abs(camera.sensorValue() - camera.flashOffValue()) > 1 && ui.canEdit(routines::btn_flashOff)) {
            func_FlashOff(true);
            // set OFF value for flash
            camera.flashOffValue(camera.sensorValue());
            }
        if (abs(camera.sensorValue() - camera.flashOnValue()) > 1 && ui.canEdit(routines::btn_flashOn)) {
            func_FlashOn(true);
            // set ON value for flash
            camera.flashOnValue(camera.sensorValue());
        }
    }

}
