#include "GlobalVariables.h"
#include "MiscFunctions.h"
#include "StepperControl.h"
#include "menu/UI-Main.h"
#include "menu/UI-AutoConfig.h"
#include "menu/UI-Global.h"

namespace autoconfig_screen {
    #define num_btns 9
    gfxButton *btn_array[num_btns];


    gfxButton btn_Start         =   btn.initButton("START", "fillRoundRect",    0,  20,     160,    80,     15,     CUSTOM_GREEN,   true);
    gfxButton btn_End           =   btn.initButton("END",   "fillRoundRect",    0,  120,    160,    80,     15,     CUSTOM_RED,     true);
    gfxButton btn_Run           =   btn.initButton("RUN",   "fillRoundRect",    0,  220,    160,    80,     15,     CUSTOM_BLUE,    true);
    gfxButton btn_Delay         =   btn.initBitmapButton(timer,     200,    20,     80,     80,     WHITE,          true);
    gfxButton btn_DelayVal      =   btn.initTransparentButton(      280,    20,     60,     30,                     false);
    gfxButton btn_Back          =   btn.initBitmapButton(backArrow, 200,    220,    80,     80,     WHITE,          true);
    gfxButton btn_ArrowUp       =   btn.initBitmapButton(arrowUp,   350,    20,     120,    120,    CUSTOM_GREEN,   true);
    gfxButton btn_ArrowDown     =   btn.initBitmapButton(arrowDown, 350,    180,    120,    120,    CUSTOM_RED,     true);


    void initAutoConfigButtons() {
        btn_array[0] = &btn_Start;
        btn_array[1] = &btn_End;
        btn_array[2] = &btn_Run;
        btn_array[3] = &btn_Delay;
        btn_array[4] = &btn_DelayVal;
        btn_array[5] = &btn_Back;
        btn_array[6] = &global::btn_Reset;
        btn_array[7] = &btn_ArrowUp;
        btn_array[8] = &btn_ArrowDown;

        btn_Start.addToggle(func_Start,             0 );
        btn_End.addToggle(func_End,                 0 );
        btn_Run.addToggle(func_Run,                 0 );
        btn_Delay.addToggle(func_Delay,             0 );
        btn_Back.addMomentary(func_Back,            0 );
        btn_ArrowUp.addMomentary(func_ArrowUp,      0 );
        btn_ArrowDown.addMomentary(func_ArrowDown,  0 );

        btn_Start.addBorder(3, WHITE);
        btn_End.addBorder(3, WHITE);
        btn_Run.addBorder(3, WHITE);
    }


    void populateAutoConfigScreen() {
        setCurrentScreen("AutoConfig");

        // draw buttons
        for (int i=0; i < num_btns; i++) {
            btn_array[i]->drawButton();
        }

        int currentPosition = stepper1.XACTUAL();

        // draw text
        btn_Start.writeTextTopCentre(Arimo_Regular_30,  WHITE);
        btn_Start.writeTextBottomCentre(Arimo_Bold_30,  WHITE,  String(getStartPosition()));
        btn_End.writeTextTopCentre(Arimo_Regular_30,    WHITE);
        btn_End.writeTextBottomCentre(Arimo_Bold_30,    WHITE,  String(getEndPosition()));
        btn_Run.writeTextTopCentre(Arimo_Regular_30,    WHITE);
        btn_Run.writeTextBottomCentre(Arimo_Bold_30,    WHITE,  String(currentPosition));
        btn_DelayVal.writeTextCentre(Arimo_Bold_30,     WHITE,  getShutterDelaySeconds());
    }


    void checkAutoConfigButtons(int touch_x, int touch_y) {
        for (int i=0; i < num_btns; i++) {
            if (btn_array[i]->isTactile()) {
                btn_array[i]->contains(touch_x, touch_y);
            }
        }
    }


    void func_Start(bool btnActive) {
        if (btnActive && !canEditShutterDelay() && !canEditEndPosition()) {
            setArrowsEnabled(true);
            setEditStartPosition(true);
            setStartPosition(stepper1.XACTUAL()); // set start position to current position

            btn_Start.writeTextTopCentre(Arimo_Regular_30, YELLOW);
            btn_Start.writeTextBottomCentre(Arimo_Bold_30, YELLOW, String(getStartPosition()));
        }
        else if (!btnActive && canEditStartPosition()) {
            setArrowsEnabled(false);
            setEditStartPosition(false);

            btn_Start.writeTextTopCentre(Arimo_Regular_30, WHITE);
            btn_Start.writeTextBottomCentre(Arimo_Bold_30, WHITE, String(getStartPosition()));
        }
    }


    void func_End(bool btnActive) {
        if (btnActive && !canEditShutterDelay() && !canEditStartPosition()) {
            setArrowsEnabled(true);
            setEditEndPosition(true);
            setEndPosition(stepper1.XACTUAL()); // set end position to current position

            btn_End.writeTextTopCentre(Arimo_Regular_30, YELLOW);
            btn_End.writeTextBottomCentre(Arimo_Bold_30, YELLOW, String(getEndPosition()));
        }
        else if (!btnActive && canEditEndPosition()) {
            setArrowsEnabled(false);
            setEditEndPosition(false);

            btn_End.writeTextTopCentre(Arimo_Regular_30, WHITE);
            btn_End.writeTextBottomCentre(Arimo_Bold_30, WHITE, String(getEndPosition()));
        }
    }


    void func_Run(bool btnActive) {
        if (btnActive && (canEditShutterDelay() + canEditStartPosition() + canEditEndPosition()) == 0) {
            int currentPosition = stepper1.XACTUAL();

            btn_Run.writeTextTopCentre(Arimo_Regular_30, YELLOW);
            btn_Run.writeTextBottomCentre(Arimo_Bold_30, YELLOW, String(currentPosition));

            dryRun(stepper1);

            btn_Run.writeTextTopCentre(Arimo_Regular_30, WHITE);
            btn_Run.writeTextBottomCentre(Arimo_Bold_30, WHITE, String(currentPosition));
            btn_Run.setToggleActive(false); // reset button state to false
        }
    }


    void func_Delay(bool btnActive) {
        if (btnActive) {
            setArrowsEnabled(true);
            setEditShutterDelay(true);

            btn_Delay.drawButton(YELLOW);
            btn_DelayVal.writeTextCentre(Arimo_Bold_30, YELLOW, getShutterDelaySeconds());
        }
        else if (!btnActive && canEditShutterDelay()) {
            setArrowsEnabled(false);
            setEditShutterDelay(false);

            btn_Delay.drawButton(WHITE);
            btn_DelayVal.writeTextCentre(Arimo_Bold_30, WHITE, getShutterDelaySeconds());
        }
    }


    // called when back arrow is pressed
    void func_Back(bool btnActive) {
        if (btnActive && !areArrowsEnabled()) {
            populateScreen("Auto");
        }
    }


    // called when up arrow is pressed, either updates start/end or shutterDelay
    void func_ArrowUp(bool btnActive) {
        if (btnActive && areArrowsEnabled()) {
            // edit start postion
            if (canEditStartPosition()) {
                executeMovement(stepper1, 1, 150); // forward
                updateStartPosition(); //set start but not end position
                printPosition(); // update rail positon
            }
            // edit end postion
            else if (canEditEndPosition()) {
                executeMovement(stepper1, 1, 150); // forward
                updateEndPosition(); //set end but not start position
                printPosition(); // update rail positon
            }
            // edit shutter delay
            else if (canEditShutterDelay()) {
                incrementShutterDelay();
                printShutterDelay();
            }
        }
    }


    // called when down arrow is pressed, either updates start/end or shutterDelay
    void func_ArrowDown(bool btnActive) {
        if (btnActive && areArrowsEnabled()) {
            // edit start postion
            if (canEditStartPosition()) {
                executeMovement(stepper1, -1, 150); // backwards
                updateStartPosition(); //set start position
                printPosition(); // update rail positon
            }
            // edit end postion
            else if (canEditEndPosition()) {
                executeMovement(stepper1, -1, 150); // forward
                updateEndPosition(); //set end position
                printPosition(); // update rail positon
            }
            // edit shutter delay
            else if (canEditShutterDelay()) {
                decrementShutterDelay();
                printShutterDelay();
            }
        }
    }


    // sets and prints new Start position for AutoStack
    void updateStartPosition() {
        // lower limit
        if (getStartPosition() < 0) {
            setStartPosition(0);
        }
        // get new value
        setStartPosition(stepper1.XACTUAL());
        btn_Start.writeTextTopCentre(Arimo_Regular_30, YELLOW);
        btn_Start.writeTextBottomCentre(Arimo_Bold_30, YELLOW, String(getStartPosition()));
        // reset AutoStack
        isNewAutoStack = true;
    }


    // sets and prints new End position for AutoStack
    void updateEndPosition() {
        // set new end value
        if (autoStackMax) {
            setEndPosition(MAX_RAIL_POSITION);
        }
        else if (!autoStackMax) {
            setEndPosition(stepper1.XACTUAL());
        }
        btn_End.writeTextTopCentre(Arimo_Regular_30, YELLOW);
        btn_End.writeTextBottomCentre(Arimo_Bold_30, YELLOW, String(getEndPosition()));
    }


    // print the new shutterDelay value to screen
    void printShutterDelay() {
        btn_DelayVal.writeTextCentre(Arimo_Bold_30, YELLOW, getShutterDelaySeconds());
    }


    // print current position of stepper under RUN text
    void printPosition() {
        btn_Run.writeTextTopCentre(Arimo_Regular_30, WHITE);
        btn_Run.writeTextBottomCentre(Arimo_Bold_30, WHITE, String(stepper1.XACTUAL()));
    }
}
