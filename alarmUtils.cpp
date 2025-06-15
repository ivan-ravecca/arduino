#include "alarmUtils.h"

const int buzzerPin = 8;
const int dotDuration = 200;
const int buzzerTone = 3500;

void initBuzzer()
{
    pinMode(buzzerPin, OUTPUT);
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