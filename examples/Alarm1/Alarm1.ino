/**
 * This example shows how to setup an alarm to trigger every
 * second on DS3231. There are more types of alarms, but the
 * usage is pretty much the same and you can easily figure out
 * how to use them if you can understand this example.
 *
 * In this example, the built-in led (pin 13) will blink once
 * per second.
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
 * | 2       | SQW    |
 * | GND     | GND    |
 * | 5V      | VCC    |
 * +---------+--------+
 */
#include <Arduino.h>
#include <Wire.h>
#include "Alarm1.h"

//All library classes are inside namespaces.
//Therefore, use the following statement to import them.
using namespace Ampliar::DS3231;

//This statement creates an instance of Alarm1, used to interface
//with DS3231 first alarm.
Alarm1 alarm;

int pin = 13; //Lets use Arduino built-in led.
volatile int state = LOW; //Starts with the led turned off.

/**
 * This function will be called every time a hardware
 * interruption heapens.
 */
void alarmTriggered()
{
    state = !state;
}

void setup()
{
    pinMode(pin, OUTPUT);

    //It turns on the alarm and it enables the hardware interruption
    alarm.turnOn(true);

    //Setup the alarm to trigger every second
    alarm.writeAlarmOncePerSecond();

    //Prepare Arduino to listen for hardware interruptions. Here, I'm using
    //the digital pin 2. Check the Arduino documentation for the list of
    //digital pins usable for interrupts. See more at:
    //https://www.arduino.cc/en/Reference/AttachInterrupt
    attachInterrupt(digitalPinToInterrupt(2), alarmTriggered, FALLING);
}

void loop()
{
    //You must ALWAYS call this method when the alarm triggers. The only
    //exception is when you use wasItTriggered() method, because it
    //already clears the alarm flag.
    alarm.clearAlarmFlag();

    digitalWrite(pin, state);
}
