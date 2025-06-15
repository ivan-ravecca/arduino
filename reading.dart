#include <ZMPT101B.h>

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
const unsigned long READING_INTERVAL = 10000;  // Read every 10 seconds
const unsigned long LONG_RUN_THRESHOLD = 600000;  // 10 minute threshold for long run warning

// State tracking variables
bool sensorAEnergized = false;
bool sensorBEnergized = false;
unsigned long sensorAStartTime = 0;
unsigned long sensorBStartTime = 0;
unsigned long lastReadingTime = 0;
bool sensorALongRunReported = false;  // Track if long run was already reported
bool sensorBLongRunReported = false;  // Track if long run was already reported
// ++++++++++++++++++++++++++++

void setup() {
    Serial.begin(9600);
    Serial.println("=======================================");
    Serial.println("++ AC Voltage Sensor Calibration ++");
    Serial.println("=======================================");

    // Set the sensitivity value (must be calibrated)
    voltageSensorA.setSensitivity(SENSOR_A_SENSITIVITY);
    voltageSensorB.setSensitivity(SENSOR_B_SENSITIVITY);

    Serial.print("Sensitivity: Sensor A: ");
    Serial.print(SENSOR_A_SENSITIVITY);
    Serial.print(" Sensor B: ");
    Serial.println(SENSOR_B_SENSITIVITY);
    Serial.print("LONG_RUN_THRESHOLD at ");
    Serial.print(LONG_RUN_THRESHOLD/1000/60);
    Serial.println(" minutes");
    Serial.println("================== BEGIN =====================");

    // Allow sensor to stabilize
    delay(2000);
}

void loop() {
    // readMultipleSamples(sensorAPin);
    // readMultipleSamples(sensorBPin);
    // Serial.println("---------------------------------");
    // delay(1000); // Read every second

    unsigned long currentTime = millis();

    if (currentTime - lastReadingTime >= READING_INTERVAL) {

        float voltageA = readVoltage(sensorAPin);
        float voltageB = readVoltage(sensorBPin);

        // Process Sensor 1
        processSensorState(sensorAPin, voltageA, sensorAEnergized, sensorAStartTime, currentTime, sensorALongRunReported);
        // Process Sensor 2
        processSensorState(sensorBPin, voltageB, sensorBEnergized, sensorBStartTime, currentTime, sensorBLongRunReported);
    
        displayCurrentReadings(voltageA, voltageB);
        lastReadingTime = currentTime;
    }
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


void processSensorState(int sensorNum, float voltage, bool &isEnergized, unsigned long &startTime, 
                        unsigned long currentTime, bool &longRunReported) {
  bool aboveThreshold = (voltage > VOLTAGE_THRESHOLD);
  
  if (aboveThreshold && !isEnergized) {
    // Sensor just became energized
    isEnergized = true;
    longRunReported = false; // Reset the long run flag
    startTime = currentTime;
    Serial.print("*** Sensor ");
    Serial.print((sensorNum == sensorAPin) ? 'A' : 'B');
    Serial.print(" ENERGIZED at ");
    Serial.print(voltage, 2);
    Serial.println("V ***");
    
  } else if (!aboveThreshold && isEnergized) {
    // Sensor just became de-energized
    isEnergized = false;
    longRunReported = false; // Reset the long run flag
    unsigned long energizedDuration = currentTime - startTime;
    
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
  
  // Check for long run condition milliseconds
if (isEnergized && (currentTime - startTime > LONG_RUN_THRESHOLD) && !longRunReported) {
    unsigned long runningDuration = currentTime - startTime;
    longRun(sensorNum, runningDuration);
    longRunReported = true; // Prevent repeated reports
  }
}

void printDuration(unsigned long duration) {
  unsigned long seconds = duration / 1000;
  unsigned long milliseconds = duration % 1000;
  unsigned long minutes = seconds / 60;
  seconds = seconds % 60;
  
  if (minutes > 0) {
    Serial.print(minutes);
    Serial.print(" min ");
  }
  if (seconds > 0) {
    Serial.print(seconds);
    Serial.print(" sec ");
  }
  Serial.print(milliseconds);
  Serial.println(" ms");
}

void longRun(int sensorNum, unsigned long duration) {
  Serial.println("!!! WARNING !!!");
  Serial.print("Sensor ");
  Serial.print((sensorNum == sensorAPin) ? 'A' : 'B');
  Serial.println(" has been running for more than ");
  Serial.print(LONG_RUN_THRESHOLD/1000);
  Serial.print(" seconds.");
  Serial.print("Current duration: ");
  printDuration(duration);
  Serial.println("!!! WARNING !!!");
}