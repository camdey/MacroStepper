#ifndef STEPPERCONFIG_H
#define STEPPERCONFIG_H

#include <Arduino.h>
#include "VariableDeclarations.h"

class TMC5160Stepper_Ext: public TMC5160Stepper {
    public:
        TMC5160Stepper_Ext(uint16_t pinCS, float RS, int8_t link_index = -1);

        void enablePin(int pin) {m_enablePin = pin;}
        int enablePin() {return m_enablePin;}
        void chipSelectPin(int pin) {m_chipSelectPin = pin;}
        int chipSelectPin() {return m_chipSelectPin;}
        void executedMovement(bool executed) {m_executedMovement = executed;}
        bool executedMovement() {return m_executedMovement;}
        void enabled(bool enable) {
            if (enable) {
                delay(10);
                digitalWrite(enablePin(), LOW); // enable
                delay(10);
            } else if (!enable) {
                delay(10);
                digitalWrite(enablePin(), HIGH); // disable
                delay(10);
            }
            m_enabled = enable;
        }
        bool enabled() {return m_enabled;}
        void rotateClockwise (bool clockwise) {m_rotateClockwise = clockwise;}
        bool rotateClockwise() {return m_rotateClockwise;}
        void slaveSelected(bool selected) {
            if (selected) {
                digitalWrite(chipSelectPin(), HIGH);
            } else if (!selected) {
                digitalWrite(chipSelectPin(), LOW);
            }
            m_slaveSelected = selected;
        }
        bool slaveSelected() {return m_slaveSelected;}
        void stallGuardActive(bool active) {m_stallGuardActive = active;}
        bool stallGuardActive() {return m_stallGuardActive;}
        void stepSize(int stepSize) {m_stepSize = stepSize;}
        int stepSize() {return m_stepSize;}
        void stepsPerMovement(int steps) {m_stepsPerMovement = steps;}
        int stepsPerMovement() {return m_stepsPerMovement;}
        void incrementStepsPerMovement() {m_stepsPerMovement++;}
        void decrementStepsPerMovement() {m_stepsPerMovement--;}
        void targetVelocity(long velocity) {m_targetVelocity = velocity;}
        long targetVelocity() {return m_targetVelocity;}
        void homed(bool homed) {m_homed = homed;}
        bool homed() {return m_homed;}
        bool reachedTarget() {return XACTUAL() == XTARGET();}


    protected:
        int m_enablePin;                    // Arduino pin for the stepper enable pin
        int m_chipSelectPin;                // Arduino pin for the stepper chip select
        float m_stepSize = 5.0000;          // distance travelled per movement in micrometres, default to 5um
        int m_stepsPerMovement = 16;        // number of microsteps to travel a specified distance, default to 16 (1 full step or 5um)
        long m_targetVelocity = 200000;     // target velocity = VMAX for TMC5160
        bool m_executedMovement = false;    // whether the stepper successfully executed a movement or if it was deferred
        bool m_enabled = true;       // if the stepper is enabled
        bool m_rotateClockwise = true;      // stepper rotation direction
        bool m_slaveSelected = false;       // if the stepper is the currently selected slave device
        bool m_stallGuardActive = false;    // if stallGuard has been configured
        bool m_homed = false;               // whether the stepper has been homed or not
};

void initDriver(TMC5160Stepper_Ext &stepper, uint16_t rmsCurrent, uint16_t nrMicrosteps, bool shaftDirection, int pinEN, int pinCS);
void configStealthChop(TMC5160Stepper_Ext &stepper);
void configStallGuard(TMC5160Stepper_Ext &stepper);

#endif