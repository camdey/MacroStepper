#ifndef LEDCONTROL_H
#define LEDCONTROL_H

#include <Arduino.h>
#include "VariableDeclarations.h"

class ledLight {
    public:
        ledLight(int pwmPin = LED_PIN, int dimmerPin = POT_PIN): m_pwmPin{pwmPin}, m_dimmerPin{dimmerPin} {}

    int pwmPin() { return m_pwmPin; }                           // pin used for outputting PWM signal to led driver
    int dimmerPin() { return m_dimmerPin; }                     // pin used for reading value from potentiometer
    void enabled(bool enable) {                                 // enable/disable the LED
        if (enable) {
            analogWrite(pwmPin(), 255);
        } else {
            analogWrite(pwmPin(), 0);
        }
        m_enabled = enable;
    }
    bool enabled() { return m_enabled; }                        // get current state of led
    int readDimmer() { return analogRead(dimmerPin()); }        // get value from potentiometer for setting brightness
    void updateBrightness() {                                   // update led brightness based on current potentiometer reading
        m_previousValue = m_brightness;
        m_brightness = map(readDimmer(), 0, 1023, 0, 255);
        if (m_brightness != previousValue()) {
            analogWrite(pwmPin(), m_brightness);
        }
    }
    void setBrightness(int value) {                             // set led brightness to specific value
        analogWrite(pwmPin(), value);
        m_brightness = value;
    }
    int getBrightness() { return m_brightness; }               // get current led brightness
    void lastCheckMillis(long millis) { m_lastCheckMillis = millis; }
    long lastCheckMillis() { return m_lastCheckMillis; }
    int previousValue() { return m_previousValue; }


    protected:

    int m_pwmPin;
    int m_dimmerPin;
    int m_brightness        =   0;
    int m_previousValue     =   1;
    bool m_enabled          =   false;
    long m_lastCheckMillis  =   0;

};

extern ledLight  led;

#endif