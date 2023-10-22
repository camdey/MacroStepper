#include "MiscFunctions.h"
#include "CameraControl.h"
#include "UserInterface.h"
#include "menu/UI-Config.h"
#include "menu/UI-Auto.h"

namespace config_screen {
    #define num_btns 5
    gfxButton *btn_array[num_btns];

    gfxButton btn_FlipScreen    =   btn.initRGBBitmapButton(rgb_toggle_off,         240,    10,     112,    50, true);
    gfxButton btn_FlashSensor   =   btn.initRGBBitmapButton(rgb_toggle_off,         240,    70,     112,    50, true);
    gfxButton btn_Stepper       =   btn.initRGBBitmapButton(rgb_toggle_on,          240,    130,    112,    50, true);
    gfxButton btn_Thing2        =   btn.initRGBBitmapButton(rgb_toggle_off,         240,    190,    112,    50, true);
    gfxButton btn_Back          =   btn.initRGBBitmapButton(rgb_back_80,            380,    220,    80,     80, true);

    void initConfigButtons() {
        btn_array[0] = &btn_FlipScreen;
        btn_array[1] = &btn_FlashSensor;
        btn_array[2] = &btn_Stepper;
        btn_array[3] = &btn_Thing2;
        btn_array[4] = &btn_Back;

        btn_Back.addMomentary(func_Back, 0);
        btn_FlipScreen.addToggle(func_FlipScreen, 0);
        btn_FlashSensor.addToggle(func_FlashSensor, 0);
        btn_Stepper.addToggle(func_Stepper, 0);

        btn_Stepper.setToggleActive(true); // init as active
        btn_FlashSensor.setToggleActive(false); // init as inactive
    }


    void populateConfigScreen() {
        ui.activeScreen(routines::ui_Config);
        // draw buttons
        for (int i=0; i < num_btns; i++) {
            btn_array[i]->drawButton();
        }
        // draw text
        tft.setTextColor(WHITE);
        tft.setFont(&Arimo_Bold_24);
        tft.setCursor(5,40);
        tft.print("Flip Screen");
        tft.setCursor(5,100);
        tft.print("Enable Sensor");
        tft.setCursor(5,160);
        tft.print("Enable Stepper");
    }


    void checkConfigButtons(int touch_x, int touch_y) {
        for (int i=0; i < num_btns; i++) {
            if (btn_array[i]->isTactile() && !btn_array[i]->isHidden()) {
                btn_array[i]->contains(touch_x, touch_y);
            }
        }
    }


    void func_Back(bool btnActive) {
        if (btnActive && !ui.canEdit(routines::btn_arrows)) {
            ui.populateScreen(ui.previousScreen());
        }
    }


    // invert screen
    void func_FlipScreen(bool btnActive) {
        if (btnActive) {
            btn_FlipScreen.updateRGBBitmap(rgb_toggle_on);
            ui.rotateScreen(); // rotate screen, touch controls, and joystick
            ui.populateScreen(routines::ui_Config);
        }
        else if (!btnActive) {
            btn_FlipScreen.updateRGBBitmap(rgb_toggle_off);
            ui.rotateScreen(); // rotate screen, touch controls, and joystick
            ui.populateScreen(routines::ui_Config);
        }
    }


    // enable/disable flash sensor reading
    void func_FlashSensor(bool btnActive) {
        if (btnActive) {
            btn_FlashSensor.updateRGBBitmap(rgb_toggle_on);
            btn_FlashSensor.drawNewBitmap(rgb_toggle_on);
            camera.flashSensorEnabled(true);
            // auto_screen::stackStatus(newStep); // always reset in case switched mid-procedure
        }
        else if (!btnActive) {
            btn_FlashSensor.updateRGBBitmap(rgb_toggle_off);
            btn_FlashSensor.drawNewBitmap(rgb_toggle_off);
            camera.flashSensorEnabled(false);
            // auto_screen::stackStatus(newStep); // always reset in case switched mid-procedure
        }
    }


    // enable/disable stepper motor
    void func_Stepper(bool btnActive) {
        if (btnActive) {
            btn_FlashSensor.updateRGBBitmap(rgb_toggle_off);
            btn_FlashSensor.drawNewBitmap(rgb_toggle_off);
            stepper1.enabled(false);
        }
        else if (!btnActive) {
            btn_FlashSensor.updateRGBBitmap(rgb_toggle_on);
            btn_FlashSensor.drawNewBitmap(rgb_toggle_on);
            stepper1.enabled(true);
        }
    }
}
