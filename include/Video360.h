#ifndef VIDEO360_H
#define VIDEO360_H

#include <Arduino.h>
#include "VariableDeclarations.h"
#include "StepperControl.h"
#include "StatusEnums.h"

class Video360 {
    public:
        TMC5160Stepper_Ext &_stepper;           // stepper that will be used for the AutoStack
        // ctor
        Video360(TMC5160Stepper_Ext &stepper): _stepper{stepper} {}

        void run();
        void rpmToVmax();
        void lastCheckMillis(long millis) { m_lastCheckMillis = millis; }
        long lastCheckMillis() { return m_lastCheckMillis; }
        void revsPerMinute(int rpm) {m_revsPerMinute = rpm;}
        int revsPerMinute() {return m_revsPerMinute;}
        void target(long target) {m_target = target;}
        long target() {return m_target;}
        void status(routines::Video status) { m_status = status; }
        routines::Video status() {return m_status; }
        bool busy() {
            bool isBusy = (status() != routines::stopped);
            return isBusy;
        }
        int getDirection() {
            int shaft_modifier;
            if (_stepper.shaft()) {
                shaft_modifier = 1;
            } else {
                shaft_modifier = -1;
            }
            if (_stepper.rotateClockwise()) {
                return 1*shaft_modifier;
            } else {
                return -1*shaft_modifier;
            }
        }

    
    protected:

        routines::Video m_status        = routines::stopped;    // current state of Video360
        long m_lastCheckMillis          = 0;                    // store readings of millis() to use for checking conditions within loops every X milliseconds
        int m_completedPhotos           = 0;                    // nr of photos taken in a 360 procedure
        int m_revsPerMinute             = 50;                   // revs per minute for video 360, stored as 10x higher to avoid floating point math
        long m_target                   = 10000;                // set target number of steps for video 360
};

extern Video360     video360;

#endif