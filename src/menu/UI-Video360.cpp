#include "StepperControl.h"
#include "UserInterface.h"
#include "JoystickControl.h"
#include "menu/UI-Video360.h"
#include "menu/UI-Global.h"
#include "Video360.h"

namespace video_screen {
    #define num_btns 11
    gfxButton *btn_array[num_btns];


    gfxButton btn_Speed         =   btn.initButton("RPM", "fillRoundRect",      180,    20,     120,    80,     15, WHITE,   true);
    gfxButton btn_Back          =   btn.initRGBBitmapButton(rgb_back_80,        200,    220,    80,     80,     true);
    gfxButton btn_Direction     =   btn.initRGBBitmapButton(rgb_rotate_cw_80,   200,    120,    80,     80,     true);
    gfxButton btn_PlayPause     =   btn.initRGBBitmapButton(rgb_play_100,       350,    100,    100,    100,    true);
    gfxButton btn_ArrowUp       =   btn.initRGBBitmapButton(rgb_arrow_up_100,   350,    35,     100,    90,     true);
    gfxButton btn_ArrowDown     =   btn.initRGBBitmapButton(rgb_arrow_down_100, 350,    195,    100,    90,     true);



    void initVideo360Buttons() {
        btn_array[0] = &global::btn_Home;
        btn_array[1] = &global::btn_Settings;
        btn_array[2] = &global::btn_Target;
        btn_array[3] = &global::btn_FlashPage;
        btn_array[4] = &global::btn_border;
        btn_array[5] = &btn_Speed;
        btn_array[6] = &btn_Back;
        btn_array[7] = &btn_PlayPause;
        btn_array[8] = &btn_ArrowUp;
        btn_array[9] = &btn_ArrowDown;
        btn_array[10] = &btn_Direction;

        btn_Speed.addToggle(func_Speed,             0 );
        btn_Direction.addToggle(func_Direction,     0 );
        btn_Back.addMomentary(func_Back,            0 );
        btn_PlayPause.addToggle(func_PlayPause,     0 );
        btn_ArrowUp.addMomentary(func_ArrowUp,      0 );
        btn_ArrowDown.addMomentary(func_ArrowDown,  0 );

        // arrows are disabled by default, only enabled when editing step size
        btn_ArrowUp.hideButton();
        btn_ArrowDown.hideButton();
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
        btn_Speed.writeTextTopCentre(Roboto_Medium_30, BLACK);
        btn_Speed.writeTextBottomCentre(Roboto_Black_30, BLACK, String(video360.revsPerMinute()/10.00, 1));
    }


    void checkVideo360Buttons(int touch_x, int touch_y) {
        for (int i=0; i < num_btns; i++) {
            if (btn_array[i]->isTactile() && !btn_array[i]->isHidden()) {
                btn_array[i]->contains(touch_x, touch_y);
            }
        }
    }


    void func_Speed(bool btnActive) {
        if (btnActive ) {
            ui.canEdit(routines::btn_arrows, true);
            hideArrows(false); // show arrows, hide play/pause
            btn_Speed.writeTextTopCentre(Roboto_Medium_30, CUSTOM_BLUE_DARK);
            btn_Speed.writeTextBottomCentre(Roboto_Black_30, CUSTOM_BLUE_DARK, String(video360.revsPerMinute()/10.00, 1));
        }
        else if (!btnActive) {
            ui.canEdit(routines::btn_arrows, false);
            hideArrows(true); // show arrows, hide play/pause
            btn_Speed.writeTextTopCentre(Roboto_Medium_30, BLACK);
            btn_Speed.writeTextBottomCentre(Roboto_Black_30, BLACK, String(video360.revsPerMinute()/10.00, 1));
        }
    }


    void func_Direction(bool btnActive) {
        if (btnActive) {
            btn_Direction.hideButton(); // replace existing button
            btn_Direction.updateRGBBitmap(rgb_rotate_ccw_80); // update bitmap image
            btn_Direction.drawButton(); // draw
            stepper2.rotateClockwise(false);
        }
        else if (!btnActive) {
            btn_Direction.hideButton(); // replace existing button
            btn_Direction.updateRGBBitmap(rgb_rotate_cw_80); // update bitmap image
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
            // ui.populateScreen(ui.previousScreen());
            ui.populateScreen(routines::ui_Orbis);
        }
    }


    void func_PlayPause(bool btnActive) {
        // make sure VMAX is updated before starting
        video360.rpmToVmax();
        if (btnActive) {
            btn_PlayPause.hideButton(); // replace existing button
            btn_PlayPause.updateRGBBitmap(rgb_pause_100); // update bitmap image
            btn_PlayPause.drawButton(); // draw
            video360.status(routines::running);
        }
        else if (!btnActive) {
            btn_PlayPause.hideButton(); // replace existing button
            btn_PlayPause.updateRGBBitmap(rgb_play_100); // update bitmap image
            btn_PlayPause.drawButton(); // draw
            video360.status(routines::stopping);
        }
    }


    void func_ArrowUp(bool btnActive) {
        if (btnActive && ui.canEdit(routines::btn_arrows)) {
            video360.revsPerMinute(video360.revsPerMinute()+1);
            btn_Speed.writeTextBottomCentre(Roboto_Black_30, CUSTOM_BLUE_DARK, String(video360.revsPerMinute()/10.00, 1));
            video360.rpmToVmax();
        }
    }


    void func_ArrowDown(bool btnActive) {
        if (btnActive && ui.canEdit(routines::btn_arrows)) {
            video360.revsPerMinute(video360.revsPerMinute()-1);
            btn_Speed.writeTextBottomCentre(Roboto_Black_30, CUSTOM_BLUE_DARK, String(video360.revsPerMinute()/10.00, 1));
            video360.rpmToVmax();
        }
    }


    void hideArrows(bool hide) {
        if (hide) {
            btn_ArrowUp.hideButton();
            btn_ArrowDown.hideButton();
            btn_PlayPause.drawButton(); // PlayPause takes opposite state to arrow buttons
        } else {
            btn_PlayPause.hideButton(); // PlayPause takes opposite state to arrow buttons
            btn_ArrowUp.drawButton();
            btn_ArrowDown.drawButton();
        }
    }
}
