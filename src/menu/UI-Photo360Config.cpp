#include "StepperControl.h"
#include "Photo360.h"
#include "UserInterface.h"
#include "JoystickControl.h"
#include "menu/UI-Photo360.h"
#include "menu/UI-Photo360Config.h"
#include "menu/UI-Global.h"

namespace photoconfig_screen {
    #define num_btns 8
    gfxButton *btn_array[num_btns];


    gfxButton btn_90            =   btn.initButton("Move 90",   "fillRoundRect",    0,        20,     160,    80,     15,   DARKGRAY,   true);
    gfxButton btn_180           =   btn.initButton("Move 180",  "fillRoundRect",    0,        120,    160,    80,     15,   DARKGRAY,   true);
    gfxButton btn_360           =   btn.initButton("Move 360",  "fillRoundRect",    0,        220,    160,    80,     15,   DARKGRAY,   true);
    gfxButton btn_Direction     =   btn.initRGBBitmapButton(rgb_rotate_cw_80,   200,    20,     80,     80,     true);
    gfxButton btn_Back          =   btn.initRGBBitmapButton(rgb_back_80,        200,    220,    80,     80,     true);
    gfxButton btn_ArrowUp       =   btn.initRGBBitmapButton(rgb_arrow_up_100,   350,    35,     100,    90,     true);
    gfxButton btn_ArrowDown     =   btn.initRGBBitmapButton(rgb_arrow_down_100, 350,    195,    100,    90,     true);


    void initPhoto360ConfigButtons() {
        btn_array[0] = &btn_90;
        btn_array[1] = &btn_180;
        btn_array[2] = &btn_360;
        btn_array[3] = &btn_Back;
        btn_array[4] = &btn_ArrowUp;
        btn_array[5] = &btn_ArrowDown;
        btn_array[6] = &btn_Direction;
        btn_array[7] = &global::btn_Reset;

        btn_90.addMomentary(func_90,                0 );
        btn_180.addMomentary(func_180,              0 );
        btn_360.addMomentary(func_360,              0 );
        btn_Direction.addToggle(func_Direction,     0 );
        btn_Back.addMomentary(func_Back,            0 );
        btn_ArrowUp.addMomentary(func_ArrowUp,      0 );
        btn_ArrowDown.addMomentary(func_ArrowDown,  0 );

        btn_90.addBorder(3, WHITE);
        btn_180.addBorder(3, WHITE);
        btn_360.addBorder(3, WHITE);

        global::btn_Reset.hideButton();
    }


    void populatePhoto360ConfigScreen() {
        ui.activeScreen(routines::ui_Photo360Config);
        rStick.maxVelocity(5000); // lower joystick speed
        // draw buttons
        for (int i=0; i < num_btns; i++) {
            if (!btn_array[i]->isHidden()) { // if button is not hidden, draw it
                btn_array[i]->drawButton();
            }
        }

        // draw text
        btn_90.writeTextCentre(Roboto_Medium_30, WHITE);
        btn_180.writeTextCentre(Roboto_Medium_30, WHITE);
        btn_360.writeTextCentre(Roboto_Medium_30, WHITE);
    }


    void checkPhoto360ConfigButtons(int touch_x, int touch_y) {
        for (int i=0; i < num_btns; i++) {
            if (btn_array[i]->isTactile() && !btn_array[i]->isHidden()) {
                btn_array[i]->contains(touch_x, touch_y);
            }
        }
    }


    void func_90(bool btnActive) {
        if (btnActive) {
             // calculate number of microsteps to move for each press based on current nrPhotos setting
            int nrSteps = (ORBIS_MOTOR_STEPS*NR_MICROSTEPS) / 4;
            stepper2.slaveSelected(true);
            stepper2.enabled(true);
            photo360.executeMovement(photo360.getDirection(), 0, nrSteps);
            stepper2.slaveSelected(false);
            stepper2.enabled(false);
        }
    }


    void func_180(bool btnActive) {
        if (btnActive) {
             // calculate number of microsteps to move for each press based on current nrPhotos setting
            int nrSteps = (ORBIS_MOTOR_STEPS*NR_MICROSTEPS) / 2;
            stepper2.slaveSelected(true);
            stepper2.enabled(true);
            photo360.executeMovement(photo360.getDirection(), 0, nrSteps);
            stepper2.slaveSelected(false);
            stepper2.enabled(false);
        }
    }


    void func_360(bool btnActive) {
        if (btnActive) {
             // calculate number of microsteps to move for each press based on current nrPhotos setting
            int nrSteps = (ORBIS_MOTOR_STEPS*NR_MICROSTEPS);
            stepper2.slaveSelected(true);
            stepper2.enabled(true);
            photo360.executeMovement(photo360.getDirection(), 0, nrSteps);
            stepper2.slaveSelected(false);
            stepper2.enabled(false);
        }
    }


    void func_Back(bool btnActive) {
        if (btnActive) {
            ui.populateScreen(routines::ui_Photo360);
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


    void func_ArrowUp(bool btnActive) {
        if (btnActive) {
            // calculate number of microsteps to move for each press based on current nrPhotos setting
            int nrSteps = (ORBIS_MOTOR_STEPS*NR_MICROSTEPS) / photo360.requiredPhotos();
            stepper2.slaveSelected(true);
            stepper2.enabled(true);
            photo360.executeMovement(photo360.getDirection(), 0, nrSteps);
            stepper2.slaveSelected(false);
            stepper2.enabled(false);
        }
    }


    void func_ArrowDown(bool btnActive) {
        if (btnActive) {
            // calculate number of microsteps to move for each press based on current nrPhotos setting
            int nrSteps = (ORBIS_MOTOR_STEPS*NR_MICROSTEPS) / photo360.requiredPhotos();
            stepper2.slaveSelected(true);
            stepper2.enabled(true);
            photo360.executeMovement(photo360.getDirection(), 0, nrSteps*-1);
            stepper2.slaveSelected(false);
            stepper2.enabled(false);
        }
    }
}
