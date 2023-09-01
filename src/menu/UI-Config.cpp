#include "GlobalVariables.h"
#include "MiscFunctions.h"
#include "CameraControl.h"
#include "menu/UI-Main.h"
#include "menu/UI-Config.h"
#include "menu/UI-Auto.h"

namespace config_screen {
    #define num_btns 5
    gfxButton *btn_array[num_btns];

    gfxButton btn_FlipScreen    =   btn.initBitmapButton(toggleOff,     240,    10,     100,    50,     CUSTOM_RED,     true);
    gfxButton btn_FlashSensor   =   btn.initBitmapButton(toggleOn,      240,    70,     100,    50,     CUSTOM_RED,     true);
    gfxButton btn_Stepper       =   btn.initBitmapButton(toggleOn,      240,    130,    100,    50,     CUSTOM_GREEN,   true);
    gfxButton btn_Thing2        =   btn.initBitmapButton(toggleOff,     240,    190,    100,    50,     CUSTOM_RED,     true);
    gfxButton btn_Back          =   btn.initBitmapButton(backArrow,     380,    220,    80,     80,     WHITE,          true);

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

        // btn_Stepper.setToggleActive(true); // init as active
        // btn_FlashSensor.setToggleActive(true); // init as active
    }


    void populateConfigScreen() {
        setCurrentScreen("Config");
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
            if (btn_array[i]->isTactile()) {
                btn_array[i]->contains(touch_x, touch_y);
            }
        }
    }


    void func_Back(bool btnActive) {
        if (btnActive && !areArrowsEnabled()) {
            populateScreen("Home");
        }
    }


    // invert screen
    void func_FlipScreen(bool btnActive) {
        if (btnActive) {
            btn_FlipScreen.updateBitmap(toggleOn);
            btn_FlipScreen.updateColour(CUSTOM_GREEN);
            rotateScreen(); // rotate screen, touch controls, and joystick
            populateScreen("Config");
        }
        else if (!btnActive) {
            btn_FlipScreen.updateBitmap(toggleOff);
            btn_FlipScreen.updateColour(CUSTOM_RED);
            rotateScreen(); // rotate screen, touch controls, and joystick
            populateScreen("Config");
        }
    }


    // enable/disable flash sensor reading
    void func_FlashSensor(bool btnActive) {
        if (btnActive) {
            btn_FlashSensor.drawButton(BLACK);
            btn_FlashSensor.updateBitmap(toggleOff);
            btn_FlashSensor.updateColour(CUSTOM_RED);
            btn_FlashSensor.drawButton();
            setFlashSensorEnabled(false);
            // auto_screen::stackStatus(newStep); // always reset in case switched mid-procedure
        }
        else if (!btnActive) {
            btn_FlashSensor.drawButton(BLACK);
            btn_FlashSensor.updateBitmap(toggleOn);
            btn_FlashSensor.updateColour(CUSTOM_GREEN);
            btn_FlashSensor.drawButton();
            setFlashSensorEnabled(true);
            // auto_screen::stackStatus(newStep); // always reset in case switched mid-procedure
        }
    }


    // enable/disable stepper motor
    void func_Stepper(bool btnActive) {
        if (btnActive) {
            btn_Stepper.drawButton(BLACK);
            btn_Stepper.updateBitmap(toggleOn);
            btn_Stepper.updateColour(CUSTOM_GREEN);
            btn_Stepper.drawButton();
            stepper1.enabled(true);
        }
        else if (!btnActive) {
            btn_Stepper.drawButton(BLACK);
            btn_Stepper.updateBitmap(toggleOff);
            btn_Stepper.updateColour(CUSTOM_RED);
            btn_Stepper.drawButton();
            stepper1.enabled(false);
        }
    }
}
