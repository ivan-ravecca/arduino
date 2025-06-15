#ifndef ALARM_UTILS_H
#define ALARM_UTILS_H

#include <Arduino.h>

// Constants
extern const int buzzerPin;
extern const int dotDuration;
extern const int buzzerTone;

// Function declarations
void initAlarm();
void dot();
void dash();
void doSOS();

#endif