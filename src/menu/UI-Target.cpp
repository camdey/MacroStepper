#include "GlobalVariables.h"
#include "StepperControl.h"
#include "MiscFunctions.h"
#include "UserInterface.h"
#include "menu/UI-Target.h"

namespace target_screen {
    #define num_btns 18
    // #define totalRows 4
    // #define totalCols 4
 
    char targetValues[7] = {'0'};
    int targetValueIndex = 0;
    int maxTargetValueIndex = 5;
    bool canEditTarget = false;
    float targetMillimetres = 0.0000;


    gfxButton btn_Target    =   btn.initButton("Target st.",    "fillRoundRect", 0,     20,    160,    80,    15,   DARKGRAY, true);
    gfxButton btn_Distance  =   btn.initButton("Target mm",     "fillRoundRect", 0,    120,    160,    80,    15,   DARKGRAY, false);
    gfxButton btn_Actual    =   btn.initButton("Actual",        "fillRoundRect", 0,    220,    160,    80,    15,   DARKGRAY, false);
    gfxButton btn_Cancel    =   btn.initButton("X",    "fillRoundRect", 264,    238,    60, 60,    5,   CUSTOM_BLUE,    true);
    gfxButton btn_Minus     =   btn.initButton("-",    "fillRoundRect", 336,    238,    60, 60,    5,   CUSTOM_BLUE,    true);
    gfxButton btn_Zero      =   btn.initButton("0",    "fillRoundRect", 408,    238,    60, 60,    5,   CUSTOM_BLUE,    true);
    gfxButton btn_One       =   btn.initButton("1",    "fillRoundRect", 264,    166,    60, 60,    5,   CUSTOM_BLUE,    true);
    gfxButton btn_Two       =   btn.initButton("2",    "fillRoundRect", 336,    166,    60, 60,    5,   CUSTOM_BLUE,    true);
    gfxButton btn_Three     =   btn.initButton("3",    "fillRoundRect", 408,    166,    60, 60,    5,   CUSTOM_BLUE,    true);
    gfxButton btn_Four      =   btn.initButton("4",    "fillRoundRect", 264,    94,     60, 60,    5,   CUSTOM_BLUE,    true);
    gfxButton btn_Five      =   btn.initButton("5",    "fillRoundRect", 336,    94,     60, 60,    5,   CUSTOM_BLUE,    true);
    gfxButton btn_Six       =   btn.initButton("6",    "fillRoundRect", 408,    94,     60, 60,    5,   CUSTOM_BLUE,    true);
    gfxButton btn_Seven     =   btn.initButton("7",    "fillRoundRect", 264,    22,     60, 60,    5,   CUSTOM_BLUE,    true);
    gfxButton btn_Eight     =   btn.initButton("8",    "fillRoundRect", 336,    22,     60, 60,    5,   CUSTOM_BLUE,    true);
    gfxButton btn_Nine      =   btn.initButton("9",    "fillRoundRect", 408,    22,     60, 60,    5,   CUSTOM_BLUE,    true);
    gfxButton btn_Back      =   btn.initBitmapButton(backArrow,         172,    220,    80, 80,         WHITE,          true);
    gfxButton btn_Run       =   btn.initBitmapButton(rocket,            172,    20,     80, 80,         WHITE,          true);
    gfxButton btn_Save      =   btn.initBitmapButton(save,              172,    120,    80, 80,         WHITE,          true);


    gfxButton *btn_array[num_btns];


    void initTargetButtons() {
        btn_array[0] = &btn_Target;
        btn_array[1] = &btn_Seven;
        btn_array[2] = &btn_Eight;
        btn_array[3] = &btn_Nine;
        btn_array[4] = &btn_Distance;
        btn_array[5] = &btn_Four;
        btn_array[6] = &btn_Five;
        btn_array[7] = &btn_Six;
        btn_array[8] = &btn_Actual;
        btn_array[9] = &btn_One;
        btn_array[10] = &btn_Two;
        btn_array[11] = &btn_Three;
        btn_array[12] = &btn_Cancel;
        btn_array[13] = &btn_Minus;
        btn_array[14] = &btn_Zero;
        btn_array[15] = &btn_Back;
        btn_array[16] = &btn_Run;
        btn_array[17] = &btn_Save;

        btn_Target.addToggle(func_Distance, 0);
        btn_Cancel.addInputKey(func_Input, 0);
        btn_Minus.addInputKey(func_Input, 0);
        btn_Zero.addInputKey(func_Input, 0);
        btn_One.addInputKey(func_Input, 0);
        btn_Two.addInputKey(func_Input, 0);
        btn_Three.addInputKey(func_Input, 0);
        btn_Four.addInputKey(func_Input, 0);
        btn_Five.addInputKey(func_Input, 0);
        btn_Six.addInputKey(func_Input, 0);
        btn_Seven.addInputKey(func_Input, 0);
        btn_Eight.addInputKey(func_Input, 0);
        btn_Nine.addInputKey(func_Input, 0);
        btn_Back.addMomentary(func_Back, 0);
        btn_Run.addMomentary(func_Run, 0);
        btn_Save.addMomentary(func_saveStepDist, 0);

        btn_Target.addBorder(3, WHITE);
        btn_Distance.addBorder(3, WHITE);
        btn_Actual.addBorder(3, WHITE);
    }


    void populateTargetScreen() {
        ui.activeScreen(routines::ui_Target);

        // draw buttons
        for (int i=0; i < num_btns; i++) {
            btn_array[i]->drawButton();
        }

        // draw text
        btn_Target.writeTextTopCentre(Arimo_Regular_30, WHITE);
        btn_Target.writeTextBottomCentre(Arimo_Bold_30, WHITE, targetValues);
        btn_Distance.writeTextTopCentre(Arimo_Regular_30, WHITE);
        btn_Distance.writeTextBottomCentre(Arimo_Bold_30, WHITE, String(targetMillimetres, 4));
        btn_Actual.writeTextTopCentre(Arimo_Regular_30, WHITE);
        btn_Actual.writeTextBottomCentre(Arimo_Bold_30, WHITE, String(stepper1.XACTUAL()));
        btn_Cancel.writeTextCentre(Arimo_Bold_30, WHITE);
        btn_Minus.writeTextCentre(Arimo_Bold_30, WHITE);
        btn_Zero.writeTextCentre(Arimo_Bold_30, WHITE);
        btn_One.writeTextCentre(Arimo_Bold_30, WHITE);
        btn_Two.writeTextCentre(Arimo_Bold_30, WHITE);
        btn_Three.writeTextCentre(Arimo_Bold_30, WHITE);
        btn_Four.writeTextCentre(Arimo_Bold_30, WHITE);
        btn_Five.writeTextCentre(Arimo_Bold_30, WHITE);
        btn_Six.writeTextCentre(Arimo_Bold_30, WHITE);
        btn_Seven.writeTextCentre(Arimo_Bold_30, WHITE);
        btn_Eight.writeTextCentre(Arimo_Bold_30, WHITE);
        btn_Nine.writeTextCentre(Arimo_Bold_30, WHITE);
    }


    void checkTargetButtons(int touch_x, int touch_y) {
        for (int i=0; i < num_btns; i++) {
            if (btn_array[i]->isTactile()) {
                btn_array[i]->contains(touch_x, touch_y);
            }
        }
    }


    void func_Distance(bool active) {
        if (active) {
            canEditTarget = true;
            btn_Target.writeTextTopCentre(Arimo_Regular_30, YELLOW);
            btn_Target.writeTextBottomCentre(Arimo_Bold_30, YELLOW, targetValues);
            btn_Distance.writeTextBottomCentre(Arimo_Bold_30, YELLOW, String(targetMillimetres, 4));
            btn_Actual.writeTextBottomCentre(Arimo_Bold_30, WHITE, String(stepper1.XACTUAL())); // update in case stepper has moved (e.g. due to joystick)
        }
        else {
            canEditTarget = false;
            btn_Target.writeTextTopCentre(Arimo_Regular_30, WHITE);
            btn_Target.writeTextBottomCentre(Arimo_Bold_30, WHITE, targetValues);
            btn_Distance.writeTextBottomCentre(Arimo_Bold_30, WHITE, String(targetMillimetres, 4));
        }
    }


    void func_Input(char* label) {
        if (canEditTarget) {
            // if input starts with negative, increase max index
            maxTargetValueIndex = targetValues[0] == '-' ? 6 : 5;
            // if input is numeric value, add to array
            if (label[0] != 'X' && label[0] != '-') {
                targetValues[targetValueIndex] = label[0];
                incrementIndex();                               // increment on valid input
            }
            // else delete value
            else if (label[0] == 'X') {
                if (targetValues[targetValueIndex] == '\0') {   // decrement then delete
                    decrementIndex();
                    targetValues[targetValueIndex] = '\0';
                }
                else {                                          // delete then decrement
                    targetValues[targetValueIndex] = '\0';
                    decrementIndex();
                }
            }
            // or add negative if first value in array
            else if (label[0] == '-'&& targetValueIndex == 0) {
                targetValues[targetValueIndex] = label[0];
                incrementIndex();                               // increment on valid input
            }
            // if last value is empty, set to 0
            if (targetValues[0] == '\0') {
                targetValues[0] = '0';
            }

            // calculate distance in mm
            targetMillimetres = atoi(targetValues)*(MICROSTEP_DIST/1000.0000);
            btn_Target.writeTextBottomCentre(Arimo_Bold_30, YELLOW, targetValues);
            btn_Distance.writeTextBottomCentre(Arimo_Bold_30, YELLOW, String(targetMillimetres, 4));
        }
    }


    void incrementIndex() {
        targetValueIndex++;
        if (targetValueIndex > maxTargetValueIndex) {
            targetValueIndex = maxTargetValueIndex;
        }
    }


    void decrementIndex() {
        targetValueIndex--;
        if (targetValueIndex < 0) {
            targetValueIndex = 0;
        }
    }


    void func_Back(bool active) {
        if (active && !canEditTarget) {
            ui.populateScreen(routines::Stack);
        }
    }


    void func_Run(bool active) {
        if (active && !canEditTarget) {
            long nrSteps = atoi(targetValues);
            btn_Run.drawButton(CUSTOM_RED);
            stepper1.VMAX(20000);
            stepper1.executeSteps(nrSteps);
            stepper1.VMAX(STEALTH_CHOP_VMAX);
            btn_Run.drawButton(CUSTOM_GREEN);
            btn_Run.setToggleActive(false); // reset button
            btn_Actual.writeTextBottomCentre(Arimo_Bold_30, WHITE, String(stepper1.XACTUAL()));
        }
    }


    void func_saveStepDist(bool active) {
        if (active && !canEditTarget) {
            long nrSteps = atoi(targetValues);
            stepper1.stepsPerMovement(nrSteps);
            stepper1.calculateStepSize(); // update step size
            btn_Save.drawButton(CUSTOM_GREEN);
            delay(250);
            btn_Save.drawButton(WHITE);
        }
    }

}
