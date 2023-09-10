#ifndef PHOTO360_H
#define PHOTO360_H

#include <Arduino.h>
#include "VariableDeclarations.h"
#include "StepperControl.h"
#include "MiscFunctions.h"
#include "StatusEnums.h"

#define photoCountArraySize 20 // need to ensure we don't go out of bounds

class Photo360 {
    public:
        TMC5160Stepper_Ext &_stepper;           // stepper that will be used for the AutoStack
        // ctor
        Photo360(TMC5160Stepper_Ext &stepper): _stepper{stepper} {}

        void run();
        void executeMovement(int stepDirection, unsigned long stepperDelay);
        void lastCheckMillis(long millis) { m_lastCheckMillis = millis; }
        long lastCheckMillis() { return m_lastCheckMillis; }
        void lastMovementMillis(long millis) { m_lastMovementMillis = millis; }
        long lastMovementMillis() { return m_lastMovementMillis; }
        void status(routines::Photo status) { m_status = status; }
        routines::Photo status() {return m_status; }
        // whether photo360 is currently mid procedure, i.e. not inactive or paused
        bool busy() {
            bool isBusy = (status() != routines::inactive && status() != routines::paused);
            return isBusy;
        }
        int requiredPhotos() {
            m_requiredPhotos = photoCountArray[m_photoCountArrayIndex];
            return m_requiredPhotos;
        }
        void incrementRequiredPhotos() {
            // minus one as array index starts at 0
            if (m_photoCountArrayIndex < photoCountArraySize-1) {
                m_photoCountArrayIndex++;
            }
        }
        void decrementRequiredPhotos() {
            if (m_photoCountArrayIndex > 0) {
                m_photoCountArrayIndex--;
            }
        }
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
        void completedPhotos(int nrPhotos) { m_completedPhotos = nrPhotos; }
        int completedPhotos() { return m_completedPhotos; }

    
    protected:

        routines::Photo m_status        = routines::inactive; // current state of AutoStack
        bool m_isNewPhoto360            = true;
        long m_lastCheckMillis          = 0;        // store readings of millis() to use for checking conditions within loops every X milliseconds
        long m_lastMovementMillis       = 0;        // millis of most recent step taken
        int m_completedMovements        = 0;        // number of completed movements (multiples of steps) within an AutoStack procedure
        int m_requiredMovements         = 0;        // number of movements (multiples of steps) required for an AutoStack procedure
        int m_photoCountArrayIndex      = 11;       // index for number of photoCountArray
        int m_requiredPhotos            = 72;       // nr photos taken in a 360 photo
        long m_shutterDelay             = 1000;     // delay in milliseconds between each photo in a photo360 procedure
        int m_completedPhotos           = 0;        // nr of photos taken in a 360 procedure

        int photoCountArray[photoCountArraySize] = {6,12,18,24,30,36,42,48,54,60,66,72,78,84,90,96,102,108,114,120}; // possible values for nr360Photos
};

extern Photo360     photo360;

#endif