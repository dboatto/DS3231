/*
 * Copyright 2015 Daniel Murari Boatto
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "RealTimeClock.h"

/**
 * Check if the clock was stopped.
 *
 * This device contains an special flag to indicate if the clock was stopped for any reason. This method checks this
 * flag and return a boolean value indicating whether it was stopped or not.
 *
 * The oscillator (clock) may stop upon any of these conditions:
 *
 * -# Initial application of power;
 * -# The voltages present on both V CC and V BAT are insufficient to support oscillation;
 * -# EOSC bit was set to 1, stopping the oscillator;
 * -# External influences upon the crystal (leakage, coupling, etc.).
 *
 * \b Note: You should only trust on the date/time read if this method returns false.
 * @return true if the clock is stopped or false, otherwise.
 */
bool RealTimeClock::wasItStopped()
{
    return BinaryHelper::istBitSet(readRegister(RTC_ADDR_STATUS), RTC_REG_STATUS_OSF);
}

/**
 * Stores a given date/time on the device.
 *
 * This method is used to store a given date/time on the device.
 *
 * \b Note: The oscillator (clock) will be active as long as power is being supplied. If you wish to keep the
 * oscillator on, even when your board is turned off, you must activate the battery.
 *
 * @see RealTimeClockController::enableBattery().
 *
 * @param year Year in yyyy format
 * @param month Month (from 1 to 12)
 * @param day Day of the month (from 1 to 31)
 * @param hour Hour in 24-hour format (from 0 to 23)
 * @param minute Minute (from 0 to 59)
 * @param second Second (from 0 to 59)
 */
void RealTimeClock::writeDateTime(int16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second)
{
    uint8_t century = 0;

    _second    = second;
    _minute    = minute;
    _hour      = hour;
    _day       = day;
    _month     = month;
    _year      = year;
    _dayOfWeek = calculateDayOfWeek(year, month, day) + 1;

    if (year > 2000)
    {
        century = 0x80;
        year -= 2000;
    }
    else
    {
        year -= 1900;
    }

    Wire.beginTransmission(RTC_ADDR_I2C);
    Wire.write(RTC_ADDR_DATE);
    Wire.write(BinaryHelper::fromDecimalToBcd(second));
    Wire.write(BinaryHelper::fromDecimalToBcd(minute));
    Wire.write(BinaryHelper::fromDecimalToBcd(hour));
    Wire.write(BinaryHelper::fromDecimalToBcd(_dayOfWeek));
    Wire.write(BinaryHelper::fromDecimalToBcd(day));
    Wire.write(BinaryHelper::fromDecimalToBcd(month) | century);
    Wire.write(BinaryHelper::fromDecimalToBcd(year));
    Wire.endTransmission();

    clearOscillatorStopFlag();
}

/**
 * Reads the date/time from the device.
 *
 * This method reads the time and calendar information from appropriate registers. It stores the values in this object
 * and make them available through getters, like getYear(), getMonth(), getHour() etc.
 *
 * \b Note:
 * - You must call this method before using any getter related to date/time, otherwise they will return zero.
 * - Use the method wasItStopped() to determine if you can thrust on the date/time read from the device.
 */
void RealTimeClock::readDateTime()
{
    Wire.beginTransmission(RTC_ADDR_I2C);
    Wire.write(RTC_ADDR_DATE);
    Wire.endTransmission();

    Wire.requestFrom(RTC_ADDR_I2C, 7);
    _second    = BinaryHelper::fromBcdToDecimal(Wire.read());
    _minute    = BinaryHelper::fromBcdToDecimal(Wire.read());
    _hour      = BinaryHelper::fromBcdToDecimal(Wire.read());
    _dayOfWeek = BinaryHelper::fromBcdToDecimal(Wire.read());
    _day       = BinaryHelper::fromBcdToDecimal(Wire.read());

    uint8_t monthAndCentury = Wire.read();

    _month = BinaryHelper::fromBcdToDecimal(monthAndCentury & 0x1F);
    _year  = BinaryHelper::fromBcdToDecimal(Wire.read());
    _year += ((monthAndCentury & 0x80) != 0 ? 2000 : 1900);
}

/**
 * Clears the EOSC flag
 *
 * This device contains an oscillator stop flag (OSF), allowing us to identify if the oscillator (the clock) stopped.
 * The oscillator can stop based upon any of these conditions:
 *
 * -# Initial application of power;
 * -# The voltages present on both V CC and V BAT are insufficient to support oscillation;
 * -# EOSC bit was set to 1, stopping the oscillator;
 * -# External influences upon the crystal (leakage, coupling, etc.).
 *
 * Every time we set a new date/time, it is necessary to clear this flag.
 */
void RealTimeClock::clearOscillatorStopFlag()
{
    uint8_t statusRegister = readRegister(RTC_ADDR_STATUS);
    statusRegister = BinaryHelper::setBitOff(statusRegister, RTC_REG_STATUS_OSF);
    writeRegister(RTC_ADDR_STATUS, statusRegister);
}

/**
 * Calculates the day of the week based.
 *
 * This is small and very efficient algorithm, created by Tomohiko Sakamoto, to calculate the day of the week, base on
 * a given date.
 *
 * You may find more details about how it works at: https://www.quora.com/How-does-Tomohiko-Sakamotos-Algorithm-work
 *
 * @param year Year with century (format yyyy). Examples: 1995, 2007, etc.
 * @param month Month (from 1 to 12)
 * @param day Day of month (from 1 to 31)
 * @return the day of the week from (0 to 6)
 */
uint8_t RealTimeClock::calculateDayOfWeek(int16_t year, uint8_t month, uint8_t day)
{
    static uint8_t t[] = {0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4};
    year -= month < 3;
    return (year + year/4 - year/100 + year/400 + t[month-1] + day) % 7;
}

/**
 * Gets seconds (from 0 to 59)
 *
 * \b Note: You must call readDateTime() before using this method, otherwise it will return 0.
 *
 * @return second (from 0 to 59)
 */
uint8_t RealTimeClock::getSecond() const
{
    return _second;
}

/**
 * Gets minutes (from 0 to 59)
 *
 * \b Note: You must call readDateTime() before using this method, otherwise it will return 0.
 *
 * @return minute (from 0 to 59)
 */
uint8_t RealTimeClock::getMinute() const
{
    return _minute;
}

/**
 * Gets hours in 24-hour format (from 0 to 23)
 *
 * \b Note: You must call readDateTime() before using this method, otherwise it will return 0.
 *
 * @return hour (from 0 to 23)
 */
uint8_t RealTimeClock::getHour() const
{
    return _hour;
}

/**
 * Gets the day of the month (from 1 to 31)
 *
 * \b Note: You must call readDateTime() before using this method, otherwise it will return 0.
 *
 * @return day of the month (from 1 to 31)
 */
uint8_t RealTimeClock::getDay() const
{
    return _day;
}

/**
 * Gets the month (from 1 to 12)
 *
 * \b Note: You must call readDateTime() before using this method, otherwise it will return 0.
 *
 * @return month (from 1 to 12)
 */
uint8_t RealTimeClock::getMonth() const
{
    return _month;
}

/**
 * Gets the day of week (from 1 to 7)
 *
 * \b Note: You must call readDateTime() before using this method, otherwise it will return 0.
 *
 * @return day of week (from 1 to 7)
 */
uint8_t RealTimeClock::getDayOfWeek() const
{
    return _dayOfWeek;
}

/**
 * Gets the year in yyyy format.
 *
 * Gets the year with century information. For instance: 1995, 2015, etc.
 *
 * \b Note: You must call readDateTime() before using this method, otherwise it will return 0.
 *
 * @return year in yyyy format.
 */
int16_t RealTimeClock::getYear() const
{
    return _year;
}

/**
 * Forces the device to update the temperature.
 *
 * This method forces the temperature sensor to convert the temperature into digital code and execute the TCXO
 * algorithm to update the capacitance array to the oscillator.
 *
 * Since an update can only happen when a conversion is not already in progress, this method returns true if the
 * command is successful or false, otherwise.
 *
 * @return true if successful or false, otherwise.
 */
bool RealTimeClock::forceTemperatureUpdate()
{
    uint8_t statusRegister = readRegister(RTC_ADDR_STATUS);
    if (BinaryHelper::istBitSet(statusRegister, RTC_REG_STATUS_BSY))
    {
        return false;
    }
    uint8_t controlRegister = readRegister(RTC_ADDR_CONTROL);
    controlRegister = BinaryHelper::setBitOn(controlRegister, RTC_REG_CONTROL_CONV);
    writeRegister(RTC_ADDR_CONTROL, controlRegister);
    return true;
}

/**
 * Reads current temperature in degrees Celsius
 *
 * This method reads the temperature from the temperature registers in degrees Celsius. Bear in mind that these
 * registers are updated every 64-seconds or after forceTemperatureUpdate() is called. Therefore, if you try
 * to call this method multiple times, within 64-seconds (without forcing temperature update), the result will be
 * the same.
 *
 * The resolution of this device is 0.25ºC.
 *
 * @return temperature in degrees Celsius.
 */
float RealTimeClock::readTemperature()
{
    uint8_t decimalPart, resolution;
    float temperature;

    Wire.beginTransmission(RTC_ADDR_I2C);
    Wire.write(RTC_ADDR_TEMPERATURE);
    Wire.endTransmission();

    Wire.requestFrom(RTC_ADDR_I2C, 2);
    decimalPart = Wire.read();
    resolution  = Wire.read();
    resolution  = resolution >> 6; //get only the upper nibble

    temperature = (decimalPart & 0x80)            //negative (two's complement)?
                ? (decimalPart | ~((1 << 8) - 1)) //convert it to a signed int
                : decimalPart;                    //positive number

    return temperature + (resolution * 0.25);
}
