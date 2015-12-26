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
Alarm1::Alarm1():
    BaseAlarm(RTC_REG_CONTROL_A1IE, RTC_REG_STATUS_A1F),
    _second(0), _minute(0), _day(0), _hour(0), _dayOfWeek(0),
    _alarmRate(ALARM1_UNDEFINED)
{
    //
}

/**
 * Retrieves settings of the first alarm and stores in this object.
 *
 * This method reads information of the first alarm from the appropriate register. Then, it stores the values in this
 * object and make them available through getters, like getHour(), getMinute(), getAlarmRate(), etc.
 *
 * \b Note: you must call this method before using any getter, otherwise they will return zero or undefined.
 */
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

/**
 * Set the alarm to trigger once per second.
 *
 * This method will program the alarm to trigger once per second. Since no further details are required, the getters
 * getMinute(), getHour(), getDay() and getDayOfWeek() will return 0 if called;
 *
 * \b Note:
 * - In order to know if the alarm was triggered, you may call wasItTriggered() method;
 * - If you enabled interruption when you turned the alarm on, the INT/SQW will initiate an interrupt signal.
 */
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

/**
 * Sets the first alarm to trigger when seconds match.
 *
 * This method will setup the first alarm to trigger when the seconds in the oscillator (clock) matches the parameter.
 * For instance, if you call writeAlarm(10), the device will alarm at 14:35:10, 15:35:10, 19:12:10, etc. As you can
 * see, the only part relevant are the seconds. Since only seconds are used, the getters getMinute(), getHour(),
 * getDay() and getDayOfWeek() will return 0 if called.
 *
 * \b Note:
 * - In order to know if the alarm was triggered, you must call wasItTriggered() method;
 * - If you enabled interruption when you turned the alarm on, the INT/SQW will initiate an interrupt signal.
 *
 * @param second second (from 0 to 59)
 */
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

/**
 * Sets the first alarm to trigger when minutes and seconds match.
 *
 * This method will setup the first alarm to trigger when minutes and seconds in the oscillator (clock) match the
 * parameters. For instance, if you call writeAlarm(19, 10), the device will alarm at 00:19:10, 04:19:10, 17:19:10,
 * etc. As you can see, the only parts relevant are the minutes and the seconds. The getters getHour(), getDay() and
 * getDayOfWeek() will return 0 if called, since their values are not used.
 *
 * \b Note:
 * - In order to know if the alarm was triggered, you must call wasItTriggered() method;
 * - If you enabled interruption when you turned the alarm on, the INT/SQW will initiate an interrupt signal.
 *
 * @param minute minute (from 0 to 59)
 * @param second second (from 0 to 59)
 */
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

/**
 * Sets the first alarm to trigger when hours, minutes and seconds match.
 *
 * This method will setup the first alarm to trigger when hours, minutes and seconds in the oscillator (clock) match
 * the parameters. For instance, if you call writeAlarm(22, 35, 15), the device will alarm at 04/06/2014 22:35:15,
 * 04/07/2014 22:35:15, 02/02/1991 22:32:15, etc. As you can see, the calendar information is not considered, therefore
 * the getters getDay() and getDayOfWeek() will return 0 if called.
 *
 * \b Note:
 * - In order to know if the alarm was triggered, you must call wasItTriggered() method;
 * - If you enabled interruption when you turned the alarm on, the INT/SQW will initiate an interrupt signal.
 *
 * @param hour hour (from 0 to 23)
 * @param minute minute (from 0 to 59)
 * @param second second (from 0 to 59)
 */
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

/**
 * Sets the first alarm to trigger when the day (or day of week), hours, minutes and seconds match.
 *
 * This method will setup the first alarm to trigger when day the (or day of week), hours, minutes and seconds in the
 * oscillator (clock) match the parameters. For instance, if you call writeAlarm(false, 25, 22, 35, 15), the device
 * will alarm at Jan/25/2014 22:35:15, Dec/25/2014 22:35:15, Jul/25/1991 22:32:15, etc. As you can see, month and year
 * are not relevant.
 *
 * If the parameter \b useDayOfWeek is true, the getter getDay() will return 0 if called. Otherwise, the getter
 * getDayOfWeek() will return zero if called.
 *
 * \b Note:
 * - In order to know if the alarm was triggered, you must call wasItTriggered() method;
 * - If you enabled interruption when you turned the alarm on, the INT/SQW will initiate an interrupt signal.
 *
 * @param useDayOfWeek indicates if the parameter \b day must be interpreted as day of the week or day of the moth.
 * @param day day of the month (from 1 to 31) or day of the week (from 1 to 7).
 * @param hour hour (from 0 to 23)
 * @param minute minute (from 0 to 59)
 * @param second second (from 0 to 59)
 */
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

/**
 * Gets seconds (from 0 to 59)
 *
 * \b Note: You must call readAlarm() before using this method, otherwise it will return 0.
 *
 * @return second (from 0 to 59)
 */
uint8_t Alarm1::getSecond() const
{
    return _second;
}

/**
 * Gets minutes (from 0 to 59)
 *
 * \b Note: You must call readAlarm() before using this method, otherwise it will return 0.
 *
 * @return minute (from 0 to 59)
 */
uint8_t Alarm1::getMinute() const
{
    return _minute;
}

/**
 * Gets hours in 24-hour format (from 0 to 23)
 *
 * \b Note: You must call readAlarm() before using this method, otherwise it will return 0.
 *
 * @return hour (from 0 to 23)
 */
uint8_t Alarm1::getHour() const
{
    return _hour;
}

/**
 * Gets the day of the month (from 1 to 31)
 *
 * \b Note: You must call readAlarm() before using this method, otherwise it will return 0.
 *
 * @return day of the month (from 1 to 31)
 */
uint8_t Alarm1::getDay() const
{
    return _day;
}

/**
 * Gets the day of week (from 1 to 7)
 *
 * \b Note: You must call readAlarm() before using this method, otherwise it will return 0.
 *
 * @return day of week (from 1 to 7)
 */
uint8_t Alarm1::getDayOfWeek() const
{
    return _dayOfWeek;
}

/**
 * Gets the alarm rate
 *
 * \b Note: You must call readAlarm() before using this method, otherwise it will return ALARM1_UNDEFINED.
 *
 * @return alarm rate
 */
Alarm1::AlarmRate Alarm1::getAlarmRate() const
{
    return _alarmRate;
}
