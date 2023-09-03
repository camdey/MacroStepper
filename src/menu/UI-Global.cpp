#include "GlobalVariables.h"
#include "AutoStack.h"
#include "CameraControl.h"
#include "Photo360.h"
#include "StatusEnums.h"
#include "menu/UI-Global.h"
#include "UserInterface.h"
#include "menu/UI-Auto.h"
#include "menu/UI-Photo360.h"

namespace global {
    gfxButton btn_Flash = btn.initBitmapButton(flashOff,    200,    20,     80,     80,     CUSTOM_RED, true);
    gfxButton btn_Reset = btn.initBitmapButton(cancel,      200,    120,    80,     80,     BLACK,      true);


    void initGlobalButtons() {
        btn_Flash.addToggle(func_Flash, 0);
        btn_Reset.addMomentary(func_Reset, 0);
    }

    void func_Flash(bool btnActive) {
        if (btnActive) {
            camera.shutterEnabled(true);
            btn_Flash.updateBitmap(flashOn);
            btn_Flash.updateColour(CUSTOM_GREEN);
            btn_Flash.drawNewBitmap(flashOn, CUSTOM_GREEN);
            // auto_screen::stackStatus(routines::newStep);
        }
        else if (!btnActive) {
            camera.shutterEnabled(false);
            btn_Flash.updateBitmap(flashOff);
            btn_Flash.updateColour(CUSTOM_RED);
            btn_Flash.drawNewBitmap(flashOff, CUSTOM_RED);
            // auto_screen::stackStatus(routines::newStep);
        }
    }

    // reset AutoStack procedure to default values
    void func_Reset(bool btnActive) {
        // if button pressed and autostack active...
        if (btnActive && stack.status() != routines::inactive) {
            stack.status(routines::inactive);
            stack.completedMovements(0);
            btn_Reset.updateColour(BLACK);
            btn_Reset.drawButton(BLACK);
            stack.status(routines::inactive);
        }
        // if button pressed and photo360 active...
        else if (btnActive && photo360.status() != routines::inactive) {
            photo360.status(routines::inactive);
            photo360.completedPhotos(0);
            btn_Reset.updateColour(BLACK);
            btn_Reset.drawButton(BLACK);
            stack.status(routines::inactive);
        }
        // if function called from elsewhere...
        else if (!btnActive && (stack.status() == routines::inactive || photo360.status() == routines::inactive)) {
            btn_Reset.updateColour(CUSTOM_RED);
        }
    }
}