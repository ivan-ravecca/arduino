#include "SensorUtils.h"
#include "TimeUtils.h"
#include "AlarmUtils.h"
#include "DisplayUtils.h"
#include "SerialComm.h"

void setup()
{
  // Initialize serial communication
  Serial.begin(9600);
  Serial.println("=======================================");
  Serial.println("++ AC Voltage Sensor Calibration ++");
  Serial.println("=======================================");

  // Initialize system components in proper order
  initSerialComm(); // Initialize ESP32 communication first
  delay(1000);      // Give time for ESP32 to respond

  initTime(); // Then initialize time (may use ESP32 time)
  initSensors();
  initAlarm();

  // Alert at startup
  dash();
  dash();
  dash();

  // Allow sensor to stabilize
  delay(1000);
}

void loop()
{
  // Handle any incoming messages from ESP32
  handleIncomingMessages();

  // Get current timestamp
  time_t currentTime = now();

  // Check if it's time to sync
  checkTimeSync(currentTime);

  // Process sensor readings at regular interval
  processReadings(currentTime);

  delay(2000); // Small delay but keep responsive to ESP32
}