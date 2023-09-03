#ifndef AUTOSTACK_H
#define AUTOSTACK_H

#include <Arduino.h>
#include "VariableDeclarations.h"
#include "StepperControl.h"
#include "MiscFunctions.h"
#include "StatusEnums.h"


class AutoStack {
    public:
        TMC5160Stepper_Ext _stepper;           // stepper that will be used for the AutoStack
        // ctor
        AutoStack(TMC5160Stepper_Ext &stepper): _stepper(stepper) {}

        void init();
        void run();
        void executeMovement(int stepDirection, unsigned long stepperDelay);
        void goToStart();
        void overshootPosition(long startPosition, int numberOfSteps);
        void terminateAutoStack();
        void dryRun();
        void startPosition(long pos) { 
            m_startPosition = pos;
            // update movementsRequired as calculation has changed
            updateRequiredMovements();
        }
        long startPosition() { return m_startPosition; }
        void endPosition(long pos) {
            m_endPosition = pos;
            // update movementsRequired as calculation has changed
            updateRequiredMovements();
        }
        long endPosition() { return m_endPosition; }
        void runMax(bool max) { m_runMax = max; } // whether the autostack should just keep moving and taking photos until told to stop
        bool runMax() { return m_runMax; }
        void lastCheckMillis(long millis) { m_lastCheckMillis = millis; }
        long lastCheckMillis() { return m_lastCheckMillis; }
        void lastMovementMillis(long millis) { m_lastMovementMillis = millis; }
        long lastMovementMillis() { return m_lastMovementMillis; }
        void completedMovements(int movements) { m_completedMovements = movements; }
        void incrementCompletedMovements() { m_completedMovements++; }
        void decrementCompletedMovements() { m_completedMovements--; }
        long completedMovements() { return m_completedMovements; }
        // void requiredMovements(long movements) { m_requiredMovements = movements; }
        void updateRequiredMovements() {
            // calculate number of steps required to cover range
            int nrMovements = ceil((endPosition()*1.00 - startPosition()*1.00) / _stepper.stepsPerMovement());
            // prevent screen under/overflow of value
            nrMovements = valueCheck(nrMovements, 0, 999);
            m_requiredMovements = nrMovements;
        }
        long requiredMovements() { return m_requiredMovements; }
        void shutterDelay(int delay) {m_shutterDelay = valueCheck(delay, SHUTTER_PULL_TIME, 5900);}   // Set delay between a movement and taking a photo via shutter pin
        int shutterDelay() { return m_shutterDelay; }
        void incrementShutterDelay() {
            m_shutterDelay+=100;
            m_shutterDelay = valueCheck(m_shutterDelay, SHUTTER_PULL_TIME, 5900);
        }
        void decrementShutterDelay() {
            m_shutterDelay-=100;
            m_shutterDelay = valueCheck(m_shutterDelay, SHUTTER_PULL_TIME, 5900);
        }
        // Get delay in seconds in a displayable String format between a movement and taking a photo via shutter pin
        String getShutterDelaySeconds() {
            int delay = shutterDelay();
            int thousandths = delay*0.001;
            int hundreths = (delay-(thousandths*1000))*0.01;
            return String(thousandths) + '.' + String(hundreths);
        }
        void status(routines::Photo status) { m_status = status; }
        routines::Photo status() {return m_status; }
        // whether AutoStack is currently mid procedure, i.e. not inactive or paused
        bool busy() {
            bool isBusy = (status() != routines::inactive || status() != routines::paused);
            return isBusy;
        }

    
    protected:

        routines::Photo m_status    = routines::inactive; // current state of AutoStack
        long m_startPosition        = 0;        // starting position of an AutoStack sequence
        long m_endPosition          = 0;        // end position of an AutoStack sequence
        long m_lastCheckMillis      = 0;        // store readings of millis() to use for checking conditions within loops every X milliseconds
        long m_lastMovementMillis   = 0;        // millis of most recent step taken
        long m_shutterDelay         = 1000;     // delay between movements
        int m_completedMovements    = 0;        // number of completed movements (multiples of steps) within an AutoStack procedure
        int m_requiredMovements     = 0;        // number of movements (multiples of steps) required for an AutoStack procedure
        bool m_runMax               = false;    // whether autostack should keep moving and taking photos until told to stop
};

extern AutoStack    stack;

#endif