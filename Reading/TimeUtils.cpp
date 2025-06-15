#include "timeUtils.h"

// Time sync variables
unsigned long lastSyncTime = 0;
const unsigned long SYNC_INTERVAL = 3600; // Try to sync time every hour (in seconds)

void initTime()
{
    // Initialize time
    syncMyTime();
}

void syncMyTime()
{
    // This is a placeholder - when internet is available,
    // replace with actual NTP time fetch

    // For now, set a static timestamp (June 15, 2025, 12:00:00)
    setTime(12, 0, 0, 15, 6, 2025); // hr, min, sec, day, month, year

    Serial.println("--------- Time synchronization ---------");
    Serial.println("Manual");
    Serial.print("Current time: ");
    Serial.print(hour());
    Serial.print(":");
    Serial.print(minute());
    Serial.print(":");
    Serial.print(second());
    Serial.print(" ");
    Serial.print(day());
    Serial.print("/");
    Serial.print(month());
    Serial.print("/");
    Serial.println(year());
    Serial.println("------------------------------------");
}

void checkTimeSync(time_t currentTime)
{
    // Check if it's time to sync
    if (currentTime - lastSyncTime >= SYNC_INTERVAL)
    {
        syncMyTime();
        lastSyncTime = now();
    }
}