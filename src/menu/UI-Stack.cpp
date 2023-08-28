#include "GlobalVariables.h"
#include "MiscFunctions.h"
#include "StepperControl.h"
#include "menu/UI-Main.h"
#include "menu/UI-Stack.h"

namespace stack_screen {
    #define num_btns 5
    gfxButton *btn_array[num_btns];


    gfxButton btn_Manual    =   btn.initButton("Manual",    "fillRoundRect",    30,     120,    180,    80,     15,     CUSTOM_BLUE,    true);
    gfxButton btn_Auto      =   btn.initButton("Auto",      "fillRoundRect",    270,    120,    180,    80,     15,     CUSTOM_GREEN,   true);
    gfxButton btn_HomeRail  =   btn.initBitmapButton(home,          20,     230,    80,     80,     WHITE,  true);
    gfxButton btn_Back      =   btn.initBitmapButton(backArrow,     200,    230,    80,     80,     WHITE,  true);
    gfxButton btn_Target    =   btn.initBitmapButton(target,        380,    230,    80,     80,     WHITE,  true);


    void initStackButtons() {
        btn_array[0] = &btn_Manual;
        btn_array[1] = &btn_Auto;
        btn_array[2] = &btn_HomeRail;
        btn_array[3] = &btn_Target;
        btn_array[4] = &btn_Back;

        btn_Manual.addMomentary(func_Manual, 0);
        btn_Auto.addMomentary(func_Auto, 0);
        btn_HomeRail.addMomentary(func_HomeRail, 0);
        btn_Target.addMomentary(func_Target, 0);
        btn_Back.addMomentary(func_Back,        0 );

        btn_Manual.addBorder(4, WHITE);
        btn_Auto.addBorder(4, WHITE);
    }


    void populateStackScreen() {
        setCurrentScreen("Stack");
        // draw buttons
        for (int i=0; i < num_btns; i++) {
            btn_array[i]->drawButton();
        }
        // draw text
        btn_Auto.writeTextCentre(Lato_Black_34, WHITE);
        btn_Manual.writeTextCentre(Lato_Black_34, WHITE);
    }


    void checkStackButtons(int touch_x, int touch_y) {
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


    void func_HomeRail(bool btnActive) {
        if (btnActive) {
            btn_HomeRail.drawButton(CUSTOM_RED);
            runHomingSequence = true;
            homeRail(stepper1);
            btn_HomeRail.drawButton(WHITE);
        }
    }


    void func_Target(bool btnActive) {
        if (btnActive) {
            populateScreen("Target");
        }
    }


    void func_Back(bool btnActive) {
        if (btnActive && !areArrowsEnabled()) {
            populateScreen("Home");
        }
    }
}
