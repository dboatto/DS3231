/**
 * This example shows how to setup an alarm to trigger at
 * a specific day/hour/minute on DS3231. There are more
 * types of alarms, but the usage is pretty much the same
 * and you can easily figure out how to use them if you
 * can understand this example.
 *
 * In this example, the built-in led (pin 13) will turn
 * on after 10 seconds.
 *
 * More information: https://github.com/dboatto/DS3231
 *
 * Wiring for Arduino Uno (for other boards, check the Wire
 * Library documentation to figure out the SDA and SCL pins
 * on Arduino):
 *
 * +---------+--------+
 * | Arduino | DS3231 |
 * +---------+--------+
 * | A4      | SDA    |
 * | A5      | SCL    |
 * | GND     | GND    |
 * | 5V      | VCC    |
 * +---------+--------+
 */
#include <Arduino.h>
#include "Alarm2.h"
#include "RealTimeClock.h"

//All library classes are inside namespaces.
//Therefore, use the following statement to import them.
using namespace Ampliar::DS3231;

//This statement creates an instance of Alarm2, used to interface
//with DS3231 second alarm.
Alarm2 alarm;

//This statement creates an instance of RealTimeClock, used to access DS3231
//basic date/time and temperature functionalities.
RealTimeClock clock;

int pin = 13; //Lets use Arduino built-in led.

void setup()
{
    pinMode(pin, OUTPUT);

    // Writes the date/time on DS3231 internal memory.
    // Use: writeDateTime(year, month, day, hour, minute, second);
    clock.writeDateTime(2015, 12, 27, 21, 41, 50);

    //It turns on the second alarm
    alarm.turnOn();

    //Setups the alarm to trigger at a specific day, hour and minute.
    //In order to simulate it, we defined the date/time of DS3231
    //internal clock to a known date. In this example, the led will
    //turn on after 10 seconds.
    //
    //Usage: writeAlarm(useDayOfWeek, day, hour, minute)
    alarm.writeAlarm(false, 27, 21, 42);
}

void loop()
{
    if (alarm.wasItTriggered())
    {
        digitalWrite(pin, HIGH);
    }

    delay (1000); //delays 1 (one) second
}
