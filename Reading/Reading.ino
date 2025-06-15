#include "sensorUtils.h"
#include "timeUtils.h"
#include "alarmUtils.h"
#include "displayUtils.h"

void setup()
{
  // Initialize serial communication
  Serial.begin(9600);
  Serial.println("=======================================");
  Serial.println("++ AC Voltage Sensor Calibration ++");
  Serial.println("=======================================");

  // Initialize system components
  initTime();
  initSensors();
  initAlarm();

  // SOS alert at startup
  doSOS();

  // Allow sensor to stabilize
  delay(5000);
}

void loop()
{
  // Get current timestamp
  time_t currentTime = now();

  // Check if it's time to sync
  checkTimeSync(currentTime);

  // Process sensor readings at regular interval
  processReadings(currentTime);

  delay(1000); // Small delay to avoid flooding serial output
}