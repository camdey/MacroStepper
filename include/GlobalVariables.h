#ifndef GLOBALVARIABLES_H
#define GLOBALVARIABLES_H

#include <Arduino.h>

void setRevsPerMinute(int rpm);
int getRevsPerMinute();
void setVideo360Active(bool active);
bool isVideo360Active();
void setVideo360Target(long target);
long getVideo360Target();


#endif
