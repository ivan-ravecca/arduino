#ifndef SENSOR_UTILS_H
#define SENSOR_UTILS_H

#include <Arduino.h>
#include <ZMPT101B.h>
#include <TimeLib.h>

// Constants
extern const float VOLTAGE_THRESHOLD;
extern const unsigned long READING_INTERVAL;
extern const unsigned long LONG_RUN_THRESHOLD;
extern const int sensorAPin;
extern const int sensorBPin;

// Variables
extern bool sensorAEnergized;
extern bool sensorBEnergized;
extern time_t sensorAStartTime;
extern time_t sensorBStartTime;
extern time_t lastReadingTime;
extern bool sensorALongRunReported;
extern bool sensorBLongRunReported;
extern bool firstRun;

// Functions
void initSerial();
void initSensors();
float readVoltage(int sensor);
void processSensorState(int sensorNum, float voltage, bool &isEnergized,
                        time_t &startTime, time_t currentTime, bool &longRunReported);
void processSensorReadings(time_t currentTime);
void displayCurrentReadings(float voltageA, float voltageB);
void longRun(int sensorNum, unsigned long duration);

#endif