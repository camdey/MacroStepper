#ifndef JOYSTICKCONTROL_H
#define JOYSTICKCONTROL_H

#include <Arduino.h>
#include "StepperControl.h"
#include "VariableDeclarations.h"

class Joystick {
    public:
        TMC5160Stepper_Ext &_stepper;           // stepper that will be used for the AutoStack
        // ctor
        Joystick(TMC5160Stepper_Ext &stepper, uint8_t stickPin, uint8_t buttonPin = ZSTICK_PIN): _stepper{stepper}, m_stickPin{stickPin}, m_buttonPin{buttonPin} {}

        long calculateVelocity(int pos);
        void calibrateRestPosition();
        void motion(bool btnTrigger);
        void printNewPositions(); 
        int read() { return analogRead(m_stickPin); }                       // Get the input reading from the analog pin
        int readSmoothed();                                                 // Smooth the joystick reading using recursive filtering
        void recursiveFilterValue(long val) { m_recursiveValue = val; }     // Set the last recursively filtered joystick reading
        long recursiveFilterValue() { return m_recursiveValue; }            // Get the last recursively filtered joystick reading
        void lastCheckMillis(long millis) { m_lastCheckMillis = millis; }
        long lastCheckMillis() { return m_lastCheckMillis; }
        void restValUpper(int val) { m_restValUpper = val; }
        int restValUpper() { return m_restValUpper; }
        void restVal(int val) { m_restVal = val; }
        int restVal() { return m_restVal; }
        void restValLower(int val) { m_restValLower = val; }
        int restValLower() { return m_restValLower; }
        void restValDiff(int val) { m_restValDiff = val; }
        int restValDiff() { return m_restValDiff; }
        int buttonActive() { return !digitalRead(m_buttonPin); }            // check if button is active, invert reading as pressed = low
        int getDirection(int pos) { return map(pos, 1024, 0, 0, 2); }       // use maxIn of 1024 and maxOut of 2 due to how map() is calculated. 511 = 0, 512 = 1
        void maxVelocity(long velocity) { m_maxVelocity = velocity; }
        long maxVelocity() { return m_maxVelocity; }

    protected:
        long m_maxVelocity              = 100000;   // VMAX when using joystick
        long m_recursiveValue           = 51200;    // store filtered value of last joystick reading, initialize as 51200 since formula multiplies values by 100 to avoid floats
        long m_lastCheckMillis          = 0;        // store readings of millis() to use for checking conditions within loops every X milliseconds
        int m_restValUpper              = 522;      // Upper boundary of joystick resting point, calibrated during setup
        int m_restVal                   = 512;      // Resting point of joystick reading, calibrated during setup
        int m_restValLower              = 502;      // Lower boundary of of joystick resting point, calibrated during setup
        int m_restValDiff               = 0;        // Difference between ideal middle (512) and actual resting point
        uint8_t m_stickPin;
        uint8_t m_buttonPin;
};

extern Joystick     xStick;
extern Joystick     rStick;

#endif
