#include "UserInterface.h"
#include "JoystickControl.h"
#include "menu/UI-Orbis.h"
#include "menu/UI-Global.h"

namespace orbis_screen {
    #define num_btns 6
    gfxButton *btn_array[num_btns];

    gfxButton btn_Video     =   btn.initButton("Video",    "fillRoundRect",    150,     35,    220,    110,     5,     WHITE,  true);
    gfxButton btn_Photo     =   btn.initButton("Photo",      "fillRoundRect",    150,    175,    220,    110,     5,     WHITE,  true);


    void initOrbisButtons() {
        btn_array[0] = &global::btn_Home;
        btn_array[1] = &global::btn_Settings;
        btn_array[2] = &global::btn_Target;
        btn_array[3] = &global::btn_FlashPage;
        btn_array[4] = &btn_Video;
        btn_array[5] = &btn_Photo;

        btn_Video.addMomentary(func_Video,  0 );
        btn_Photo.addMomentary(func_Photo,  0 );

        btn_Video.addBorder(5, CUSTOM_GREY);
        btn_Photo.addBorder(5, CUSTOM_GREY);
    }


    void populateOrbisScreen() {
        ui.activeScreen(routines::ui_Orbis);
        tft.fillRect(80, 0, 5, 320, WHITE);
        rStick.maxVelocity(5000); // lower joystick speed
        // draw buttons
        for (int i=0; i < num_btns; i++) {
            if (!btn_array[i]->isHidden()) { // if button is not hidden, draw it
                btn_array[i]->drawButton();
            }
        }

        btn_Video.writeTextCentre(Lato_Black_40, DARKGRAY);
        btn_Photo.writeTextCentre(Lato_Black_40, DARKGRAY);
    }


    void checkOrbisButtons(int touch_x, int touch_y) {
        for (int i=0; i < num_btns; i++) {
            if (btn_array[i]->isTactile()) {
                btn_array[i]->contains(touch_x, touch_y);
            }
        }
    }


    void func_Video(bool btnActive) {
        if (btnActive) {
            ui.populateScreen(routines::ui_Video360);
        }
    }


    void func_Photo(bool btnActive) {
        if (btnActive) {
            ui.populateScreen(routines::ui_Photo360);
        }
    }


    void func_Back(bool btnActive) {
        if (btnActive && !ui.canEdit(routines::btn_arrows)) {
            ui.populateScreen(ui.previousScreen());
            rStick.maxVelocity(100000); // reset back to original value
        }
    }
}
