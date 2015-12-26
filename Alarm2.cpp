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
#include "Alarm2.h"

using namespace Upscale::DS3231;
using Upscale::BinaryHelper::setBitOn;
using Upscale::BinaryHelper::setBitOff;
using Upscale::BinaryHelper::isBitSet;
using Upscale::BinaryHelper::fromBcdToDecimal;
using Upscale::BinaryHelper::fromDecimalToBcd;

Alarm2::Alarm2():
    BaseAlarm(RTC_REG_CONTROL_A2IE, RTC_REG_STATUS_A2F)
{
    //
}

void Alarm2::readAlarm()
{
    Wire.beginTransmission(RTC_ADDR_I2C);
    Wire.write(RTC_ADDR_ALARM2);
    Wire.endTransmission();

    //Gets raw data
    Wire.requestFrom(RTC_ADDR_I2C, 3);
    _minute    = Wire.read();
    _hour      = Wire.read();
    _day       = Wire.read();

    //Gets the flags used to determine the alarm rate
    bool a2m2        = isBitSet(_minute, RTC_ALARM2_A2M2);
    bool a2m3        = isBitSet(_hour  , RTC_ALARM2_A2M3);
    bool a2m4        = isBitSet(_day   , RTC_ALARM2_A2M4);
    bool isDayOfWeek = isBitSet(_day   , RTC_ALARM2_DYDT);

    //It figures out the alarm rate
    if (a2m2 && a2m3 && a2m4)
    {
        _alarmRate = ONCE_PER_MINUTE;
    }
    else if (!a2m2 && a2m3 && a2m4)
    {
        _alarmRate = WHEN_MINUTES_MATCH;
    }
    else if (!a2m2 && !a2m3 && a2m4)
    {
        _alarmRate = WHEN_MINUTES_AND_HOURS_MATCH;
    }
    else if (!a2m2 && !a2m3 && !a2m4 && !isDayOfWeek)
    {
        _alarmRate = WHEN_MINUTES_AND_HOURS_AND_DAY_MATCH;
    }
    else if (!a2m2 && !a2m3 && !a2m4 && isDayOfWeek)
    {
        _alarmRate = WHEN_MINUTES_AND_HOURS_AND_DAY_OF_WEEK_MATCH;
    }

    //Turns off the flags before getting the values in a base-10 representation
    setBitOff(_minute, RTC_ALARM2_A2M2);
    setBitOff(_hour  , RTC_ALARM2_A2M3);
    setBitOff(_day   , RTC_ALARM2_A2M4);
    setBitOff(_day   , RTC_ALARM2_DYDT);

    //Gets the actual values
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

void Alarm2::writeAlarmOncePerMinute()
{
    _minute    = 0;
    _hour      = 0;
    _day       = 0;
    _dayOfWeek = 0;
    _alarmRate = ONCE_PER_MINUTE;

    Wire.beginTransmission(RTC_ADDR_I2C);
    Wire.write(RTC_ADDR_ALARM2);
    Wire.write(0x80); //A2M2
    Wire.write(0x80); //A2M3
    Wire.write(0x80); //A2M4
    Wire.endTransmission();
}

void Alarm2::writeAlarm(uint8_t minute)
{
    _minute    = minute;
    _hour      = 0;
    _day       = 0;
    _dayOfWeek = 0;
    _alarmRate = WHEN_MINUTES_MATCH;

    minute = fromDecimalToBcd(minute);

    Wire.beginTransmission(RTC_ADDR_I2C);
    Wire.write(RTC_ADDR_ALARM2);
    Wire.write(minute); //A2M2
    Wire.write(0x80);   //A2M3
    Wire.write(0x80);   //A2M4
    Wire.endTransmission();
}

void Alarm2::writeAlarm(uint8_t hour, uint8_t minute)
{
    _minute    = minute;
    _hour      = hour;
    _day       = 0;
    _dayOfWeek = 0;
    _alarmRate = WHEN_MINUTES_AND_HOURS_MATCH;

    hour   = fromDecimalToBcd(hour);
    minute = fromDecimalToBcd(minute);

    Wire.beginTransmission(RTC_ADDR_I2C);
    Wire.write(RTC_ADDR_ALARM2);
    Wire.write(minute); //A2M2
    Wire.write(hour);   //A2M3
    Wire.write(0x80);   //A2M4
    Wire.endTransmission();
}

void Alarm2::writeAlarm(bool useDayOfWeek, uint8_t day, uint8_t hour, uint8_t minute)
{
    _minute    = minute;
    _hour      = hour;
    _day       = useDayOfWeek ? 0            : day;
    _dayOfWeek = useDayOfWeek ? useDayOfWeek : 0 ;
    _alarmRate = useDayOfWeek
               ? WHEN_MINUTES_AND_HOURS_AND_DAY_OF_WEEK_MATCH
               : WHEN_MINUTES_AND_HOURS_AND_DAY_MATCH;

    minute = fromDecimalToBcd(minute);
    hour   = fromDecimalToBcd(hour);
    day    = fromDecimalToBcd(day);

    if (useDayOfWeek)
    {
        setBitOn(day, RTC_ALARM2_DYDT);
    }

    Wire.beginTransmission(RTC_ADDR_I2C);
    Wire.write(RTC_ADDR_ALARM2);
    Wire.write(minute);  //A2M2
    Wire.write(hour);    //A2M3
    Wire.write(day);     //A2M4
    Wire.endTransmission();
}

uint8_t Alarm2::getMinute() const
{
    return _minute;
}

uint8_t Alarm2::getHour() const
{
    return _hour;
}

uint8_t Alarm2::getDay() const
{
    return _day;
}

uint8_t Alarm2::getDayOfWeek() const
{
    return _dayOfWeek;
}

Alarm2::AlarmRate Alarm2::getAlarmRate() const
{
    return _alarmRate;
}
