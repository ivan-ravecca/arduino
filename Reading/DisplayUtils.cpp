#include "DisplayUtils.h"

// Function to print current date and time
void printDateTime(time_t t)
{
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

void printDuration(unsigned long durationMinutes)
{
    unsigned long hours = durationMinutes / 60;
    unsigned long minutes = durationMinutes % 60;

    if (hours > 0)
    {
        Serial.print(hours);
        Serial.print(" hr ");
    }
    if (minutes > 0)
    {
        Serial.print(minutes);
        Serial.print(" min");
    }
    else if (hours == 0)
    {
        Serial.print("0 min");
    }
    Serial.println();
}