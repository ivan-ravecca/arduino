#include "SerialComm.h"
#include "TimeUtils.h"
#include "AlarmUtils.h"

// Define pins for SoftwareSerial connection to ESP32
const int ESP_RX_PIN = 2; // Connect to ESP32 TX
const int ESP_TX_PIN = 3; // Connect to ESP32 RX

// Initialize SoftwareSerial
SoftwareSerial espSerial(ESP_RX_PIN, ESP_TX_PIN);

void initSerialComm()
{
    // Use a lower baud rate - 9600 is much more reliable for SoftwareSerial
    espSerial.begin(9600);
    Serial.println("ESP32 communication initialized");

    // Initial time request
    requestTimeFromESP32();
}

void requestTimeFromESP32()
{
    Serial.println("Requesting time from ESP32...");
    espSerial.println("GET_CURRENT_TIME");
}

// Modified to include duration parameter
void sendSensorUpdate(char sensorId, bool isEnergized, float voltage, bool isRepeatedAlarm, unsigned long durationMinutes)
{
    // Format: SENSOR_UPDATE,<ID>,<STATUS>,<VOLTAGE>,<ALARM>,<DURATION>
    espSerial.print("SENSOR_UPDATE,");
    espSerial.print(sensorId);
    espSerial.print(",");
    espSerial.print(isEnergized ? "1" : "0");
    espSerial.print(",");
    espSerial.print(voltage);
    espSerial.print(",");
    espSerial.print(isRepeatedAlarm ? "1" : "0");
    espSerial.print(",");
    espSerial.println(durationMinutes);

    Serial.print("Sent to ESP32: SENSOR_UPDATE,");
    Serial.print(sensorId);
    Serial.print(",");
    Serial.print(isEnergized ? "1" : "0");
    Serial.print(",");
    Serial.print(voltage);
    Serial.print(",");
    Serial.print(isRepeatedAlarm ? "1" : "0");
    Serial.print(",");
    Serial.println(durationMinutes);
}

bool handleIncomingMessages()
{
    bool messageProcessed = false;

    if (espSerial.available())
    {
        String message = espSerial.readStringUntil('\n');
        message.trim();

        Serial.print("Received from ESP32: ");
        Serial.println(message);

        // Process different message types
        if (message.startsWith("TIME:"))
        {
            processSyncTimeMessage(message.substring(5));
            messageProcessed = true;
        }
        else if (message.startsWith("WIFI:CONNECTED"))
        {
            Serial.println("ESP32 WiFi connected.");
            messageProcessed = true;
            // Auditive info to notify esp32 is connected
            dash();
            dot();
            dot();
            dot();
            dash();
        }
        // Add other message types as needed
    }

    return messageProcessed;
}

void processSyncTimeMessage(String timeStr)
{
    // Expected format: TIME:YYYY-MM-DD HH:MM:SS
    Serial.print("Processing time sync: ");
    Serial.println(timeStr);

    // Parse the date-time string
    // Format: 2025-06-18 12:50:20
    int year = timeStr.substring(0, 4).toInt();
    int month = timeStr.substring(5, 7).toInt();
    int day = timeStr.substring(8, 10).toInt();
    int hour = timeStr.substring(11, 13).toInt();
    int minute = timeStr.substring(14, 16).toInt();
    int second = timeStr.substring(17, 19).toInt();

    // Validate parsed values
    if (year >= 2020 && month >= 1 && month <= 12 && day >= 1 && day <= 31 &&
        hour >= 0 && hour <= 23 && minute >= 0 && minute <= 59 && second >= 0 && second <= 59)
    {

        Serial.print("Setting time to: ");
        Serial.print(year);
        Serial.print("-");
        Serial.print(month);
        Serial.print("-");
        Serial.print(day);
        Serial.print(" ");
        Serial.print(hour);
        Serial.print(":");
        Serial.print(minute);
        Serial.print(":");
        Serial.println(second);

        setTime(hour, minute, second, day, month, year);
        updateLastSyncTime(); // Update the last sync time in TimeUtils
    }
    else
    {
        Serial.println("Received invalid date-time format from ESP32");
        Serial.print("Parsed values: Y:");
        Serial.print(year);
        Serial.print(" M:");
        Serial.print(month);
        Serial.print(" D:");
        Serial.print(day);
        Serial.print(" H:");
        Serial.print(hour);
        Serial.print(" M:");
        Serial.print(minute);
        Serial.print(" S:");
        Serial.println(second);
    }
}