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

RealTimeClock::RealTimeClock()
{
}

/*************************************************************************************************/

bool RealTimeClock::wasItStopped()
{
    return BinaryHelper::istBitSet(readRegister(RTC_ADDR_STATUS), RTC_REG_STATUS_OSF);
}

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

void RealTimeClock::clearOscillatorStopFlag()
{
    uint8_t statusRegister = readRegister(RTC_ADDR_STATUS);
    statusRegister = BinaryHelper::setBitOff(statusRegister, RTC_REG_STATUS_OSF);
    writeRegister(RTC_ADDR_STATUS, statusRegister);
}

uint8_t RealTimeClock::calculateDayOfWeek(int16_t year, uint8_t month, uint8_t day)
{
    static uint8_t t[] = {0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4};
    year -= month < 3;
    return (year + year/4 - year/100 + year/400 + t[month-1] + day) % 7;
}

uint8_t RealTimeClock::getSecond() const
{
    return _second;
}

uint8_t RealTimeClock::getMinute() const
{
    return _minute;
}

uint8_t RealTimeClock::getHour() const
{
    return _hour;
}

uint8_t RealTimeClock::getDay() const
{
    return _day;
}

uint8_t RealTimeClock::getMonth() const
{
    return _month;
}

uint8_t RealTimeClock::getDayOfWeek() const
{
    return _dayOfWeek;
}

int16_t RealTimeClock::getYear() const
{
    return _year;
}

/*************************************************************************************************/

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

float RealTimeClock::getTemperature()
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
