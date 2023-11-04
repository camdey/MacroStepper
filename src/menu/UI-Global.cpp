#include "AutoStack.h"
#include "CameraControl.h"
#include "Photo360.h"
#include "StatusEnums.h"
#include "menu/UI-Global.h"
#include "UserInterface.h"
#include "menu/UI-Auto.h"
#include "menu/UI-Photo360.h"

namespace global {
    gfxButton btn_Flash     =   btn.initRGBBitmapButton(rgb_flash_on_80,        270,    20,     80,     80,     true);
    gfxButton btn_Reset     =   btn.initRGBBitmapButton(rgb_cancel_white_80,    200,    120,    80,     80,     true);
    gfxButton btn_Home      =   btn.initRGBBitmapButton(rgb_home_70,            5,      5,      70,     70,     true);
    gfxButton btn_Settings  =   btn.initRGBBitmapButton(rgb_settings_70,        5,      85,     70,     70,     true);
    gfxButton btn_FlashPage =   btn.initRGBBitmapButton(rgb_flash_70,           5,      165,    70,     70,     true);
    gfxButton btn_Target    =   btn.initRGBBitmapButton(rgb_target_70,          5,      245,    70,     70,     true);
    gfxButton btn_border    =   btn.initButton("",  "fillRect",                 80,     0,      3,      320,    0,      WHITE,      false);

    void initGlobalButtons() {
        btn_Flash.addToggle(func_Flash, 0);
        btn_Reset.addMomentary(func_Reset, 0);
        btn_Home.addMomentary(func_Home, 0);
        btn_Settings.addMomentary(func_Settings, 0);
        btn_FlashPage.addMomentary(func_FlashPage, 0);
        btn_Target.addMomentary(func_Target, 0);
    }


    void func_Flash(bool btnActive) {
        if (btnActive) {
            camera.shutterEnabled(false);
            btn_Flash.updateRGBBitmap(rgb_flash_off_80);
            btn_Flash.drawButton();
        }
        else if (!btnActive) {
            camera.shutterEnabled(true);
            btn_Flash.updateRGBBitmap(rgb_flash_on_80);
            btn_Flash.drawButton();
        }
    }

    // reset AutoStack procedure to default values
    void func_Reset(bool btnActive) {
        // if button pressed and autostack active...
        if (btnActive && stack.status() != routines::inactive) {
            stack.status(routines::inactive);
            stack.completedMovements(0);
            btn_Reset.hideButton();
            stack.status(routines::inactive);
        }
        // if button pressed and photo360 active...
        else if (btnActive && photo360.status() != routines::inactive) {
            photo360.status(routines::inactive);
            photo360.completedPhotos(0);
            btn_Reset.hideButton();
            stack.status(routines::inactive);
        }
        // if function called from elsewhere...
        else if (!btnActive && (stack.status() == routines::inactive || photo360.status() == routines::inactive)) {
            btn_Reset.drawNewBitmap(rgb_cancel_red_80);
        }
    }


    bool editingEnabled() {
        bool canEdit = false;
        for (int i = routines::enum_first+1; i != routines::enum_last; i++) {
            routines::Buttons e = static_cast<routines::Buttons>(i);
            if (ui.canEdit(e)) {
                canEdit = true;
                break;
            }
        }
        return canEdit;
    }


    void func_Home(bool btnActive) {
        if (!editingEnabled()) {
            ui.populateScreen(routines::ui_Home);
        }
    }


    void func_Settings(bool btnActive) {
        if (!editingEnabled()) {
            ui.populateScreen(routines::ui_Config);
        }
    }


    void func_FlashPage(bool btnActive) {
        if (!editingEnabled()) {
            ui.populateScreen(routines::ui_Flash);
        }
    }


    void func_Target(bool btnActive) {
        if (!editingEnabled()) {
            ui.populateScreen(routines::ui_Target);
        }
    }

}
