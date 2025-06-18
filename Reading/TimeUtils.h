#ifndef TIME_UTILS_H
#define TIME_UTILS_H

#include <Arduino.h>
#include <TimeLib.h>

// Time sync variables
extern unsigned long lastSyncTime;
extern const unsigned long SYNC_INTERVAL;

// Function declarations
void initTime();
void syncMyTime();
void checkTimeSync(time_t currentTime);
void updateLastSyncTime(); // New function to update last sync time

#endif