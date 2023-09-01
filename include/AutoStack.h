#ifndef AUTOSTACK_H
#define AUTOSTACK_H

#include <Arduino.h>
#include "VariableDeclarations.h"
#include "StepperConfig.h"
#include "MiscFunctions.h"

enum StackStatus {
    inactive,
    start,
    paused,
    waitShutter,
    newMovement,
    delayMovement,
    executedMovement,
    debugFlash,
    completedStack
};

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
        void status(StackStatus status) { m_status = status; }
        StackStatus status() {return m_status; }

    
    protected:

        StackStatus m_status        = inactive; // current state of AutoStack
        bool m_isNewStack           = false;    // whether a new autostack sequence has begun
        long m_startPosition        = 0;        // starting position of an AutoStack sequence
        long m_endPosition          = 0;        // end position of an AutoStack sequence
        long m_lastCheckMillis      = 0;        // store readings of millis() to use for checking conditions within loops every X milliseconds
        long m_lastMovementMillis   = 0;        // millis of most recent step taken
        int m_completedMovements    = 0;        // number of completed movements (multiples of steps) within an AutoStack procedure
        int m_requiredMovements     = 0;        // number of movements (multiples of steps) required for an AutoStack procedure
};

extern AutoStack    stack;

#endif