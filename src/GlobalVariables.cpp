#include "GlobalVariables.h"
#include "StepperControl.h"
#include "MiscFunctions.h"


// video360
int revsPerMinute                   = 50;               // revs per minute for video 360, stored as 10x higher to avoid floating point math
bool video360Active                 = false;            // is video 360 running?
long video360Target                 = 10000;            // set target number of steps for video 360

 

void setRevsPerMinute(int rpm) {
    revsPerMinute = rpm;
}


int getRevsPerMinute() {
    return revsPerMinute;
}


void setVideo360Active(bool active) {
    video360Active = active;
}


bool isVideo360Active() {
    return video360Active;
}


void setVideo360Target(long target) {
    video360Target = target;
}


long getVideo360Target() {
    return video360Target;
}