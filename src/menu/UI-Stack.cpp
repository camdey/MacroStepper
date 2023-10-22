#include "MiscFunctions.h"
#include "StepperControl.h"
#include "UserInterface.h"
#include "menu/UI-Stack.h"
#include "menu/UI-Global.h"

namespace stack_screen {
    #define num_btns 6
    gfxButton *btn_array[num_btns];

    gfxButton btn_Manual    =   btn.initButton("Manual",    "fillRoundRect",    150,     35,    220,    110,     4,     WHITE,  true);
    gfxButton btn_Auto      =   btn.initButton("Auto",      "fillRoundRect",    150,    175,    220,    110,     4,     WHITE,  true);
    // gfxButton btn_HomeRail  =   btn.initBitmapButton(home,          200,    230,    80,     80,     WHITE,  BLACK,  true);


    void initStackButtons() {
        btn_array[0] = &global::btn_Home;
        btn_array[1] = &global::btn_Settings;
        btn_array[2] = &global::btn_Target;
        btn_array[3] = &global::btn_FlashPage;
        btn_array[4] = &btn_Manual;
        btn_array[5] = &btn_Auto;
        // btn_array[6] = &btn_HomeRail;

        btn_Manual.addMomentary(func_Manual, 0);
        btn_Auto.addMomentary(func_Auto, 0);
        // btn_HomeRail.addMomentary(func_HomeRail, 0);

        btn_Manual.addBorder(5, CUSTOM_GREY);
        btn_Auto.addBorder(5, CUSTOM_GREY);
    }


    void populateStackScreen() {
        ui.activeScreen(routines::ui_Stack);
        tft.fillRect(80, 0, 5, 320, WHITE);
        // draw buttons
        for (int i=0; i < num_btns; i++) {
            btn_array[i]->drawButton();
        }
        // draw text
        btn_Manual.writeTextCentre(Lato_Black_40, DARKGRAY);
        btn_Auto.writeTextCentre(Lato_Black_40, DARKGRAY);
    }


    void checkStackButtons(int touch_x, int touch_y) {
        for (int i=0; i < num_btns; i++) {
            if (btn_array[i]->isTactile() && !btn_array[i]->isHidden()) {
                btn_array[i]->contains(touch_x, touch_y);
            }
        }
    }


    void func_Auto(bool btnActive) {
        if (btnActive) {
            ui.populateScreen(routines::ui_Auto);
        }
    }


    void func_Manual(bool btnActive) {
        if (btnActive) {
            ui.populateScreen(routines::ui_Manual);
        }
    }


    void func_HomeRail(bool btnActive) {
        // if (btnActive) {
        //     btn_HomeRail.drawButton(CUSTOM_RED);
        //     stepper1.homeRail();
        //     btn_HomeRail.drawButton(WHITE);
        // }
    }
}
