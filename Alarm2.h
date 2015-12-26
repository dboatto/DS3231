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
#ifndef __AMPLIAR_DS3231_ALARM2_H__
#define __AMPLIAR_DS3231_ALARM2_H__

#include <stdint.h>
#include <Wire.h>
#include "BaseClock.h"
#include "BinaryHelper.h"
#include "BaseAlarm.h"

namespace Ampliar { namespace DS3231 {

#define RTC_ALARM2_A2M2 7 ///< Alarm 1 A1M2
#define RTC_ALARM2_A2M3 7 ///< Alarm 1 A1M3
#define RTC_ALARM2_A2M4 7 ///< Alarm 1 A1M4
#define RTC_ALARM2_DYDT 6 ///< Alarm 1 DY/DT

/**
 * Abstraction of the second alarm of DS3231.
 *
 * This class provides access to all functionalities of DS3231's second alarm.
 *
 * @author Daniel Murari Boatto
 */
class Alarm2 : public BaseAlarm
{
public:
    enum AlarmRate : uint8_t
    {
        ALARM2_UNDEFINED,
        ONCE_PER_MINUTE,
        WHEN_MINUTES_MATCH,
        WHEN_MINUTES_AND_HOURS_MATCH,
        WHEN_MINUTES_AND_HOURS_AND_DAY_MATCH,
        WHEN_MINUTES_AND_HOURS_AND_DAY_OF_WEEK_MATCH
    };

public:
    Alarm2();
    void readAlarm();
    void writeAlarmOncePerMinute();
    void writeAlarm(uint8_t minute);
    void writeAlarm(uint8_t hour, uint8_t minute);
    void writeAlarm(bool useDayOfWeek, uint8_t day, uint8_t hour, uint8_t minute);
    uint8_t getMinute() const;
    uint8_t getHour() const;
    uint8_t getDay() const;
    uint8_t getDayOfWeek() const;
    AlarmRate getAlarmRate() const;

private:
    uint8_t _minute;
    uint8_t _day;
    uint8_t _hour;
    uint8_t _dayOfWeek;
    AlarmRate _alarmRate;
};

}} //end of namespace
#endif //__AMPLIAR_DS3231_ALARM2_H__
