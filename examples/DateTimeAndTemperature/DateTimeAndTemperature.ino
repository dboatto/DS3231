/**
 * This example shows how to read and write date/time on DS3231 through
 * this library. Additionally, since DS3231 is able to measure the
 * temperature, this library provides a method to read it.
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
 *
 * More information: https://github.com/dboatto/DS3231
 *
 * In order to use this example, open the Serial Monitor on
 * Arduino IDE (Ctrl+Shit+M).
 */
#include <Arduino.h>
#include "RealTimeClock.h"

//All library classes are inside namespaces.
//Therefore, use the following statement to import them.
using namespace Ampliar::DS3231;

//This statement creates an instance of RealTimeClock, used to access DS3231
//basic date/time and temperature functionalities.
RealTimeClock clock;

//We gonna show the days of the week in the loop() function.
String days[] = { "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday" };

void setup()
{
    Serial.begin(9600);

    // Writes the date/time on DS3231 internal memory.
    // Usage: writeDateTime(year, month, day, hour, minute, second);
    clock.writeDateTime(2015, 12, 27, 16, 28, 0);
}

void loop()
{
    //Reads the date/time information from DS3231
    clock.readDateTime();

    Serial.print(clock.getDay());
    Serial.print("/");
    Serial.print(clock.getMonth());
    Serial.print("/");
    Serial.print(clock.getYear());
    Serial.print(" ");
    Serial.print(clock.getHour());
    Serial.print(":");
    Serial.print(clock.getMinute());
    Serial.print(":");
    Serial.print(clock.getSecond());
    Serial.print(" ");
    Serial.print(days[clock.getDayOfWeek() - 1]);
    Serial.print(" ");
    Serial.print(clock.readTemperature());
    Serial.print(" C");
    Serial.println("");

    delay (1000); //delays 1 (one) second
}
