#include "AutoStack.h"
#include "CameraControl.h"
#include "Photo360.h"
#include "StatusEnums.h"
#include "menu/UI-Global.h"
#include "UserInterface.h"
#include "menu/UI-Auto.h"
#include "menu/UI-Photo360.h"

namespace global {
    gfxButton btn_Flash = btn.initBitmapButton(flashOff,    200,    20,     80,     80,     CUSTOM_RED, BLACK,  true);
    gfxButton btn_Reset = btn.initBitmapButton(cancel,      200,    120,    80,     80,     BLACK,      BLACK,  true);
    gfxButton btn_Home      =   btn.initRGBBitmapButton(rgb_home_70,        5,  5,      70,     70,     true);
    gfxButton btn_Stack     =   btn.initRGBBitmapButton(rgb_autostack_70,   5,  85,     70,     70,     true);
    gfxButton btn_360       =   btn.initRGBBitmapButton(rgb_360_70,         5,  165,    70,     70,     true);
    gfxButton btn_3D        =   btn.initRGBBitmapButton(rgb_3D_70,          5,  245,    70,     70,     true);


    void initGlobalButtons() {
        btn_Flash.addToggle(func_Flash, 0);
        btn_Reset.addMomentary(func_Reset, 0);
        btn_Home.addMomentary(func_Home, 0);
        btn_Stack.addMomentary(func_Stack, 0);
        btn_360.addMomentary(func_Orbis, 0);
        btn_3D.addMomentary(func_Orbis, 0);
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


    bool editingDisabled() {
        for (int i = routines::enum_first+1; i != routines::enum_last; i++) {
            routines::Buttons e = static_cast<routines::Buttons>(i);
            if (ui.canEdit(e)) {
                return false;
            } else {
                return true;
            }
        }
    }


    void func_Home(bool btnActive) {
        if (editingDisabled()) {
            ui.populateScreen(routines::ui_Home);
        }
    }


    void func_Stack(bool btnActive) {
        if (editingDisabled()) {
            ui.populateScreen(routines::ui_Stack);
        }
    }


    void func_Orbis(bool btnActive) {
        if (editingDisabled()) {
            ui.populateScreen(routines::ui_Orbis);
        }
    }

}