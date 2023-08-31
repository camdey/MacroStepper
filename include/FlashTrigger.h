#ifndef FLASHTRIGGER_H
#define FLASHTRIGGER_H

#include <Arduino.h>
#include "VariableDeclarations.h"
#include "StepperConfig.h"
#include "MiscFunctions.h"

enum Status {
    idle,
    pullDown,
    release,
    triggered,
    unresponsive
};

class FlashTrigger {
    public:
        // ctor
        FlashTrigger(int flashPin = FLASH_SENSOR_PIN, uint8_t shutterPin = SHUTTER_PIN)  {
            m_flashPin = flashPin;
            m_shutterPin = shutterPin;
        }

        // Set delay between a movement and taking a photo via shutter pin
        void shutterDelay(int delay) {m_shutterDelay = valueCheck(delay, 500, 5900);}
        int shutterDelay() { return m_shutterDelay; }
        void incrementShutterDelay() {
            m_shutterDelay+=100;
            m_shutterDelay = valueCheck(m_shutterDelay, 500, 5900);
        }
        void decrementShutterDelay() {
            m_shutterDelay-=100;
            m_shutterDelay = valueCheck(m_shutterDelay, 500, 5900);
        }
        // Get delay in seconds in a displayable String format between a movement and taking a photo via shutter pin
        String getShutterDelaySeconds() {
            int delay = shutterDelay();
            int thousandths = delay*0.001;
            int hundreths = (delay-(thousandths*1000))*0.01;
            return String(thousandths) + '.' + String(hundreths);
        }
        void isTestingFlash(bool testing) { m_testingFlash = testing; }
        bool isTestingFlash() { return m_testingFlash; }
        void sensorEnabled(bool enable) { m_sensorEnabled = enable; }       // Set flash availability
        bool sensorEnabled() { return m_sensorEnabled; }                    // Get flash availability
        void sensorValue(int value) { m_sensorValue = value; }              // Set the light sensor reading from the godox pin
        int sensorValue() { return m_sensorValue; }                         // Get the last light sensor reading from the godox pin
        void sensorAdjValue(long value) { m_sensorAdjValue = value; }       // Set the recursive filter value of the light sensor reading from the godox pin
        long sensorAdjValue() { return m_sensorAdjValue; }                  // Get the recursive filter value of the light sensor reading from the godox pin
        void lastFlashMillis(long millis) { m_lastFlashMillis = millis; }   // Set the time of the last successful flash trigger
        long lastFlashMillis() { return m_lastFlashMillis; }                // Get the time of the last successful flash trigger


    protected:
        int m_flashPin;
        uint8_t m_shutterPin;
        int m_shutterDelay          = 500;          // delay in milliseconds between a movement and taking a photo via the shutter pin
        bool m_shutterTriggered     = true;         // shutter successfully triggered or not
        bool m_testingFlash         = false;        // flag for testing flash threshold
        bool m_sensorEnabled        = false;        // is flash bulb enabled, or only take photos without flash?
        long m_lastFlashMillis      = 0;            // last millis time flash was triggered successfully
        bool m_flashAvailable       = false;        // check whether flash is available to trigger or not
        int m_sensorValue           = 0;            // light sensor reading from the godox flash LED
        long m_sensorAdjValue       = 10000;        // recursive filter value of light sensor reading from the godox flash LED
};


extern FlashTrigger flash;

#endif