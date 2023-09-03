#include "StatusEnums.h"
#include "UserInterface.h"
#include "menu/UI-Global.h"
#include "menu/UI-Home.h"
#include "menu/UI-Stack.h"
#include "menu/UI-Orbis.h"
#include "menu/UI-Video360.h"
#include "menu/UI-Photo360.h"
#include "menu/UI-Photo360Config.h"
#include "menu/UI-Target.h"
#include "menu/UI-Flash.h"
#include "menu/UI-Config.h"
#include "menu/UI-Manual.h"
#include "menu/UI-Auto.h"
#include "menu/UI-AutoConfig.h"

using namespace global;
using namespace manual_screen;
using namespace home_screen;
using namespace stack_screen;
using namespace orbis_screen;
using namespace video_screen;
using namespace photo_screen;
using namespace photoconfig_screen;
using namespace target_screen;
using namespace flash_screen;
using namespace config_screen;
using namespace auto_screen;
using namespace autoconfig_screen;


void UserInterface::initButtons(unsigned long toggleDebounce, unsigned long momentaryDebounce) {
    btn.setBackgroundColour(BLACK);
    btn.setToggleDelay(toggleDebounce);
    btn.setMomentaryDelay(momentaryDebounce);

    initGlobalButtons();
    initHomeButtons();
    initStackButtons();
    initOrbisButtons();
    initVideo360Buttons();
    initPhoto360Buttons();
    initPhoto360ConfigButtons();
    initTargetButtons();
    initFlashButtons();
    initConfigButtons();
    initManualButtons();
    initAutoButtons();
    initAutoConfigButtons();
}


void UserInterface::populateScreen(routines::Screens screen) {
    tft.fillScreen(BLACK);
    delay(20); // see if this solves the superimposition of two different screens
    tft.fillScreen(BLACK);
    if (screen == routines::ui_Home) {
        populateHomeScreen();
    }
    if (screen == routines::ui_Stack) {
        populateStackScreen();
    }
    if (screen == routines::ui_Orbis) {
        populateOrbisScreen();
    }
    if (screen == routines::ui_Video360) {
        populateVideo360Screen();
    }
    if (screen == routines::ui_Photo360) {
        populatePhoto360Screen();
    }
    if (screen == routines::ui_Photo360Config) {
        populatePhoto360ConfigScreen();
    }
    if (screen == routines::ui_Target) {
        populateTargetScreen();
    }
    else if (screen == routines::ui_Flash) {
        populateFlashScreen();
    }
    else if (screen == routines::ui_Config) {
        populateConfigScreen();
    }
    else if (screen == routines::ui_Manual) {
        populateManualScreen();
    }
    else if (screen == routines::ui_Auto) {
        populateAutoScreen();
    }
    else if (screen == routines::ui_AutoConfig) {
        populateAutoConfigScreen();
    }
}


void UserInterface::readTouchScreen(routines::Screens screen) {
    TSPoint point = ts.getPoint();

    // reset pinModes for tft (otherwise can't draw!)
    pinMode(XM, OUTPUT);
    pinMode(YP, OUTPUT);

    int touch_x;
    int touch_y;
    int touch_z = point.z;

    if (!ui.screenRotated()) {
        touch_x = map(point.y, TS_MINY, TS_MAXY, 0, tft.width());        // rotate & scale to TFT boundaries
        touch_y = map(point.x, TS_MINX, TS_MAXX, 0, tft.height());     //     ... USB port at upper left
    }
    else if (ui.screenRotated()) {
        touch_x = map(point.y, TS_MINY, TS_MAXY, tft.width(), 0);        // rotate & scale to TFT boundaries
        touch_y = map(point.x, TS_MINX, TS_MAXX, tft.height(), 0);     //     ... USB port at lower right
    }
    
    if (touch_z >= 50 && touch_z <= 1000) {
        if ((touch_x > 0 && touch_x <= tft.width()) && (touch_y > 0 && touch_y <= tft.height())) {
            if (screen == routines::ui_Home) {
                checkHomeButtons(touch_x, touch_y);
            }
            else if (screen == routines::ui_Stack) {
                checkStackButtons(touch_x, touch_y);
            }
            else if (screen == routines::ui_Orbis) {
                checkOrbisButtons(touch_x, touch_y);
            }
            else if (screen == routines::ui_Video360) {
                checkVideo360Buttons(touch_x, touch_y);
            }
            else if (screen == routines::ui_Photo360) {
                checkPhoto360Buttons(touch_x, touch_y);
            }
            else if (screen == routines::ui_Photo360Config) {
                checkPhoto360ConfigButtons(touch_x, touch_y);
            }
            else if (screen == routines::ui_Target) {
                checkTargetButtons(touch_x, touch_y);
            }
            else if (screen == routines::ui_Flash) {
                checkFlashButtons(touch_x, touch_y);
            }
            else if (screen == routines::ui_Config) {
                checkConfigButtons(touch_x, touch_y);
            }
            else if (screen == routines::ui_Manual) {
                checkManualButtons(touch_x, touch_y);
            }
            else if (screen == routines::ui_Auto) {
                checkAutoButtons(touch_x, touch_y);
            }
            else if (screen == routines::ui_AutoConfig) {
                checkAutoConfigButtons(touch_x, touch_y);
            }
        }
    }
    else if (touch_z == 0 && btn.isToggleActive()) {
        // if toggle active, reset flag to false when
        // no touch is recorded
        btn.setToggleActive(false);
    }
}
