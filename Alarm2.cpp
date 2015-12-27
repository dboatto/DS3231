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

using namespace Ampliar::DS3231;
using Ampliar::BinaryHelper::setBitOn;
using Ampliar::BinaryHelper::setBitOff;
using Ampliar::BinaryHelper::isBitSet;
using Ampliar::BinaryHelper::fromBcdToDecimal;
using Ampliar::BinaryHelper::fromDecimalToBcd;

/**
 * Constructor.
 *
 * This constructor does not read any information from the device. Therefore, if you try to call any getter, it will
 * return the values initialized here (i.e., zero).
 */
Alarm2::Alarm2():
    BaseAlarm(RTC_REG_CONTROL_A2IE, RTC_REG_STATUS_A2F),
    _minute(0), _day(0), _hour(0), _dayOfWeek(0), _alarmRate(ALARM2_UNDEFINED)

{
    //
}

/**
 * Retrieves settings of the second alarm and stores in this object.
 *
 * This method reads information of the second alarm from DS3231 internal registers. Then, it stores the values in this
 * object and make them available through getters, like getHour(), getMinute(), getAlarmRate(), etc.
 *
 * \b Note: you must call this method before using any getter, otherwise they will return zero or undefined.
 */
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

/**
 * Set the alarm to trigger once per minute.
 *
 * This method will program the alarm to trigger once per minute. Since no further details are required, the getters
 * getMinute(), getHour(), getDay() and getDayOfWeek() will return 0 if called;
 *
 * \b Note:
 * - In order to know if the alarm was triggered, you may call wasItTriggered() method;
 * - If you enabled interruption when you turned the alarm on, the INT/SQW will initiate an interrupt signal.
 */
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

/**
 * Sets the second alarm to trigger when minutes match.
 *
 * This method will setup the second alarm to trigger when the minutes in DS3231's clock matches the parameter.
 * For instance, if you call writeAlarm(35), the device will alarm at 14:35:40, 15:35:12, 19:35:00, etc. As you can
 * see, the only part relevant are the minutes component. Since only minutes are used, the getters getHour(),
 * getDay() and getDayOfWeek() will return 0 if called.
 *
 * \b Note:
 * - In order to know if the alarm was triggered, you must call wasItTriggered() method;
 * - If you enabled interruption when you turned the alarm on, the INT/SQW will initiate an interrupt signal.
 *
 * @param minute The minutes (from 0 to 59).
 */
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

/**
 * Sets the second alarm to trigger when hours and minutes match.
 *
 * This method will setup the second alarm to trigger when hours and minutes in DS3231's clock match the
 * parameters. For instance, if you call writeAlarm(19, 10), the device will alarm at 19:10:05, 19:10:45, 19:10:00,
 * etc. As you can see, the only parts relevant are the hours and the minutes components. The getters getDay() and
 * getDayOfWeek() will return 0 if called, since their values are not used.
 *
 * \b Note:
 * - In order to know if the alarm was triggered, you must call wasItTriggered() method;
 * - If you enabled interruption when you turned the alarm on, the INT/SQW will initiate an interrupt signal.
 *
 * @param hour   The hours (from 0 to 23).
 * @param minute The minutes (from 0 to 59).
 */
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

/**
 * Sets the second alarm to trigger when the day (or day of week), hours and minutes match.
 *
 * This method will setup the second alarm to trigger when day (or day of week), hours and minutes in DS3231's
 * clock match the parameters. For instance, if you call writeAlarm(false, 25, 22, 35, 15), the device will alarm at
 * Jan/25/2014 22:35:00, Dec/25/2014 22:35:12, Jul/25/1991 22:32:56, etc. As you can see, month and year are not
 * relevant.
 *
 * If the parameter \b useDayOfWeek is true, the getter getDay() will return 0 if called. Otherwise, the getter
 * getDayOfWeek() will return zero if called.
 *
 * \b Note:
 * - In order to know if the alarm was triggered, you must call wasItTriggered() method;
 * - If you enabled interruption when you turned the alarm on, the INT/SQW will initiate an interrupt signal.
 *
 * @param useDayOfWeek Indicates if the parameter \b day must be interpreted as day of the week or day of the month.
 * @param day          The day of the month (from 1 to 31) or the day of the week (from 1 to 7).
 * @param hour         The hours (from 0 to 23).
 * @param minute       The minutes (from 0 to 59).
 */
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

/**
 * Gets the minute component of the date represented by this instance.
 *
 * \b Note: You must call readAlarm() before using this method, otherwise it will return 0.
 *
 * @return The minute component (from 0 to 59).
 */
uint8_t Alarm2::getMinute() const
{
    return _minute;
}

/**
 * Gets the hour component of the date represented by this instance.
 *
 * \b Note: You must call readAlarm() before using this method, otherwise it will return 0.
 *
 * @return The hour component (from 0 to 23).
 */
uint8_t Alarm2::getHour() const
{
    return _hour;
}

/**
 * Gets the day of the month represented by this instance.
 *
 * \b Note: You must call readAlarm() before using this method, otherwise it will return 0.
 *
 * @return The day of the month (from 1 to 31).
 */
uint8_t Alarm2::getDay() const
{
    return _day;
}

/**
 * Gets the day of the week represented by this instance.
 *
 * \b Note: You must call readAlarm() before using this method, otherwise it will return 0.
 *
 * @return The day of the week (from 1 to 7).
 */
uint8_t Alarm2::getDayOfWeek() const
{
    return _dayOfWeek;
}

/**
 * Gets the alarm rate.
 *
 * \b Note: You must call readAlarm() before using this method, otherwise it will return Alarm2::ALARM2_UNDEFINED.
 *
 * @return The alarm rate. Check enum Alarm1::AlarmRate for more details.
 */
Alarm2::AlarmRate Alarm2::getAlarmRate() const
{
    return _alarmRate;
}
