#include <ZMPT101B.h>
#include <TimeLib.h>  // Add TimeLib for timestamp handling

#define SENSOR_A_SENSITIVITY 660.0f
#define SENSOR_B_SENSITIVITY 547.0f
#define UTE_FREQUENCY 50.0

const int sensorAPin = A0;       // Analog pin for first ZMPT101B
const int sensorBPin = A1;       // Analog pin for second ZMPT101B

ZMPT101B voltageSensorA(sensorAPin, UTE_FREQUENCY);
ZMPT101B voltageSensorB(sensorBPin, UTE_FREQUENCY);

// ++++++++++++++++++++++++++++
// Threshold and timing variables
const float VOLTAGE_THRESHOLD = 180.0;  // Only report as energized above 180V
const unsigned long READING_INTERVAL = 10;  // Read every X seconds
const unsigned long LONG_RUN_THRESHOLD = 600;  // X minute threshold for long run warning (in seconds)

// Time sync variables
unsigned long lastSyncTime = 0;
const unsigned long SYNC_INTERVAL = 3600;  // Try to sync time every hour (in seconds)

// State tracking variables
bool sensorAEnergized = false;
bool sensorBEnergized = false;
time_t sensorAStartTime = 0;
time_t sensorBStartTime = 0;
time_t lastReadingTime = 0;
bool sensorALongRunReported = false;  // Track if long run was already reported
bool sensorBLongRunReported = false;  // Track if long run was already reported
bool firstRun = true;  // Add this as a global variable at the top

time_t sensorALastAlertTime = 0;  // Track when last alert was sent for sensor A
time_t sensorBLastAlertTime = 0;  // Track when last alert was sent for sensor B
const unsigned long ALERT_REPEAT_INTERVAL = LONG_RUN_THRESHOLD / 2UL;  // Repeat alert every X minutes 
// ++++++++++++++++++++++++++++

const int buzzerPin = 8;
const int dotDuration = 200;
const int buzzerTone = 3500;

void setup() {
    Serial.begin(9600);
    Serial.println("=======================================");
    Serial.println("++ AC Voltage Sensor Calibration ++");
    Serial.println("=======================================");

    // Initialize time
    syncMyTime();

    // Set the sensitivity value (must be calibrated)
    voltageSensorA.setSensitivity(SENSOR_A_SENSITIVITY);
    voltageSensorB.setSensitivity(SENSOR_B_SENSITIVITY);

    Serial.print("Sensitivity: Sensor A: ");
    Serial.print(SENSOR_A_SENSITIVITY);
    Serial.print(" Sensor B: ");
    Serial.println(SENSOR_B_SENSITIVITY);
    Serial.print("LONG_RUN_THRESHOLD at ");
    Serial.print(LONG_RUN_THRESHOLD/60);
    Serial.println(" minutes");
    Serial.println("=======================================");

    pinMode(buzzerPin, OUTPUT);

    doSOS();

    // Allow sensor to stabilize
    delay(5000);
}

void loop() {
  // Get current timestamp
  time_t currentTime = now();
  
  // Check if it's time to sync
  if (currentTime - lastSyncTime >= SYNC_INTERVAL) {
      syncMyTime();
      lastSyncTime = now();
  }

  if (currentTime - lastReadingTime >= READING_INTERVAL) {
      float voltageA = readVoltage(sensorAPin);
      float voltageB = readVoltage(sensorBPin);

      // Process Sensor 1
      processSensorState(sensorAPin, voltageA, sensorAEnergized, sensorAStartTime, currentTime, sensorALongRunReported);
      // Process Sensor 2
      processSensorState(sensorBPin, voltageB, sensorBEnergized, sensorBStartTime, currentTime, sensorBLongRunReported);
  
      displayCurrentReadings(voltageA, voltageB);
      lastReadingTime = currentTime;
      
      if (firstRun) {
          firstRun = false;
      }
  }
  delay(1000);  // Small delay to avoid flooding the serial output
}

// Function to sync time - will be expanded later for internet connectivity
void syncMyTime() {
  // This is a placeholder - when internet is available, 
  // replace with actual NTP time fetch
  
  // For now, set a static timestamp (June 15, 2025, 12:00:00)
  setTime(12, 0, 0, 15, 6, 2025);  // hr, min, sec, day, month, year
  
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


void readMultipleSamples(int sensor) {
    // Read RMS voltage with 5 samples for better accuracy
    float voltage;
    if(sensor == sensorAPin) {
        voltage = voltageSensorA.getRmsVoltage(5);
    } else if(sensor == sensorBPin) {
        voltage = voltageSensorB.getRmsVoltage(5);
    }
    

    Serial.print("AC Voltage (5 samples) for sensor ");
    Serial.print((sensor == sensorAPin) ? 'A' : 'B');
    Serial.print(": ");
    Serial.print(voltage, 2);
    Serial.println(" V");
}

float readVoltage(int sensor) {
    // Read RMS voltage with 5 samples for better accuracy
    if(sensor == sensorAPin) {
        return voltageSensorA.getRmsVoltage(5);
    } else if(sensor == sensorBPin) {
        return voltageSensorB.getRmsVoltage(5);
    }
}

void displayCurrentReadings(float voltageA, float voltageB) {
  // Only display if either sensor is energized or for debugging
  if (sensorAEnergized || sensorBEnergized) {
    Serial.println("======= Sensor reading =======");
    Serial.print("Time: ");
    printDateTime(now());
    Serial.print("Sensor A: ");
    Serial.print(voltageA, 1);
    Serial.print("V");
    if (sensorAEnergized) Serial.print(" [ON]"); else Serial.print(" [OFF]");
    Serial.println();
    Serial.print("Sensor B: ");
    Serial.print(voltageB, 1);
    Serial.print("V");
    if (sensorBEnergized) Serial.print(" [ON]"); else Serial.print(" [OFF]");
    Serial.println();
  }
}

void processSensorState(int sensorNum, float voltage, bool &isEnergized, time_t &startTime, 
                        time_t currentTime, bool &longRunReported) {
  bool aboveThreshold = (voltage > VOLTAGE_THRESHOLD);
  
  // Special handling for initial state detection
  if (firstRun && aboveThreshold) {
    return;  // Skip the rest of the processing for first run
  }
  
  if (aboveThreshold && !isEnergized) {
    // Sensor just became energized
    isEnergized = true;
    longRunReported = false; // Reset the long run flag
    startTime = currentTime;
    Serial.print("*** Sensor ");
    Serial.print((sensorNum == sensorAPin) ? 'A' : 'B');
    Serial.print(" ENERGIZED at ");
    Serial.print(voltage, 2);
    Serial.print("V at ");
    printDateTime(currentTime);
    
  } else if (!aboveThreshold && isEnergized) {
    // Sensor just became de-energized
    isEnergized = false;
    longRunReported = false; // Reset the long run flag
    // Reset alert time
    if (sensorNum == sensorAPin) {
      sensorALastAlertTime = 0;
    } else {
      sensorBLastAlertTime = 0;
    }
    
    unsigned long energizedDuration = currentTime - startTime;
    startTime = 0; // Reset start time
    // Report energization time
    Serial.println("========================================");
    Serial.print(">>> Sensor ");
    Serial.print((sensorNum == sensorAPin) ? 'A' : 'B');
    Serial.println(" DE-ENERGIZED <<<");
    Serial.print("Duration: ");
    printDuration(energizedDuration);
    Serial.print("Final voltage: ");
    Serial.print(voltage, 2);
    Serial.println("V");
    Serial.println("========================================");
  }
  
  time_t &lastAlertTime = (sensorNum == sensorAPin) ? sensorALastAlertTime : sensorBLastAlertTime;
  
  // Check for initial long run condition
  if (isEnergized && ((unsigned long)(currentTime - startTime) > LONG_RUN_THRESHOLD) && !longRunReported) {
    unsigned long runningDuration = currentTime - startTime;
    longRun(sensorNum, runningDuration);
    longRunReported = true; // Prevent repeated immediate reports
    lastAlertTime = currentTime; // Set initial alert time
  }
  
  // Check for repeating alerts if already in long-run state
  if (isEnergized && longRunReported && 
      ((unsigned long)(currentTime - lastAlertTime) >= ALERT_REPEAT_INTERVAL)) {
    unsigned long runningDuration = currentTime - startTime;
    Serial.println("!!! REPEAT WARNING !!!");
    Serial.print("Sensor ");
    Serial.print((sensorNum == sensorAPin) ? 'A' : 'B');
    Serial.print(" has been running for ");
    printDuration(runningDuration);
    Serial.print("At this time: ");
    printDateTime(currentTime);
    Serial.println("!!! REPEAT WARNING !!!");
    doSOS();
    lastAlertTime = currentTime; // Update the last alert time
  }
}

// Function to print current date and time
void printDateTime(time_t t) {
  Serial.print(hour(t));
  Serial.print(":");
  Serial.print(minute(t));
  Serial.print(":");
  Serial.print(second(t));
  Serial.print(" ");
  Serial.print(day(t));
  Serial.print("/");
  Serial.print(month(t));
  Serial.print("/");
  Serial.println(year(t));
}

void printDuration(unsigned long durationSeconds) {
  unsigned long minutes = durationSeconds / 60;
  unsigned long seconds = durationSeconds % 60;
  unsigned long hours = minutes / 60;
  minutes = minutes % 60;
  
  if (hours > 0) {
    Serial.print(hours);
    Serial.print(" hr ");
  }
  if (minutes > 0) {
    Serial.print(minutes);
    Serial.print(" min ");
  }
  Serial.print(seconds);
  Serial.println(" sec");
}

void longRun(int sensorNum, unsigned long duration) {
  Serial.println("!!! WARNING !!!");
  Serial.print("Sensor ");
  Serial.print((sensorNum == sensorAPin) ? 'A' : 'B');
  Serial.println(" has been running for more than ");
  Serial.print(LONG_RUN_THRESHOLD);
  Serial.print(" minutes.");
  Serial.print("Current duration: ");
  printDuration(duration);
  Serial.print("At this time: ");
  printDateTime(now());
  Serial.println("!!! WARNING !!!");
  doSOS();
}


void dot() {
  tone(buzzerPin, buzzerTone, dotDuration);
  delay(dotDuration);
  noTone(buzzerPin);
  delay(dotDuration);
}

void dash() {
  tone(buzzerPin, buzzerTone, dotDuration * 3);
  delay(dotDuration * 3);
  noTone(buzzerPin);
  delay(dotDuration);
}

void doSOS() {
  dot();
  dot();
  dot();
  delay(dotDuration * 3);
  dash();
  dash();
  dash();
  delay(dotDuration * 3);
  dot();
  dot();
  dot();
}