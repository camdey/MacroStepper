#include "VariableDeclarations.h"
#include "StepperConfig.h"
#include "GlobalVariables.h"
#include "MiscFunctions.h"
#include "AutoStack.h"
#include "StepperControl.h"
#include "ShutterControl.h"
#include "menu/UI-Main.h"
#include "menu/UI-Auto.h"
#include "menu/UI-AutoConfig.h"
#include "menu/UI-Global.h"
#include "menu/UI-Photo360.h"

/***********************************************************************
Runs an AutoStack procedure comprised of multiple Movements. The
function will first check whether the stage is in the Start position,
if not it will move to the Start. If the number of completed
Movements is less than the number of Movements required, it will
attempt to take a photo and move the stage one Movement. If no
Movement was taken due to delay, this function is called again and
another Movement attempted. When all Movements required are completed,
the autoStack will be completed and ceased to be called.
***********************************************************************/
void autoStack(TMC5160Stepper_Ext &stepper) {
    readyStealthChop(stepper);
    
    if (isNewAutoStack) {                               // move to start position if beginning AutoStack
        stepper.targetVelocity(10000);                  // reduce max velocity to minimize vibration
        goToStart(stepper);
        setNrMovementsCompleted(0);
        stepper.executedMovement(false);
        setShutterTriggered(false);
        setLastStepTime(millis());
        global::func_Reset(false);                      // change reset button to red
        auto_screen::stackStatus(start);
    }

    if (getNrMovementsCompleted() <= getNrMovementsRequired() && !stepper.executedMovement()) {
         // take photo if there's been >= 500ms since a movement was executed successfully (gives time for vibration to settle)
		if (isShutterEnabled() && !hasShutterTriggered() && millis() - getLastStepTime() >= 500) {
            // if flashProcedure idle or is successful, start new procedure or trigger flash if !isFlashSensorEnabled
            if (getCurrentStage() == start || getCurrentStage() == newStep) {
                triggerShutter();                       // take photo and trigger flash immediately or start flash procedure
            }
            else {
                runFlashProcedure(false);               // keep trying to fire flash
            }
            if (getCurrentStage() == flashUnresponsive) {
                auto_screen::stackStatus(newStep);      // reset flash procedure
                auto_screen::pauseStack();              // pause autostack so user can troubleshoot flash issue
                populateScreen("Flash");                // go to flashScreen if can't trigger after 10 seconds
            }
		}
        // only move if autoStack hasn't been paused by flash failure
        if (!autoStackPaused && (!isShutterEnabled() || hasShutterTriggered())) {
            unsigned long delay = getShutterDelay();
    		executeMovement(stepper, 1, delay);
        }

		if (stepper.executedMovement()) {
			incrementNrMovementsCompleted();
            auto_screen::stackStatus(newStep);
	        if (getCurrentScreen() == "Auto") {         // make sure correct screen is displaying
	            auto_screen::printAutoStackProgress();
				auto_screen::estimateDuration();
	        }
			setShutterTriggered(false);                 // reset shutter triggered flag
			stepper.executedMovement(false);            // reset executed movement flag
		}
    }
    if (getNrMovementsCompleted() >= getNrMovementsRequired()) {
        auto_screen::stackStatus(stackCompleted);
        terminateAutoStack(stepper);                    // stop AutoStack sequence if end reached
    }
}