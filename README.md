# DS3231

Arduino library for DS3231 Real Time Clock (RTC)

This project is a comprehensive library for [Arduino](http://arduino.cc) to interface with
[Maxim DS3231](https://www.maximintegrated.com/en/products/digital/real-time-clocks/DS3231.html) real-time clock.

## About DS3231

DS3231 is a low-cost, extremely accurate I2C real-time clock (RTC) with an integrated temperature-compensated
crystal oscillator (TCXO) and crystal. The device incorporates a battery input, and maintains accurate timekeeping when
main power to the device is interrupted.

For more details, please referrer the product [datasheet] (https://datasheets.maximintegrated.com/en/ds/DS3231.pdf).

## Library Features

* Read and write date/time information.
* Read the temperature and force the temperature update.
* Full control of both alarms supported by DS3231.
* Full control of DS3231 functionalities:
    * enable/disable the battery-backed mode;
    * enable/disable an output of a 32.768 kHz square-wave signal on the correspondent pin of DS3231;
    * enable/disable the square-wave output at a given frequency;
    * enable/disable the battery-backed square-wave output;
    * calibration by setting the aging offset register.

## Where to Buy?

**USA**
* [Adafruit] (https://www.adafruit.com/product/255)

**Brazil**
* [FILIPEFLOP] (http://www.filipeflop.com/pd-1c7dbf-real-time-clock-rtc-ds3231.html)

## Documentation

All cpp files are heavily documented. If you prefer, you can generate the documentation using Doxygen. You just need to
run doxygen in the library's folder.

## Example

You can find more examples on examples folder. When you install this library, the Arduino IDE will make them available
through the menu File/Examples/Ampliar DS3231.

```c++
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
```

## License

This library is licensed under the Apache License, Version 2.0. Check the LICENSE file for more details.
