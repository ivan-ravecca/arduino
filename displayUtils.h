#ifndef DISPLAY_UTILS_H
#define DISPLAY_UTILS_H

#include <Arduino.h>
#include <TimeLib.h>

void printDuration(unsigned long durationSeconds);
void printDateTime(time_t t);

#endif