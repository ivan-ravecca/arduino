#ifndef SERIAL_COMM_H
#define SERIAL_COMM_H

#include <Arduino.h>
#include <SoftwareSerial.h>
#include <TimeLib.h>

// External variables
extern SoftwareSerial espSerial;

// Function declarations
void initSerialComm();
void requestTimeFromESP32();
void sendSensorUpdate(char sensorId, bool isEnergized, float voltage, bool isRepeatedAlarm, unsigned long durationMinutes);
bool handleIncomingMessages();
void processSyncTimeMessage(String timeStr);

#endif