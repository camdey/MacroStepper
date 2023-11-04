#include "CameraControl.h"
#include "UserInterface.h"
#include "Piezo.h"
#include "LedControl.h"
#include "menu/UI-Config.h"
#include "menu/UI-Auto.h"
#include "HelperFunctions.h"

// add homing function again
// add debug page with readouts of sensor values

namespace config_screen {
    #define num_btns 5
    gfxButton *btn_array[num_btns];

    gfxButton btn_FlipScreen    =   btn.initRGBBitmapButton(rgb_toggle_off,         240,    10,     112,    50, true);
    gfxButton btn_Piezo         =   btn.initRGBBitmapButton(rgb_toggle_on,          240,    70,     112,    50, true);
    gfxButton btn_Stepper       =   btn.initRGBBitmapButton(rgb_toggle_off,         240,    130,    112,    50, true);
    gfxButton btn_LED           =   btn.initRGBBitmapButton(rgb_toggle_on,          240,    190,    112,    50, true);
    gfxButton btn_Back          =   btn.initRGBBitmapButton(rgb_back_80,            380,    220,    80,     80, true);

    void initConfigButtons() {
        btn_array[0] = &btn_FlipScreen;
        btn_array[1] = &btn_Piezo;
        btn_array[2] = &btn_Stepper;
        btn_array[3] = &btn_LED;
        btn_array[4] = &btn_Back;

        btn_Back.addMomentary(func_Back, 0);
        btn_FlipScreen.addToggle(func_FlipScreen, 0);
        btn_Piezo.addToggle(func_Piezo, 0);
        btn_Stepper.addToggle(func_Stepper, 0);
        btn_LED.addToggle(func_LED, 0);

        // btn_Stepper.setToggleActive(false); // init as active
        // btn_Piezo.setToggleActive(false); // init as inactive
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
        tft.print("Enable Piezo");
        tft.setCursor(5,160);
        tft.print("Shutdown Motors");
        tft.setCursor(5,220);
        tft.print("Enable LED");
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
    void func_Piezo(bool btnActive) {
        if (btnActive) {
            btn_Piezo.updateRGBBitmap(rgb_toggle_off);
            btn_Piezo.drawNewBitmap(rgb_toggle_off);
            piezo.enabled(false);
        }
        else if (!btnActive) {
            btn_Piezo.updateRGBBitmap(rgb_toggle_on);
            btn_Piezo.drawNewBitmap(rgb_toggle_on);
            piezo.enabled(true);
        }
    }


    // enable/disable stepper motor
    void func_Stepper(bool btnActive) {
        if (btnActive) {
            btn_Stepper.updateRGBBitmap(rgb_toggle_on);
            btn_Stepper.drawNewBitmap(rgb_toggle_on);
            stepper1.shutdown(true);
            stepper2.shutdown(true);

        }
        else if (!btnActive) {
            btn_Stepper.updateRGBBitmap(rgb_toggle_off);
            btn_Stepper.drawNewBitmap(rgb_toggle_off);
            stepper1.shutdown(false);
            stepper2.shutdown(false);
        }
    }


    // enable/disable stepper motor
    void func_LED(bool btnActive) {
        if (btnActive) {
            btn_LED.updateRGBBitmap(rgb_toggle_off);
            btn_LED.drawNewBitmap(rgb_toggle_off);
            led.enabled(false);

        }
        else if (!btnActive) {
            btn_LED.updateRGBBitmap(rgb_toggle_on);
            btn_LED.drawNewBitmap(rgb_toggle_on);
            led.enabled(true);
        }
    }
}
