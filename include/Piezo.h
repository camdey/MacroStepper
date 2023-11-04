#ifndef PIEZO_H
#define PIEZO_H

#include <Arduino.h>
#include "VariableDeclarations.h"

class Piezo {
    public:
        Piezo(uint8_t pin = PIEZO_PIN) { m_piezoPin = pin; }
        int getPin() {return m_piezoPin;}
        void enabled(bool enable) { m_enabled = enable; }
        bool enabled() {return m_enabled;}
        void lastCheckMillis(long millis) { m_lastCheckMillis = millis; }
        long lastCheckMillis() { return m_lastCheckMillis; }
        // sound a tone with a given number of repetitions each for a specified length of time (ms) separated by an interval (ms)
        void produceTone(unsigned long frequency, int reps, int duration, int interval, int volume = 10) {
            if (m_enabled) {
                for (int i = 0; i < reps; i++) {
                    TimerFreeTone(m_piezoPin, frequency, duration, volume);
                    delay(interval);
                }
            }
        }

    protected:
        int m_piezoPin;                                // Arduino pin for the piezo pin
        bool m_enabled                  = true;         // if the stepper is enabled
        long m_lastCheckMillis          = 0;            // time of the last millis check
};

extern Piezo    piezo;

#endif