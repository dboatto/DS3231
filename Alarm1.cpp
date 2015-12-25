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
#include "Alarm1.h"

using namespace Upscale::DS3231;
using namespace Upscale::BinaryHelper;

bool Alarm1::isOn() const
{
    Wire.beginTransmission(RTC_ADDR_I2C);
    Wire.write(RTC_ADDR_CONTROL);
    Wire.endTransmission();

    Wire.requestFrom(RTC_ADDR_I2C, 1);
    uint8_t controlRegister = Wire.read();

    return isBitSet(controlRegister, RTC_REG_CONTROL_A1IE) && isBitSet(controlRegister, RTC_REG_CONTROL_INTCN);
}

void Alarm1::turnOn() const
{
    uint8_t controlRegister = readRegister(RTC_ADDR_CONTROL);
    setBitOn(controlRegister, RTC_REG_CONTROL_INTCN);
    setBitOn(controlRegister, RTC_REG_CONTROL_A1IE);
    writeRegister(RTC_ADDR_CONTROL, controlRegister);
}

void Alarm1::turnOff() const
{
    uint8_t controlRegister = readRegister(RTC_ADDR_CONTROL);
    setBitOff(controlRegister, RTC_REG_CONTROL_INTCN);
    setBitOff(controlRegister, RTC_REG_CONTROL_A1IE);
    writeRegister(RTC_ADDR_CONTROL, controlRegister);
}

bool Alarm1::wasItTriggered() const
{
    uint8_t statusRegister = readRegister(RTC_ADDR_STATUS);
    bool triggered = isBitSet(statusRegister, RTC_REG_STATUS_A1F);

    //If it was triggered, it is necessary to reset it
    if (triggered)
    {
        setBitOff(statusRegister, RTC_REG_STATUS_A1F);
        writeRegister(RTC_ADDR_STATUS, statusRegister);
    }

    return triggered;
}

void Alarm1::readAlarm()
{
    Wire.beginTransmission(RTC_ADDR_I2C);
    Wire.write(RTC_ADDR_ALARM1);
    Wire.endTransmission();

    //Gets raw data
    Wire.requestFrom(RTC_ADDR_I2C, 4);
    _second    = Wire.read();
    _minute    = Wire.read();
    _hour      = Wire.read();
    _day       = Wire.read();

    //Gets the flags used to determine the alarm rate
    bool a1m1        = isBitSet(_second, RTC_ALARM1_A1M1);
    bool a1m2        = isBitSet(_minute, RTC_ALARM1_A1M2);
    bool a1m3        = isBitSet(_hour  , RTC_ALARM1_A1M3);
    bool a1m4        = isBitSet(_day   , RTC_ALARM1_A1M4);
    bool isDayOfWeek = isBitSet(_day   , RTC_ALARM1_DYDT);

    //It figures out the alarm rate
    if (a1m1 && a1m2 && a1m3 && a1m4)
    {
        _alarmRate = ONCE_PER_SECOND;
    }
    else if (!a1m1 && a1m2 && a1m3 && a1m4)
    {
        _alarmRate = WHEN_SECONDS_MATCH;
    }
    else if (!a1m1 && !a1m2 && a1m3 && a1m4)
    {
        _alarmRate = WHEN_SECONDS_AND_MINUTES_MATCH;
    }
    else if (!a1m1 && !a1m2 && !a1m3 && a1m4)
    {
        _alarmRate = WHEN_SECONDS_AND_MINUTES_AND_HOURS_MATCH;
    }
    else if (!a1m1 && !a1m2 && !a1m3 && !a1m4 && !isDayOfWeek)
    {
        _alarmRate = WHEN_SECONDS_AND_MINUTES_AND_HOURS_AND_DAY_MATCH;
    }
    else if (!a1m1 && !a1m2 && !a1m3 && !a1m4 && isDayOfWeek)
    {
        _alarmRate = WHEN_SECONDS_AND_MINUTES_AND_HOURS_AND_DAY_OF_WEEK_MATCH;
    }

    //Turns off the flags before getting the values in a base-10 representation
    setBitOff(_second, RTC_ALARM1_A1M1);
    setBitOff(_minute, RTC_ALARM1_A1M2);
    setBitOff(_hour  , RTC_ALARM1_A1M3);
    setBitOff(_day   , RTC_ALARM1_A1M4);
    setBitOff(_day   , RTC_ALARM1_DYDT);

    //Gets the actual values
    _second = fromBcdToDecimal(_second);
    _minute = fromBcdToDecimal(_minute);
    _hour   = fromBcdToDecimal(_hour);
    _day    = fromBcdToDecimal(_day);

    /* This device uses the same place to store the the day of the month and the
     * day of the week. Therefore, we need to figure out what the value in _day is.
     */
    if (isDayOfWeek)
    {
        _dayOfWeek = _day;
        _day = 0;
    }
    else
    {
        _dayOfWeek = 0;
    }
}

void Alarm1::writeAlarmOncePerSecond()
{
    _second    = 0;
    _minute    = 0;
    _hour      = 0;
    _day       = 0;
    _dayOfWeek = 0;
    _alarmRate = ONCE_PER_SECOND;

    Wire.beginTransmission(RTC_ADDR_I2C);
    Wire.write(RTC_ADDR_ALARM1);
    Wire.write(0x80); //A1M1
    Wire.write(0x80); //A1M2
    Wire.write(0x80); //A1M3
    Wire.write(0x80); //A1M4
    Wire.endTransmission();
}

void Alarm1::writeAlarm(uint8_t second)
{
    _second    = second;
    _minute    = 0;
    _hour      = 0;
    _day       = 0;
    _dayOfWeek = 0;
    _alarmRate = WHEN_SECONDS_MATCH;

    second = fromDecimalToBcd(second);

    Wire.beginTransmission(RTC_ADDR_I2C);
    Wire.write(RTC_ADDR_ALARM1);
    Wire.write(second);  //A1M1
    Wire.write(0x80);    //A1M2
    Wire.write(0x80);    //A1M3
    Wire.write(0x80);    //A1M4
    Wire.endTransmission();
}

void Alarm1::writeAlarm(uint8_t minute, uint8_t second)
{
    _second    = second;
    _minute    = minute;
    _hour      = 0;
    _day       = 0;
    _dayOfWeek = 0;
    _alarmRate = WHEN_SECONDS_AND_MINUTES_MATCH;

    second = fromDecimalToBcd(second);
    minute = fromDecimalToBcd(minute);

    Wire.beginTransmission(RTC_ADDR_I2C);
    Wire.write(RTC_ADDR_ALARM1);
    Wire.write(second);  //A1M1
    Wire.write(minute);  //A1M2
    Wire.write(0x80);    //A1M3
    Wire.write(0x80);    //A1M4
    Wire.endTransmission();
}

void Alarm1::writeAlarm(uint8_t hour, uint8_t minute, uint8_t second)
{
    _second    = second;
    _minute    = minute;
    _hour      = hour;
    _day       = 0;
    _dayOfWeek = 0;
    _alarmRate = WHEN_SECONDS_AND_MINUTES_AND_HOURS_MATCH;

    second = fromDecimalToBcd(second);
    minute = fromDecimalToBcd(minute);
    hour   = fromDecimalToBcd(hour);

    Wire.beginTransmission(RTC_ADDR_I2C);
    Wire.write(RTC_ADDR_ALARM1);
    Wire.write(second);  //A1M1
    Wire.write(minute);  //A1M2
    Wire.write(hour);    //A1M3
    Wire.write(0x80);    //A1M4
    Wire.endTransmission();
}

void Alarm1::writeAlarm(bool useDayOfWeek, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second)
{
    _second    = second;
    _minute    = minute;
    _hour      = hour;
    _day       = useDayOfWeek ? 0            : day;
    _dayOfWeek = useDayOfWeek ? useDayOfWeek : 0 ;
    _alarmRate = useDayOfWeek
               ? WHEN_SECONDS_AND_MINUTES_AND_HOURS_AND_DAY_OF_WEEK_MATCH
               : WHEN_SECONDS_AND_MINUTES_AND_HOURS_AND_DAY_MATCH;

    second = fromDecimalToBcd(second);
    minute = fromDecimalToBcd(minute);
    hour   = fromDecimalToBcd(hour);
    day    = fromDecimalToBcd(day);

    if (useDayOfWeek)
    {
        setBitOn(day, RTC_ALARM1_DYDT);
    }

    Wire.beginTransmission(RTC_ADDR_I2C);
    Wire.write(RTC_ADDR_ALARM1);
    Wire.write(second);  //A1M1
    Wire.write(minute);  //A1M2
    Wire.write(hour);    //A1M3
    Wire.write(day);     //A1M4
    Wire.endTransmission();
}

uint8_t Alarm1::getSecond() const
{
    return _second;
}

uint8_t Alarm1::getMinute() const
{
    return _minute;
}

uint8_t Alarm1::getHour() const
{
    return _hour;
}

uint8_t Alarm1::getDay() const
{
    return _day;
}

uint8_t Alarm1::getDayOfWeek() const
{
    return _dayOfWeek;
}

uint8_t Alarm1::getAlarmRate() const
{
    return _alarmRate;
}
