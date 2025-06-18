#include "SensorUtils.h"
#include "DisplayUtils.h"
#include "AlarmUtils.h"
#include "SerialComm.h"

// Sensor configuration
#define SENSOR_A_SENSITIVITY 660.0f
#define SENSOR_B_SENSITIVITY 547.0f
#define UTE_FREQUENCY 50.0

// Pin definitions
const int sensorAPin = A0; // Analog pin for first ZMPT101B
const int sensorBPin = A1; // Analog pin for second ZMPT101B

// Sensor objects
ZMPT101B voltageSensorA(sensorAPin, UTE_FREQUENCY);
ZMPT101B voltageSensorB(sensorBPin, UTE_FREQUENCY);

// Thresholds and timing
const float VOLTAGE_THRESHOLD = 180.0;                                // Only report as energized above 180V
const unsigned long READING_INTERVAL = 5;                            // Read every X seconds
const unsigned long LONG_RUN_THRESHOLD = 900;                         // X minute threshold for long run warning (in seconds)
const unsigned long ALERT_REPEAT_INTERVAL = LONG_RUN_THRESHOLD / 2UL; // Repeat alert every X minutes

// State tracking variables
bool sensorAEnergized = false;
bool sensorBEnergized = false;
time_t sensorAStartTime = 0;
time_t sensorBStartTime = 0;
time_t lastReadingTime = 0;
bool sensorALongRunReported = false;
bool sensorBLongRunReported = false;
bool firstRun = true;
time_t sensorALastAlertTime = 0;
time_t sensorBLastAlertTime = 0;

void initSensors()
{
    // Set the sensitivity value (must be calibrated)
    voltageSensorA.setSensitivity(SENSOR_A_SENSITIVITY);
    voltageSensorB.setSensitivity(SENSOR_B_SENSITIVITY);

    Serial.print("Sensitivity: Sensor A: ");
    Serial.print(SENSOR_A_SENSITIVITY);
    Serial.print(" Sensor B: ");
    Serial.println(SENSOR_B_SENSITIVITY);
    Serial.print("LONG_RUN_THRESHOLD at ");
    Serial.print(LONG_RUN_THRESHOLD / 60);
    Serial.println(" minutes");
    Serial.println("=======================================");
}

float readVoltage(int sensor)
{
    // Read RMS voltage with 5 samples for better accuracy
    if (sensor == sensorAPin)
    {
        return voltageSensorA.getRmsVoltage(5);
    }
    else if (sensor == sensorBPin)
    {
        return voltageSensorB.getRmsVoltage(5);
    }
    return 0.0;
}

void displayCurrentReadings(float voltageA, float voltageB)
{
    // Only display if either sensor is energized or for debugging
    if (sensorAEnergized || sensorBEnergized)
    {
        Serial.println("======= Sensor reading =======");
        Serial.print("Time: ");
        printDateTime(now());
        Serial.print("Sensor A: ");
        Serial.print(voltageA, 1);
        Serial.print("V");
        if (sensorAEnergized)
            Serial.print(" [ON]");
        else
            Serial.print(" [OFF]");
        Serial.println();
        Serial.print("Sensor B: ");
        Serial.print(voltageB, 1);
        Serial.print("V");
        if (sensorBEnergized)
            Serial.print(" [ON]");
        else
            Serial.print(" [OFF]");
        Serial.println();
    }
}

void processSensorState(int sensorNum, float voltage, bool &isEnergized, time_t &startTime,
                        time_t currentTime, bool &longRunReported)
{
    bool aboveThreshold = (voltage > VOLTAGE_THRESHOLD);
    char sensorId = (sensorNum == sensorAPin) ? 'A' : 'B';

    // Calculate duration the sensor has been energized (in seconds)
    unsigned long durationMinutes = 0;
    if (isEnergized && startTime > 0)
    {
        durationMinutes = (currentTime - startTime) / 60;
    }

    // Special handling for initial state detection
    if (firstRun && aboveThreshold)
    {
        return; // Skip the rest of the processing for first run
    }

    if (aboveThreshold && !isEnergized)
    {
        // Sensor just became energized
        isEnergized = true;
        longRunReported = false; // Reset the long run flag
        startTime = currentTime;

        // Send update to ESP32 - duration is 0 as it just started
        sendSensorUpdate(sensorId, true, voltage, false, 0);

        Serial.print("*** Sensor ");
        Serial.print(sensorId);
        Serial.print(" ENERGIZED at ");
        Serial.print(voltage, 2);
        Serial.print("V at ");
        printDateTime(currentTime);
    }
    else if (!aboveThreshold && isEnergized)
    {
        // Sensor just became de-energized
        isEnergized = false;
        longRunReported = false; // Reset the long run flag

        // Send update to ESP32 with final duration
        sendSensorUpdate(sensorId, false, voltage, false, durationMinutes);

        startTime = 0; // Reset start time
        // Report energization time
        Serial.println("========================================");
        Serial.print(">>> Sensor ");
        Serial.print(sensorId);
        Serial.println(" DE-ENERGIZED <<<");
        Serial.print("Duration: ");
        printDuration(durationMinutes);
        Serial.print("Final voltage: ");
        Serial.print(voltage, 2);
        Serial.println("V");
        Serial.println("========================================");
    }

    time_t &lastAlertTime = (sensorNum == sensorAPin) ? sensorALastAlertTime : sensorBLastAlertTime;

    // Check for initial long run condition
    if (isEnergized && ((unsigned long)(currentTime - startTime) > LONG_RUN_THRESHOLD) && !longRunReported)
    {
        // Send update to ESP32 with alarm and current duration
        sendSensorUpdate(sensorId, true, voltage, false, durationMinutes);

        longRun(sensorNum, durationMinutes);
        longRunReported = true;      // Prevent repeated immediate reports
        lastAlertTime = currentTime; // Set initial alert time
    }

    // Check for repeating alerts if already in long-run state
    if (isEnergized && longRunReported &&
        ((unsigned long)(currentTime - lastAlertTime) >= ALERT_REPEAT_INTERVAL))
    {

        // Send update to ESP32 with repeated alarm and current duration
        sendSensorUpdate(sensorId, true, voltage, true, durationMinutes);

        Serial.println("!!! REPEAT WARNING !!!");
        Serial.print("Sensor ");
        Serial.print(sensorId);
        Serial.print(" has been running for ");
        printDuration(durationMinutes);
        Serial.print("At this time: ");
        printDateTime(currentTime);
        Serial.println("!!! REPEAT WARNING !!!");
        doSOS();
        lastAlertTime = currentTime; // Update the last alert time
    }
}

void longRun(int sensorNum, unsigned long duration)
{
    Serial.println("!!! WARNING !!!");
    Serial.print("Sensor ");
    Serial.print((sensorNum == sensorAPin) ? 'A' : 'B');
    Serial.println(" has been running for more than ");
    Serial.print(LONG_RUN_THRESHOLD / 60);
    Serial.print(" minutes.");
    Serial.print("Current duration: ");
    printDuration(duration);
    Serial.print("At this time: ");
    printDateTime(now());
    Serial.println("!!! WARNING !!!");
    doSOS();
}

void processReadings(time_t currentTime)
{
    if (currentTime - lastReadingTime >= READING_INTERVAL)
    {
        float voltageA = readVoltage(sensorAPin);
        float voltageB = readVoltage(sensorBPin);

        // Process Sensor A
        processSensorState(sensorAPin, voltageA, sensorAEnergized, sensorAStartTime,
                           currentTime, sensorALongRunReported);
        // Process Sensor B
        processSensorState(sensorBPin, voltageB, sensorBEnergized, sensorBStartTime,
                           currentTime, sensorBLongRunReported);

        displayCurrentReadings(voltageA, voltageB);
        lastReadingTime = currentTime;

        if (firstRun)
        {
            firstRun = false;
        }
    }
}