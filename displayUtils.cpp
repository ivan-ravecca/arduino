#include "displayUtils.h"

void printDuration(unsigned long durationSeconds)
{
    unsigned long minutes = durationSeconds / 60;
    unsigned long seconds = durationSeconds % 60;
    unsigned long hours = minutes / 60;
    minutes = minutes % 60;

    if (hours > 0)
    {
        Serial.print(hours);
        Serial.print(" hr ");
    }
    if (minutes > 0)
    {
        Serial.print(minutes);
        Serial.print(" min ");
    }
    Serial.print(seconds);
    Serial.println(" sec");
}

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