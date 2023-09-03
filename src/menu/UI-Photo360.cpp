#include "GlobalVariables.h"
#include "StepperControl.h"
#include "Photo360.h"
#include "UserInterface.h"
#include "JoystickControl.h"
#include "menu/UI-Photo360.h"
#include "menu/UI-Global.h"

namespace photo_screen {
    #define num_btns 9
    gfxButton *btn_array[num_btns];

    gfxButton btn_PhotoNr       =   btn.initButton("Nr Photos",     "fillRoundRect",    0,  20,     160,    80,     15, DARKGRAY,   true);
    gfxButton btn_Delay         =   btn.initButton("Delay",         "fillRoundRect",    0,  120,    160,    80,     15, DARKGRAY,   true);
    gfxButton btn_Progress      =   btn.initButton("Progress",      "fillRoundRect",    0,  220,    160,    80,     15, DARKGRAY,   false);
    gfxButton btn_Config        =   btn.initBitmapButton(cogWheel,      200,    120,    80,     80,     WHITE,          true);
    gfxButton btn_Back          =   btn.initBitmapButton(backArrow,     200,    220,    80,     80,     WHITE,          true);
    gfxButton btn_PlayPause     =   btn.initBitmapButton(play,          350,    100,    120,    120,    CUSTOM_GREEN,   true);
    gfxButton btn_ArrowUp       =   btn.initBitmapButton(arrowUp,       350,    20,     120,    120,    CUSTOM_GREEN,   true);
    gfxButton btn_ArrowDown     =   btn.initBitmapButton(arrowDown,     350,    180,    120,    120,    CUSTOM_RED,     true);


    void initPhoto360Buttons() {
        btn_array[0] = &btn_PhotoNr;
        btn_array[1] = &btn_Delay;
        btn_array[2] = &btn_Progress;
        btn_array[3] = &global::btn_Flash;
        btn_array[4] = &btn_Config;
        btn_array[5] = &btn_Back;
        btn_array[6] = &btn_PlayPause;
        btn_array[7] = &btn_ArrowUp;
        btn_array[8] = &btn_ArrowDown;

        btn_PhotoNr.addToggle(func_PhotoNr,             0 );
        btn_Delay.addToggle(func_Delay,                 0 );
        btn_Config.addMomentary(func_Config,            0 );
        btn_Back.addMomentary(func_Back,                0 );
        btn_PlayPause.addToggle(func_PlayPause,         0 );
        btn_ArrowUp.addMomentary(func_ArrowUp,          0 );
        btn_ArrowDown.addMomentary(func_ArrowDown,      0 );

        // arrows are disabled by default, only enabled when editing step size
        btn_ArrowUp.hideButton(true);
        btn_ArrowDown.hideButton(true);

        btn_PhotoNr.addBorder(3, WHITE);
        btn_Delay.addBorder(3,    WHITE);
        btn_Progress.addBorder(3, WHITE);
    }


    void populatePhoto360Screen() {
        ui.activeScreen(routines::ui_Photo360);
        rStick.maxVelocity(5000); // lower joystick speed

        // draw buttons
        for (int i=0; i < num_btns; i++) {
            if (!btn_array[i]->isHidden()) { // if button is not hidden, draw it
                btn_array[i]->drawButton();
            }
        }

        // draw text
        btn_PhotoNr.writeTextTopCentre(Arimo_Regular_30, WHITE);
        btn_PhotoNr.writeTextBottomCentre(Arimo_Bold_30, WHITE, String(photo360.requiredPhotos()));
        btn_Delay.writeTextTopCentre(Arimo_Regular_30, WHITE);
        btn_Delay.writeTextBottomCentre(Arimo_Bold_30, WHITE, String(photo360.requiredPhotos()));
        btn_Progress.writeTextTopCentre(Arimo_Regular_30, WHITE);
        printPhoto360Progress();
    }


    void checkPhoto360Buttons(int touch_x, int touch_y) {
        for (int i=0; i < num_btns; i++) {
            if (btn_array[i]->isTactile()) {
                btn_array[i]->contains(touch_x, touch_y);
            }
        }
    }


    // set number of photos required for photo360 procedure
    void func_PhotoNr(bool btnActive) {
        // don't allow changing mid-procedure
        if (btnActive && photo360.status() == routines::inactive && !ui.canEdit(routines::btn_photo360Delay)) {
            ui.canEdit(routines::btn_arrows, true);
            ui.canEdit(routines::btn_photo360Nr, true);
            hideArrows(false); // show arrows, hide play/pause

            btn_PhotoNr.writeTextTopCentre(Arimo_Regular_30, YELLOW);
            btn_PhotoNr.writeTextBottomCentre(Arimo_Bold_30, YELLOW, String(photo360.requiredPhotos()));

            // clear play/pause button
            btn_PlayPause.drawButton(BLACK);
            btn_ArrowUp.drawButton(CUSTOM_GREEN);
            btn_ArrowDown.drawButton(CUSTOM_RED);
        }
        else if (!btnActive && photo360.status() == routines::inactive && !ui.canEdit(routines::btn_photo360Delay)) {
            ui.canEdit(routines::btn_arrows, false);
            ui.canEdit(routines::btn_photo360Delay, false);
            hideArrows(true); // hide arrows, show play/pause

            // TODO would be nice to not re-write the top line on every arrow press
            btn_PhotoNr.writeTextTopCentre(Arimo_Regular_30, WHITE);
            btn_PhotoNr.writeTextBottomCentre(Arimo_Bold_30, WHITE, String(photo360.requiredPhotos()));

            btn_ArrowUp.drawButton(BLACK);
            btn_ArrowDown.drawButton(BLACK);
            btn_PlayPause.drawButton();
        }
        else {
            // set back to off if conditions above not met
            btn_PhotoNr.setToggleActive(false);
        }
    }


    // set delay between photos for photo360 procedure
    void func_Delay(bool btnActive) {
        // don't allow changing unless paused or not started
        if (btnActive && !photo360.busy() && !ui.canEdit(routines::btn_photo360Nr)) {
            ui.canEdit(routines::btn_arrows, true);
            ui.canEdit(routines::btn_photo360Delay, true);
            hideArrows(false); // show arrows, hide play/pause

            btn_Delay.writeTextTopCentre(Arimo_Regular_30, YELLOW);
            btn_Delay.writeTextBottomCentre(Arimo_Bold_30, YELLOW, String(photo360.requiredPhotos()));

            // clear play/pause button
            btn_PlayPause.drawButton(BLACK);
            btn_ArrowUp.drawButton(CUSTOM_GREEN);
            btn_ArrowDown.drawButton(CUSTOM_RED);
        }
        else if (!btnActive && !photo360.busy() && !ui.canEdit(routines::btn_photo360Nr)) {
            ui.canEdit(routines::btn_arrows, false);
            ui.canEdit(routines::btn_photo360Delay, false);
            hideArrows(true); // hide arrows, show play/pause

            btn_Delay.writeTextTopCentre(Arimo_Regular_30, WHITE);
            btn_Delay.writeTextBottomCentre(Arimo_Bold_30, WHITE, String(photo360.requiredPhotos()));

            btn_ArrowUp.drawButton(BLACK);
            btn_ArrowDown.drawButton(BLACK);
            btn_PlayPause.drawButton();
        }
        else {
            // set back to off if conditions above not met
            btn_Delay.setToggleActive(false);
        }
    }


    void func_Config(bool btnActive) {
        if (btnActive && !ui.canEdit(routines::btn_arrows)) {
            ui.populateScreen(routines::ui_Photo360Config);
        }
    }


    void func_Back(bool btnActive) {
        if (btnActive && !ui.canEdit(routines::btn_arrows)) {
            ui.populateScreen(routines::ui_Orbis);
        }
    }


    void func_PlayPause(bool btnActive) {
        if (btnActive && !ui.canEdit(routines::btn_arrows)) {
            if (photo360.status() == routines::inactive) {
                photo360.completedPhotos(0);
                photo360.status(routines::start);
            } else {
                // else we must be resuming from a pause
                photo360.status(routines::waitShutter);
            }
            btn_PlayPause.drawButton(BLACK); // replace existing button
            btn_PlayPause.updateBitmap(pause); // update bitmap image
            btn_PlayPause.updateColour(CUSTOM_BLUE); // change colour
            btn_PlayPause.drawButton(); // draw
            photo360.run();
        }
        else if (!btnActive && !ui.canEdit(routines::btn_arrows)) {
            photo360.status(routines::paused);  // photo360 paused
            btn_PlayPause.drawButton(BLACK);    // replace existing button
            btn_PlayPause.updateBitmap(play);   // update bitmap image
            btn_PlayPause.updateColour(CUSTOM_GREEN); // change colour
            btn_PlayPause.drawButton();         // draw
        }
        else {
            btn_PlayPause.setToggleActive(!btnActive); // reset button state
        }
    }


    /***********************************************************************
    Used to pause photo360 when needing to be called from another file.
    ***********************************************************************/
    void pausePhoto360() {
        // photo360Paused = true; // pause photo360
        btn_PlayPause.drawButton(BLACK); // replace existing button
        btn_PlayPause.updateBitmap(play); // update bitmap image
        btn_PlayPause.updateColour(CUSTOM_GREEN); // update color
        btn_PlayPause.drawButton(); // draw button
        // btn_PlayPause.setToggleActive(false); // set button state to "pause" mode
    }


    /***********************************************************************
    Used to reset photo360 after completed 360.
    ***********************************************************************/
    void resetPhoto360() {
        photo360.completedPhotos(0);
        photo360.status(routines::inactive);
        // setTargetVelocity(STEALTH_CHOP_VMAX);
        btn_PlayPause.drawButton(BLACK); // replace existing button
        btn_PlayPause.updateBitmap(play); // update to show play button
        btn_PlayPause.updateColour(CUSTOM_GREEN); // update color
        btn_PlayPause.drawButton(); // draw button
        btn_PlayPause.setButtonActive(false);
    }


    void func_ArrowUp(bool btnActive) {
        if (btnActive && ui.canEdit(routines::btn_arrows)) {
            if (ui.canEdit(routines::btn_photo360Nr)) {
                photo360.incrementRequiredPhotos();
                photo360.completedPhotos(0); // reset in case adjusting after previous run
                printPhoto360Progress();
                btn_PhotoNr.writeTextBottomCentre(Arimo_Bold_30, YELLOW, String(photo360.requiredPhotos()));
            }
            else if (ui.canEdit(routines::btn_photo360Delay)) {
                photo360.incrementShutterDelay();
                btn_Delay.writeTextBottomCentre(Arimo_Bold_30, YELLOW, String(photo360.shutterDelay()));
            }
        }
    }


    void func_ArrowDown(bool btnActive) {
        if (btnActive && ui.canEdit(routines::btn_arrows)) {
            if (ui.canEdit(routines::btn_photo360Nr)) {
                photo360.decrementRequiredPhotos();
                photo360.completedPhotos(0); // reset in case adjusting after previous run
                printPhoto360Progress();
                btn_PhotoNr.writeTextBottomCentre(Arimo_Bold_30, YELLOW, String(photo360.requiredPhotos()));
            }
            else if (ui.canEdit(routines::btn_photo360Delay)) {
                photo360.decrementShutterDelay();
                btn_Delay.writeTextBottomCentre(Arimo_Bold_30, YELLOW, String(photo360.shutterDelay()));
            }
        }
    }


    /***********************************************************************
    Updates the photo360 screen with current progress of 360 sequence.
    Uses sprintf_P function to concatenate two values and format them as
    "completed / remaining" on the screen.
    ***********************************************************************/
    void printPhoto360Progress() {
        char photo360Progress[10]    = "0/0";

        // format progress in "Completed / Total" string
        sprintf_P(photo360Progress, PSTR("%02d/%02d"), photo360.completedPhotos(), photo360.requiredPhotos());
        // print to screen
        btn_Progress.writeTextBottomCentre(Arimo_Bold_30, WHITE, photo360Progress);
    }


    void hideArrows(bool hide) {
        btn_ArrowUp.hideButton(hide);
        btn_ArrowDown.hideButton(hide);
        btn_PlayPause.hideButton(!hide); // PlayPause takes opposite state to arrow buttons
    }
}
