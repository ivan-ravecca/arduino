/*
 * ZMPT101B Calibration Code
 * Use this to find the correct sensitivity value for your sensor
 */

#include <ZMPT101B.h>

#define ACTUAL_VOLTAGE 212.0f     // Set this to your actual AC voltage (measured with multimeter)
#define DEFAULT_SENSITIVITY 648.0f // Default sensitivity value from library

ZMPT101B voltageSensor(A1, 50.0);  // Pin A0, 60Hz

void setup() {
  Serial.begin(9600);
  Serial.println("ZMPT101B Calibration");
  Serial.println("===================");
  Serial.print("Actual Voltage: ");
  Serial.print(ACTUAL_VOLTAGE);
  Serial.println(" V");
  Serial.println("===================");
  
  // Start with default sensitivity
  voltageSensor.setSensitivity(DEFAULT_SENSITIVITY);
  
  delay(2000);
  
  // Calibration process
  Serial.println("Calibrating... Please wait");
  Serial.print("Using default sensitivity: ");
  Serial.println(DEFAULT_SENSITIVITY);
  
  float sumVoltage = 0;
  int samples = 100;
  
  // Take multiple readings
  for(int i = 0; i < samples; i++) {
    float reading = voltageSensor.getRmsVoltage();
    sumVoltage += reading;
    Serial.print("Reading ");
    Serial.print(i+1);
    Serial.print(": ");
    Serial.print(reading, 2);
    Serial.println(" V");
    delay(100);
  }
  
  float averageReading = sumVoltage / samples;
  float newSensitivity = DEFAULT_SENSITIVITY * (ACTUAL_VOLTAGE / averageReading);
  
  Serial.println("===================");
  Serial.print("Average Reading: ");
  Serial.print(averageReading, 2);
  Serial.println(" V");
  Serial.print("Calculated Sensitivity: ");
  Serial.println(newSensitivity, 2);
  Serial.println("===================");
  Serial.println("Copy this sensitivity value to your main code!");
}

void loop() {
  // Empty - calibration runs once in setup
}