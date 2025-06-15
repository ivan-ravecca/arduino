#ifndef TIME_UTILS_H
#define TIME_UTILS_H

#include <Arduino.h>
#include <TimeLib.h>

extern unsigned long lastSyncTime;
extern const unsigned long SYNC_INTERVAL;

void initTime();
void syncMyTime();
void checkTimeSync(time_t currentTime);

#endif