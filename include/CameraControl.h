#ifndef CAMERACONTROL_H
#define CAMERACONTROL_H

#include <Arduino.h>
#include "VariableDeclarations.h"
#include "StepperControl.h"
#include "MiscFunctions.h"

enum FlashStatus {
    flashAvailable,
    shutterLow,
    shutterHigh,
    flashRecycling,
    shutterCompleted,
    flashUnavailable,
    flashUnresponsive
};

class CameraControl {
    public:
        // ctor
        CameraControl(uint8_t shutterPin = SHUTTER_PIN, int flashPin = FLASH_SENSOR_PIN) {
            m_shutterPin = shutterPin;
            m_flashPin = flashPin;
        }

        void shutterEnabled(bool enable) { m_shutterEnabled = enable; }                 // set shutter state to determine if a photo will be taken or not
        bool shutterEnabled() { return m_shutterEnabled; }                              // get shutter state to determine if a photo will be taken or not
        void shutterDelay(int delay) {m_shutterDelay = valueCheck(delay, 500, 5900);}   // Set delay between a movement and taking a photo via shutter pin
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
        void flashSensorEnabled(bool enable) { m_flashSensorEnabled = enable; } // Set whether the sensor for detecting flash availability is being used
        bool flashSensorEnabled() { return m_flashSensorEnabled; }              // Get whether the sensor for detecting flash availability is being used
        void sensorValue(int value) { m_sensorValue = value; }                  // Set the light sensor reading from the godox pin
        int sensorValue() { return m_sensorValue; }                             // Get the last light sensor reading from the godox pin
        void sensorAdjValue(long value) { m_sensorAdjValue = value; }           // Set the recursive filter value of the light sensor reading from the godox pin
        long sensorAdjValue() { return m_sensorAdjValue; }                      // Get the recursive filter value of the light sensor reading from the godox pin
        void lastFlashMillis(long millis) { m_lastFlashMillis = millis; }       // Set the time of the last successful flash trigger
        long lastFlashMillis() { return m_lastFlashMillis; }                    // Get the time of the last successful flash trigger
        void flashThreshold(int val) { m_flashThreshold = val; }                // Set threshold value for flash being ready to fire
        int flashThreshold() { return m_flashThreshold; }                       // Get threshold value for flash being ready to fire
        void flashOnValue(int val) { m_flashOnValue = val; }                    // Set flash on value that initially determines whether the flash is likely ready or not
        int flashOnValue() { return m_flashOnValue; }                           // Get flash on value that initially determines whether the flash is likely ready or not
        void flashOffValue(int val) { m_flashOffValue = val; }                  // Set flash on value that initially determines whether the flash is likely recycling
        int flashOffValue() { return m_flashOffValue; }                         // Get flash on value that initially determines whether the flash is likely recycling
        void recycleTime (long time) { m_recycleTime = time; }                  // time it takes to pull shutter high, detect recycling, and flash availability to return
        long recycleTime() { return m_recycleTime; }
        void photoTaken (bool taken) { m_photoTaken = taken; }                  // record if a photo was successfully taken
        bool photoTaken() { return m_photoTaken; }
        void checkFlashAvailability();
        void triggerShutter(bool resetProcedure);
        void runFlashProcedure(bool restart);
        int shutterPin() { return m_shutterPin; }
        int flashPin() { return m_flashPin; }
        void status(FlashStatus status) { m_status = status; }
        FlashStatus status() {return m_status; }

    protected:
        int m_flashPin;
        uint8_t m_shutterPin;
        FlashStatus m_status        = flashUnavailable;  // current state of flash availability
        int m_flashThreshold        = 280;
        int m_flashOnValue          = 300;          // initial value for flash considered as being ready
        int m_flashOffValue         = 30;           // initial value for flash considered as recycling
        int m_shutterDelay          = 500;          // delay in milliseconds between a movement and taking a photo via the shutter pin
        bool m_shutterEnabled       = true;         // enable/disable shutter for taken a photo automatically after a step
        bool m_shutterTriggered     = true;         // shutter successfully triggered or not
        bool m_testingFlash         = false;        // flag for testing flash threshold
        bool m_flashSensorEnabled   = false;        // is flash bulb enabled, or only take photos without flash?
        long m_lastFlashMillis      = 0;            // last millis time flash was triggered successfully
        bool m_flashAvailable       = false;        // check whether flash is available to trigger or not
        int m_sensorValue           = 0;            // light sensor reading from the godox flash LED
        long m_sensorAdjValue       = 10000;        // recursive filter value of light sensor reading from the godox flash LED
        long m_recycleTime          = 200;          // time for the flash to recycle after a photo
        bool m_photoTaken           = false;        // whether a photo was successfully taken
};


extern CameraControl camera;

#endif