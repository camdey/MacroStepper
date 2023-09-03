#include "GlobalVariables.h"
#include "StepperControl.h"
#include "Photo360.h"
#include "UserInterface.h"
#include "menu/UI-Photo360.h"
#include "menu/UI-Photo360Config.h"
#include "menu/UI-Global.h"

namespace photoconfig_screen {
    #define num_btns 8
    gfxButton *btn_array[num_btns];


    gfxButton btn_90            =   btn.initButton("Move 90",   "fillRoundRect",    0,        20,     160,    80,     15,   DARKGRAY,   true);
    gfxButton btn_180           =   btn.initButton("Move 180",  "fillRoundRect",    0,        120,    160,    80,     15,   DARKGRAY,   true);
    gfxButton btn_360           =   btn.initButton("Move 360",  "fillRoundRect",    0,        220,    160,    80,     15,   DARKGRAY,   true);
    gfxButton btn_Direction     =   btn.initBitmapButton(dir_cw,    200,    20,     80,     80,     CUSTOM_GREEN,   true);
    gfxButton btn_Back          =   btn.initBitmapButton(backArrow, 200,    220,    80,     80,     WHITE,          true);
    gfxButton btn_ArrowUp       =   btn.initBitmapButton(arrowUp,   350,    20,     120,    120,    CUSTOM_GREEN,   true);
    gfxButton btn_ArrowDown     =   btn.initBitmapButton(arrowDown, 350,    180,    120,    120,    CUSTOM_RED,     true);


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
    }


    void populatePhoto360ConfigScreen() {
        ui.activeScreen(routines::ui_Photo360Config);
        setJoystickMaxVelocity(5000); // lower joystick speed
        // draw buttons
        for (int i=0; i < num_btns; i++) {
            if (!btn_array[i]->isHidden()) { // if button is not hidden, draw it
                btn_array[i]->drawButton();
            }
        }

        // draw text
        btn_90.writeTextCentre(Arimo_Regular_30, WHITE);
        btn_180.writeTextCentre(Arimo_Regular_30, WHITE);
        btn_360.writeTextCentre(Arimo_Regular_30, WHITE);
    }


    void checkPhoto360ConfigButtons(int touch_x, int touch_y) {
        for (int i=0; i < num_btns; i++) {
            if (btn_array[i]->isTactile()) {
                btn_array[i]->contains(touch_x, touch_y);
            }
        }
    }


    void func_90(bool btnActive) {
        if (btnActive) {
             // calculate number of microsteps to move for each press based on current nrPhotos setting
            int nrSteps = (ORBIS_MOTOR_STEPS*NR_MICROSTEPS) / 4;
            stepper2.readyStealthChop();
            stepper2.targetVelocity(360);
            if (!stepper2.rotateClockwise()) {
                nrSteps = nrSteps*-1;
            }
            stepper2.XTARGET(stepper2.XACTUAL()+nrSteps);
        }
    }


    void func_180(bool btnActive) {
        if (btnActive) {
             // calculate number of microsteps to move for each press based on current nrPhotos setting
            int nrSteps = (ORBIS_MOTOR_STEPS*NR_MICROSTEPS) / 2;
            stepper2.readyStealthChop();
            stepper2.targetVelocity(360);
            if (!stepper2.rotateClockwise()) {
                nrSteps = nrSteps*-1;
            }
            stepper2.XTARGET(stepper2.XACTUAL()+nrSteps);
        }
    }


    void func_360(bool btnActive) {
        if (btnActive) {
             // calculate number of microsteps to move for each press based on current nrPhotos setting
            int nrSteps = (ORBIS_MOTOR_STEPS*NR_MICROSTEPS);
            stepper2.readyStealthChop();
            stepper2.targetVelocity(360);
            if (!stepper2.rotateClockwise()) {
                nrSteps = nrSteps*-1;
            }
            stepper2.XTARGET(stepper2.XACTUAL()+nrSteps);
        }
    }


    void func_Back(bool btnActive) {
        if (btnActive) {
            ui.populateScreen(routines::ui_Photo360);
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


    void func_ArrowUp(bool btnActive) {
        if (btnActive) {
            // calculate number of microsteps to move for each press based on current nrPhotos setting
            int nrSteps = (ORBIS_MOTOR_STEPS*NR_MICROSTEPS) / photo360.requiredPhotos();
            stepper2.readyStealthChop();
            stepper2.targetVelocity(360);
            stepper2.XTARGET(stepper2.XACTUAL()+nrSteps);
        }
    }


    void func_ArrowDown(bool btnActive) {
        if (btnActive) {
            // calculate number of microsteps to move for each press based on current nrPhotos setting
            int nrSteps = (ORBIS_MOTOR_STEPS*NR_MICROSTEPS) / photo360.requiredPhotos();
            stepper2.readyStealthChop();
            stepper2.targetVelocity(360);
            stepper2.XTARGET(stepper2.XACTUAL()-nrSteps);
        }
    }
}
