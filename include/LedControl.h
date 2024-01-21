#ifndef LEDCONTROL_H
#define LEDCONTROL_H

#include <Arduino.h>
#include "VariableDeclarations.h"
#include "AutoStack.h"
#define nrReadings 8

class ledLight {
    public:
        ledLight(int pwmPin = LED_PIN, int dimmerPin = POT_PIN): m_pwmPin{pwmPin}, m_dimmerPin{dimmerPin} {}

    int pwmPin() { return m_pwmPin; }                           // pin used for outputting PWM signal to led driver
    int dimmerPin() { return m_dimmerPin; }                     // pin used for reading value from potentiometer
    void enabled(bool enable) {                                 // enable/disable the LED
        if (enable) {
            analogWrite(pwmPin(), m_brightness);
        } else {
            analogWrite(pwmPin(), 0);
        }
        m_enabled = enable;
    }
    bool enabled() { return m_enabled; }                        // get current state of led
    int readDimmer() {                                          // get value from potentiometer for setting brightness
        int val = analogRead(dimmerPin());
        if (m_counter >= nrReadings) {
            m_counter = 0;
        }
        // remove the previous reading in this position from the total
        m_readingsTotal -= m_readingsArray[m_counter];
        // add the new reading to the array
        m_readingsArray[m_counter] = val;
        // add the new reading to the total
        m_readingsTotal += val;
        // iterate the counter
        m_counter++;

        val = m_readingsTotal / nrReadings;
        return val;
    }
    int convertToPWM(int val) {
        int pwmVal = map(val, 0, 1023, MIN_LED_PWM, MAX_LED_PWM);
        return pwmVal;
    }
    void updateBrightness() {                                   // update led brightness based on current potentiometer reading
        if (enabled()) {
            if (stack.busy() && stack.maxBackgroundLED()) {
                // set led to max if in a stack and maxBackgroundLED = true
                setBrightness(255);
            } else {
                m_brightness = convertToPWM(readDimmer());
                if (m_brightness != m_previousBrightness) {
                    analogWrite(pwmPin(), m_brightness);
                    m_previousBrightness = m_brightness;
                }
            }
        }
    }
    void setBrightness(int value) {                             // set led brightness to specific value
        if (enabled()) {
            analogWrite(pwmPin(), value);
            m_brightness = value;
        }
    }
    int getBrightness() { return m_brightness; }                // get current led brightness
    void lastCheckMillis(long millis) { m_lastCheckMillis = millis; }
    long lastCheckMillis() { return m_lastCheckMillis; }


    protected:
    int m_previousVal           =   0;
    int m_pwmPin;
    int m_dimmerPin;
    int m_brightness            =   0;
    int m_previousBrightness    =   1;
    bool m_enabled              =   true;
    long m_lastCheckMillis      =   0;
    int m_readingsArray[nrReadings];
    int m_counter               =   0;
    int m_readingsTotal         =   0;

};

extern ledLight  led;

#endif