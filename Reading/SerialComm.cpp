#include "SerialComm.h"
#include "TimeUtils.h"

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

void sendSensorUpdate(char sensorId, bool isEnergized, float voltage, bool isRepeatedAlarm)
{
    // Format: SENSOR_UPDATE,<ID>,<STATUS>,<VOLTAGE>,<ALARM>
    espSerial.print("SENSOR_UPDATE,");
    espSerial.print(sensorId);
    espSerial.print(",");
    espSerial.print(isEnergized ? "1" : "0");
    espSerial.print(",");
    espSerial.print(voltage);
    espSerial.print(",");
    espSerial.println(isRepeatedAlarm ? "1" : "0");

    Serial.print("Sent to ESP32: SENSOR_UPDATE,");
    Serial.print(sensorId);
    Serial.print(",");
    Serial.print(isEnergized ? "1" : "0");
    Serial.print(",");
    Serial.print(voltage);
    Serial.print(",");
    Serial.println(isRepeatedAlarm ? "1" : "0");
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
        // Add other message types as needed
    }

    return messageProcessed;
}

void processSyncTimeMessage(String timeStr)
{
    // Expected format: TIME:1656789012 (Unix timestamp)
    Serial.print("Processing time sync: ");
    Serial.println(timeStr);

    unsigned long unixTime = timeStr.toInt();

    // Check if time value seems valid (after 2020-01-01)
    if (unixTime > 1577836800)
    {
        Serial.println("Setting time from ESP32");
        setTime(unixTime);
        updateLastSyncTime(); // Update the last sync time in TimeUtils
    }
}