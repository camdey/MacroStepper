#ifndef AUTOSTACK_H
#define AUTOSTACK_H

#include <Arduino.h>
#include "VariableDeclarations.h"
#include "StepperConfig.h"
#include "MiscFunctions.h"

class AutoStack {
        public:
                TMC5160Stepper_Ext m_stepper;             // stepper that will be used for the AutoStack
                // AutoStack() = default;
                AutoStack(TMC5160Stepper_Ext &stepper): m_stepper(stepper) {}

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
                void lastMillis(long millis) { m_lastMillis = millis; }
                long lastMillis() { return m_lastMillis; }
                void lastStep(long millis) { m_lastStep = millis; }
                long lastStep() { return m_lastStep; }
                void completedMovements(int movements) { m_completedMovements = movements; }
                long completedMovements() { return m_completedMovements; }
                // void requiredMovements(long movements) { m_requiredMovements = movements; }
                void updateRequiredMovements() {
                        // calculate number of steps required to cover range
                        int nrMovements = ceil((endPosition()*1.00 - startPosition()*1.00) / m_stepper.stepsPerMovement());
                        // prevent screen under/overflow of value
                        nrMovements = valueCheck(nrMovements, 0, 999);
                        m_requiredMovements = nrMovements;
                }
                long getRequiredMovements() { return m_requiredMovements; }
        
        protected:

                long m_startPosition                = 0;                // starting position of an AutoStack sequence
                long m_endPosition                    = 0;                // end position of an AutoStack sequence
                long m_lastMillis                     = 0;                // store readings of millis() to use for checking conditions within loops every X milliseconds
                long m_lastStepTime                 = 0;                // last millis time a step was executed
                long m_lastStep                         = 0;                // millis of most recent step taken
                int m_completedMovements        = 0;                // number of completed movements (multiples of steps) within an AutoStack procedure
                int m_requiredMovements         = 0;                // number of movements (multiples of steps) required for an AutoStack procedure
};

#endif