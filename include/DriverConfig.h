#ifndef DRIVERCONFIG_H
#define DRIVERCONFIG_H

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
        void stepperEnabled(bool enable) {
            if (enable) {
                delay(10);
                digitalWrite(enablePin(), LOW); // enable
                delay(10);
            } else if (!enable) {
                delay(10);
                digitalWrite(enablePin(), HIGH); // disable
                delay(10);
            }
            m_stepperEnabled = enable;
        }
        bool stepperEnabled() {return m_stepperEnabled;}
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

    protected:
        int m_enablePin;
        int m_chipSelectPin;
        bool m_executedMovement = false;
        bool m_stepperEnabled = true;
        bool m_rotateClockwise = true;
        bool m_slaveSelected = false;
        bool m_stallGuardActive = false;
};

void initDriver(TMC5160Stepper_Ext &driver, uint16_t rmsCurrent, uint16_t nrMicrosteps, bool shaftDirection, int pinEN, int pinCS) {};
void configStealthChop(TMC5160Stepper_Ext &driver);
void configStallGuard(TMC5160Stepper_Ext &driver);

#endif