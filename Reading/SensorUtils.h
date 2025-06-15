#ifndef SENSOR_UTILS_H
#define SENSOR_UTILS_H

#include <Arduino.h>
#include <ZMPT101B.h>
#include <TimeLib.h>

// Constants
extern const float VOLTAGE_THRESHOLD;
extern const unsigned long READING_INTERVAL;
extern const unsigned long LONG_RUN_THRESHOLD;
extern const unsigned long ALERT_REPEAT_INTERVAL;
extern const int sensorAPin;
extern const int sensorBPin;

// Function declarations
void initSensors();
float readVoltage(int sensor);
void displayCurrentReadings(float voltageA, float voltageB);
void processSensorState(int sensorNum, float voltage, bool &isEnergized,
                        time_t &startTime, time_t currentTime, bool &longRunReported);
void longRun(int sensorNum, unsigned long duration);
void processReadings(time_t currentTime);

#endif