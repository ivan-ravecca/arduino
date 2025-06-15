#ifndef ALARM_UTILS_H
#define ALARM_UTILS_H

#include <Arduino.h>

extern const int buzzerPin;

void initBuzzer();
void dot();
void dash();
void doSOS();

#endif