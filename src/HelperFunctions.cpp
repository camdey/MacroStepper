#include "HelperFunctions.h"

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