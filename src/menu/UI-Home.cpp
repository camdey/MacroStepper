#include "MiscFunctions.h"
#include "StepperControl.h"
#include "UserInterface.h"
#include "menu/UI-Home.h"

namespace home_screen {
    #define num_btns 6
    gfxButton *btn_array[num_btns];

    gfxButton btn_Stack     =   btn.initRGBBitmapButton(rgb_autostack_120,  20,     20,     120,     120,     true);
    gfxButton btn_360       =   btn.initRGBBitmapButton(rgb_360_120,        180,    20,     120,     120,     true);
    gfxButton btn_3D        =   btn.initRGBBitmapButton(rgb_3D_120,         340,    20,     120,     120,     true);
    gfxButton btn_Target    =   btn.initRGBBitmapButton(rgb_target_120,     20,     180,    120,     120,     true);
    gfxButton btn_Flash     =   btn.initRGBBitmapButton(rgb_flash_120,      180,    180,    120,     120,     true);
    gfxButton btn_Settings  =   btn.initRGBBitmapButton(rgb_settings_120,   340,    180,    120,     120,     true);


    void initHomeButtons() {
        btn_array[0] = &btn_Stack;
        btn_array[1] = &btn_360;
        btn_array[2] = &btn_3D;
        btn_array[3] = &btn_Target;
        btn_array[4] = &btn_Flash;
        btn_array[5] = &btn_Settings;

        btn_Stack.addMomentary(func_Stack, 0);
        btn_360.addMomentary(func_Orbis, 0);
        btn_3D.addMomentary(func_Orbis, 0);
        btn_Target.addMomentary(func_Target, 0);
        btn_Flash.addMomentary(func_Flash, 0);
        btn_Settings.addMomentary(func_Config, 0);
    }


    void populateHomeScreen() {
        ui.activeScreen(routines::ui_Home);
        // draw buttons
        for (int i=0; i < num_btns; i++) {
            btn_array[i]->drawButton();
        }
    }


    void checkHomeButtons(int touch_x, int touch_y) {
        for (int i=0; i < num_btns; i++) {
            if (btn_array[i]->isTactile() && !btn_array[i]->isHidden()) {
                btn_array[i]->contains(touch_x, touch_y);
            }
        }
    }


    void func_Stack(bool btnActive) {
        if (btnActive) {
            ui.populateScreen(routines::ui_Stack);
        }
    }


    void func_Orbis(bool btnActive) {
        if (btnActive) {
            ui.populateScreen(routines::ui_Orbis);
        }
    }


    void func_Target(bool btnActive) {
        if (btnActive) {
            ui.populateScreen(routines::ui_Target);
        }
    }


    void func_Flash(bool btnActive) {
        if (btnActive) {
            ui.populateScreen(routines::ui_Flash);
        }
    }


    void func_Config(bool btnActive) {
        if (btnActive) {
            ui.populateScreen(routines::ui_Config);
        }
    }
}
