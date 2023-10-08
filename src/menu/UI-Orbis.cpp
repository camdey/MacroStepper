#include "UserInterface.h"
#include "JoystickControl.h"
#include "menu/UI-Orbis.h"

namespace orbis_screen {
    #define num_btns 3
    gfxButton *btn_array[num_btns];

    gfxButton btn_Video     =   btn.initButton("Video", "fillRoundRect",    30,     120,    180,    80,     15,     CUSTOM_BLUE,    true);
    gfxButton btn_Photo     =   btn.initButton("Photo", "fillRoundRect",    270,    120,    180,    80,     15,     CUSTOM_GREEN,   true);
    gfxButton btn_Back      =   btn.initBitmapButton(backArrow,             200,    230,    80,     80,             WHITE,          true);


    void initOrbisButtons() {
        btn_array[0] = &btn_Video;
        btn_array[1] = &btn_Photo;
        btn_array[2] = &btn_Back;

        btn_Video.addMomentary(func_Video,         0 );
        btn_Photo.addMomentary(func_Photo,    0 );
        btn_Back.addMomentary(func_Back,        0 );

        btn_Video.addBorder(4, WHITE);
        btn_Photo.addBorder(4,    WHITE);
    }


    void populateOrbisScreen() {
        ui.activeScreen(routines::ui_Orbis);
        rStick.maxVelocity(5000); // lower joystick speed
        // draw buttons
        for (int i=0; i < num_btns; i++) {
            if (!btn_array[i]->isHidden()) { // if button is not hidden, draw it
                btn_array[i]->drawButton();
            }
        }

        btn_Video.writeTextCentre(Lato_Black_34, WHITE);
        btn_Photo.writeTextCentre(Lato_Black_34, WHITE);
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
            ui.populateScreen(routines::ui_Home);
            rStick.maxVelocity(100000); // reset back to original value
        }
    }
}
