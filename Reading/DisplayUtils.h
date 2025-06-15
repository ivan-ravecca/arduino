#ifndef DISPLAY_UTILS_H
#define DISPLAY_UTILS_H

#include <Arduino.h>
#include <TimeLib.h>

// Function declarations
void printDateTime(time_t t);
void printDuration(unsigned long durationSeconds);

#endif