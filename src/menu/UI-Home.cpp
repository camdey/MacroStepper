#include "GlobalVariables.h"
#include "MiscFunctions.h"
#include "StepperControl.h"
#include "menu/UI-Main.h"
#include "menu/UI-Home.h"

namespace home_screen {
    #define num_btns 5
    gfxButton *btn_array[num_btns];


    gfxButton btn_Orbis     =   btn.initButton("Orbis", "fillRoundRect",    30,     120,    180,    80,     15, CUSTOM_BLUE,    true);
    gfxButton btn_Stack     =   btn.initButton("Stack", "fillRoundRect",    270,    120,    180,    80,     15, CUSTOM_GREEN,   true);
    gfxButton btn_Logo      =   btn.initBitmapButton(logo,                  50,     5,      380,    105,        CUSTOM_YELLOW,  true);
    gfxButton btn_Flash     =   btn.initBitmapButton(flashBulb,             260,    230,    80,     80,         WHITE,          true);
    gfxButton btn_Config    =   btn.initBitmapButton(cogWheel,              380,    230,    80,     80,         WHITE,          true);


    void initHomeButtons() {
        btn_array[0] = &btn_Orbis;
        btn_array[1] = &btn_Stack;
        btn_array[2] = &btn_Logo;
        btn_array[3] = &btn_Flash;
        btn_array[4] = &btn_Config;

        btn_Orbis.addMomentary(func_Orbis, 0);
        btn_Stack.addMomentary(func_Stack, 0);
        btn_Flash.addMomentary( func_Flash, 0);
        btn_Config.addMomentary(func_Config, 0);

        btn_Orbis.addBorder(4, WHITE);
        btn_Stack.addBorder(4, WHITE);
    }


    void populateHomeScreen() {
        setCurrentScreen("Home");
        // draw buttons
        for (int i=0; i < num_btns; i++) {
            btn_array[i]->drawButton();
        }
        // draw text
        btn_Stack.writeTextCentre(Lato_Black_34, WHITE);
        btn_Orbis.writeTextCentre(Lato_Black_34, WHITE);
    }


    void checkHomeButtons(int touch_x, int touch_y) {
        for (int i=0; i < num_btns; i++) {
            if (btn_array[i]->isTactile()) {
                btn_array[i]->contains(touch_x, touch_y);
            }
        }
    }


    void func_Stack(bool btnActive) {
        if (btnActive) {
            populateScreen("Stack");
        }
    }


    void func_Orbis(bool btnActive) {
        if (btnActive) {
            populateScreen("Orbis");
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
}
