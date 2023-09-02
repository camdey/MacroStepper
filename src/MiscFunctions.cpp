#include "GlobalVariables.h"
#include "MiscFunctions.h"
#include "menu/UI-Main.h"
#include "menu/UI-Manual.h"
#include "menu/UI-Auto.h"


void rotateScreen() {
    if (!isScreenRotated()) {
        tft.setRotation(3);
        setScreenRotated(true);
    }
    else if (isScreenRotated()) {
        tft.setRotation(1);
        setScreenRotated(false);
    }
}


// sound a tone with a given number of repetitions each for a specified length of time (ms) separated by an interval (ms)
void produceTone(int reps, int duration, int interval) {
    for (int i = 0; i < reps; i++) {
        TimerFreeTone(PIEZO_PIN, 4000, duration, 10);
        delay(interval);
    }
}


// check whether a given value is in a specified range
// if not change value to min or max, whichever is closest
int valueCheck(int value, int min, int max) {
    if (value > max) {
        value = max;
    }
    if (value < min) {
        value = min;
    }
    else if (value >= min && value <= max) {
        value = value;
    }
    return value;
}
