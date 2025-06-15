#include "sensorUtils.h"
#include "timeUtils.h"
#include "displayUtils.h"
#include "alarmUtils.h"

void setup()
{
  initSerial();
  initSensors();
  initTime();
  initBuzzer();

  // Allow sensors to stabilize
  delay(5000);
}

void loop()
{
  // Get current timestamp
  time_t currentTime = now();

  // Check if it's time to sync
  checkTimeSync(currentTime);

  // Process sensor readings at regular intervals
  processSensorReadings(currentTime);

  delay(1000); // Small delay to avoid flooding the serial output
}