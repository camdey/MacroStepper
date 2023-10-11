#include "StepperControl.h"
#include "UserInterface.h"
#include "JoystickControl.h"
#include "menu/UI-Video360.h"
#include "menu/UI-Global.h"
#include "Video360.h"

namespace video_screen {
    #define num_btns 6
    gfxButton *btn_array[num_btns];

    gfxButton btn_PlayPause     =     btn.initBitmapButton(play,                30,     100,    120,    120,    CUSTOM_GREEN,   true    );
    gfxButton btn_Speed         =     btn.initButton("RPM", "fillRoundRect",    160,    20, 160,    80, 15,     DARKGRAY,       true    );
    gfxButton btn_Back          =     btn.initBitmapButton(backArrow,           200,    220,    80,     80,     WHITE,          true    );
    gfxButton btn_Direction     =     btn.initBitmapButton(dir_cw,              200,    120,    80,     80,     CUSTOM_GREEN,   true    );
    gfxButton btn_ArrowUp       =     btn.initBitmapButton(arrowUp,             350,    20,     120,    120,    CUSTOM_GREEN,   true    );
    gfxButton btn_ArrowDown     =     btn.initBitmapButton(arrowDown,           350,    180,    120,    120,    CUSTOM_RED,     true    );



    void initVideo360Buttons() {
        btn_array[0] = &btn_Speed;
        btn_array[1] = &btn_Back;
        btn_array[2] = &btn_PlayPause;
        btn_array[3] = &btn_ArrowUp;
        btn_array[4] = &btn_ArrowDown;
        btn_array[5] = &btn_Direction;

        btn_Speed.addToggle(func_Speed,             0 );
        btn_Direction.addToggle(func_Direction,     0 );
        btn_Back.addMomentary(func_Back,            0 );
        btn_PlayPause.addToggle(func_PlayPause,     0 );
        btn_ArrowUp.addMomentary(func_ArrowUp,      0 );
        btn_ArrowDown.addMomentary(func_ArrowDown,  0 );
    }


    void populateVideo360Screen() {
        ui.activeScreen(routines::ui_Video360);
        rStick.maxVelocity(5000); // lower joystick speed

        // draw buttons
        for (int i=0; i < num_btns; i++) {
            if (!btn_array[i]->isHidden()) { // if button is not hidden, draw it
                btn_array[i]->drawButton();
            }
        }

        // draw text
        btn_Speed.writeTextTopCentre(Arimo_Regular_30, WHITE);
        btn_Speed.writeTextBottomCentre(Arimo_Bold_30, WHITE, String(video360.revsPerMinute()/10.00, 1));
    }


    void checkVideo360Buttons(int touch_x, int touch_y) {
        for (int i=0; i < num_btns; i++) {
            if (btn_array[i]->isTactile()) {
                btn_array[i]->contains(touch_x, touch_y);
            }
        }
    }


    void func_Speed(bool btnActive) {
        if (btnActive ) {
            ui.canEdit(routines::btn_arrows, true);
            btn_Speed.writeTextTopCentre(Arimo_Regular_30, YELLOW);
            btn_Speed.writeTextBottomCentre(Arimo_Bold_30, YELLOW, String(video360.revsPerMinute()/10.00, 1));
        }
        else if (!btnActive) {
            ui.canEdit(routines::btn_arrows, false);
            btn_Speed.writeTextTopCentre(Arimo_Regular_30, WHITE);
            btn_Speed.writeTextBottomCentre(Arimo_Bold_30, WHITE, String(video360.revsPerMinute()/10.00, 1));
        }
    }


    void func_Direction(bool btnActive) {
        if (btnActive) {
            btn_Direction.drawButton(BLACK); // replace existing button
            btn_Direction.updateBitmap(dir_ccw); // update bitmap image
            btn_Direction.updateColour(CUSTOM_RED); // change colour
            btn_Direction.drawButton(); // draw
            stepper2.rotateClockwise(false);
        }
        else if (!btnActive) {
            btn_Direction.drawButton(BLACK); // replace existing button
            btn_Direction.updateBitmap(dir_cw); // update bitmap image
            btn_Direction.updateColour(CUSTOM_GREEN); // change colour
            btn_Direction.drawButton(); // draw
            stepper2.rotateClockwise(true);
        }
    }


    void func_Back(bool btnActive) {
        if (btnActive && !ui.canEdit(routines::btn_arrows)) {
            if (video360.busy()) {
                video360.status(routines::stopping); // pause movement
                func_PlayPause(false); // reset PlayPause button
                btn_PlayPause.setButtonActive(false); // reset button state
            }
            ui.populateScreen(routines::ui_Orbis);
        }
    }


    void func_PlayPause(bool btnActive) {
        // make sure VMAX is updated before starting
        video360.rpmToVmax();
        if (btnActive) {
            btn_PlayPause.drawButton(BLACK); // replace existing button
            btn_PlayPause.updateBitmap(pause); // update bitmap image
            btn_PlayPause.updateColour(CUSTOM_BLUE); // change colour
            btn_PlayPause.drawButton(); // draw
            video360.status(routines::running);
        }
        else if (!btnActive) {
            btn_PlayPause.drawButton(BLACK); // replace existing button
            btn_PlayPause.updateBitmap(play); // update bitmap image
            btn_PlayPause.updateColour(CUSTOM_GREEN); // change colour
            btn_PlayPause.drawButton(); // draw
            video360.status(routines::stopping);
        }
    }


    void func_ArrowUp(bool btnActive) {
        if (btnActive && ui.canEdit(routines::btn_arrows)) {
            video360.revsPerMinute(video360.revsPerMinute()+1);
            btn_Speed.writeTextBottomCentre(Arimo_Bold_30, YELLOW, String(video360.revsPerMinute()/10.00, 1));
            video360.rpmToVmax();
        }
    }


    void func_ArrowDown(bool btnActive) {
        if (btnActive && ui.canEdit(routines::btn_arrows)) {
            video360.revsPerMinute(video360.revsPerMinute()-1);
            btn_Speed.writeTextBottomCentre(Arimo_Bold_30, YELLOW, String(video360.revsPerMinute()/10.00, 1));
            video360.rpmToVmax();
        }
    }
}
