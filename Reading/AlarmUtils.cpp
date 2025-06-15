#include "alarmUtils.h"

// Constants
const int buzzerPin = 8;     // Digital pin connected to the buzzer
const int dotDuration = 200; // Duration of a dot in milliseconds
const int buzzerTone = 3500; // Frequency of the buzzer tone in Hz

void initAlarm()
{
    pinMode(buzzerPin, OUTPUT);
    Serial.println("Alarm system initialized");
}

void dot()
{
    tone(buzzerPin, buzzerTone, dotDuration);
    delay(dotDuration);
    noTone(buzzerPin);
    delay(dotDuration);
}

void dash()
{
    tone(buzzerPin, buzzerTone, dotDuration * 3);
    delay(dotDuration * 3);
    noTone(buzzerPin);
    delay(dotDuration);
}

void doSOS()
{
    // SOS pattern: ... --- ...
    Serial.println("Playing SOS pattern");

    // S (...)
    dot();
    dot();
    dot();

    delay(dotDuration * 3); // Letter spacing

    // O (---)
    dash();
    dash();
    dash();

    delay(dotDuration * 3); // Letter spacing

    // S (...)
    dot();
    dot();
    dot();

    delay(dotDuration * 7); // End of message spacing
}