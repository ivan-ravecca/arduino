#include "TimeUtils.h"
#include "SerialComm.h"

// Time sync variables
unsigned long lastSyncTime = 0;
const unsigned long SYNC_INTERVAL = 3600; // Try to sync time every X seconds
unsigned long timeRequestSentAt = 0;
const unsigned long TIME_REQUEST_TIMEOUT = 5000; // 5 seconds timeout for ESP32 to respond
bool timeRequestPending = false;
bool timeReceived = false;

void initTime()
{
    // Initialize time with default values
    setTime(12, 0, 0, 1, 1, 2025); // Default time: 12:00:00 1/1/2025

    // Request time from ESP32
    syncMyTime();
}

void syncMyTime()
{
    // Request time from ESP32
    Serial.println("--------- Time synchronization ---------");

    // Send time request to ESP32
    requestTimeFromESP32();

    // Set pending flag and record request time
    timeRequestPending = true;
    timeRequestSentAt = millis();

    // The actual time update will happen when ESP32 responds with TIME message
    // which is handled in SerialComm.cpp via processSyncTimeMessage()
}

void updateLastSyncTime()
{
    lastSyncTime = now();
    timeReceived = true;
    timeRequestPending = false;

    Serial.println("Time successfully synchronized");
    printCurrentTime();
}

void printCurrentTime()
{
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
    // Check for timeout on pending time request
    if (timeRequestPending && (millis() - timeRequestSentAt > TIME_REQUEST_TIMEOUT))
    {
        Serial.println("Time request timeout - ESP32 did not respond");
        timeRequestPending = false;

        // If we've never received time, print current (fallback) time
        if (!timeReceived)
        {
            Serial.println("Using fallback time:");
            printCurrentTime();
        }
    }

    // Check if it's time for a regular sync
    if (currentTime - lastSyncTime >= SYNC_INTERVAL && !timeRequestPending)
    {
        syncMyTime();
    }
}