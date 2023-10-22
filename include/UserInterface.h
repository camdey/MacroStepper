#ifndef USERINTERFACE_H
#define USERINTERFACE_H

#include <Arduino.h>
#include "VariableDeclarations.h"
#include "StatusEnums.h"

class UserInterface {
    public:
        // ctor
        UserInterface() = default;

        void activeScreen(routines::Screens screen) { m_screen = screen; }
        routines::Screens activeScreen() {return m_screen; }
        void rotateScreen() {
            if (!screenRotated()) {
                tft.setRotation(1);
                screenRotated(true);
            }
            else if (screenRotated()) {
                tft.setRotation(3);
                screenRotated(false);
            }
        }
        void screenRotated(bool rotated) { m_screenRotated = rotated; }
        bool screenRotated() { return m_screenRotated; }
        void lastCheckMillis(long millis) { m_lastCheckMillis = millis; }
        long lastCheckMillis() { return m_lastCheckMillis; }
        void initButtons(unsigned long toggleDebounce, unsigned long momentaryDebounce);
        void populateScreen(routines::Screens screen);
        void previousScreen(routines::Screens screen) {
            m_prevScreen = screen;
        }
        routines::Screens previousScreen() {return m_prevScreen; }
        void readTouchScreen(routines::Screens screen);
        void canEdit(routines::Buttons button, bool state) {
            if (button == routines::btn_flashOn) {
                m_flashOn = state;
            } else if (button == routines::btn_flashOff) {
                m_flashOff = state;
            } else if (button == routines::btn_distance) {
                m_distance = state;
            } else if (button == routines::btn_startPosition) {
                m_startPosition = state;
            } else if (button == routines::btn_endPosition) {
                m_endPosition = state;
            } else if (button == routines::btn_shutterDelay) {
                m_shutterDelay = state;
            } else if (button == routines::btn_arrows) {
                m_arrows = state;
            } else if (button == routines::btn_photo360Delay) {
                m_photo360Delay = state;
            } else if (button == routines::btn_photo360Nr) {
                m_photo360Nr = state;
            } else if (button == routines::btn_targetPosition) {
                m_targetPosition = state;
            }
        }
        bool canEdit(routines::Buttons button) {
            if (button == routines::btn_flashOn) {
                return m_flashOn;
            } else if (button == routines::btn_flashOff) {
                return m_flashOff;
            } else if (button == routines::btn_distance) {
                return m_distance;
            } else if (button == routines::btn_startPosition) {
                return m_startPosition;
            } else if (button == routines::btn_endPosition) {
                return m_endPosition;
            } else if (button == routines::btn_shutterDelay) {
                return m_shutterDelay;
            } else if (button == routines::btn_arrows) {
                return m_arrows;
            } else if (button == routines::btn_photo360Delay) {
                return m_photo360Delay;
            } else if (button == routines::btn_photo360Nr) {
                return m_photo360Nr;
            } else if (button == routines::btn_targetPosition) {
                return m_targetPosition;
            }
            return false;
        }

    protected:
        routines::Screens m_screen      = routines::ui_Home;   // set current screen shown to user
        routines::Screens m_prevScreen  = routines::ui_Home;   // set current screen shown to user
        bool m_screenRotated            = false;            // check whether screen has been rotated or not
        bool m_arrows                   = false;
        bool m_shutterDelay             = false;            // set shutter delay time
        bool m_startPosition            = false;            // set start point for auto mode
        bool m_endPosition              = false;            // set end point for auto mode
        bool m_distance                 = false;            // set step distance in any mode
        bool m_flashOn                  = false;            // set flash on value
        bool m_flashOff                 = false;            // set flash off value
        bool m_photo360Delay            = false;            // set photo360 delay
        bool m_photo360Nr               = false;            // set photo360 required photos
        bool m_targetPosition           = false;            // set target position
        long m_lastCheckMillis          = 0;

};

extern UserInterface    ui;

#endif