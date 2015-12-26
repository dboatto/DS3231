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
#ifndef __UPSCALE_DS3231_ALARM1_H__
#define __UPSCALE_DS3231_ALARM1_H__

#include <stdint.h>
#include <Wire.h>
#include "BaseClock.h"
#include "BinaryHelper.h"

namespace Upscale { namespace DS3231 {

#define RTC_ALARM1_A1M1 7 ///< Alarm 1 A1M1
#define RTC_ALARM1_A1M2 7 ///< Alarm 1 A1M2
#define RTC_ALARM1_A1M3 7 ///< Alarm 1 A1M3
#define RTC_ALARM1_A1M4 7 ///< Alarm 1 A1M4
#define RTC_ALARM1_DYDT 6 ///< Alarm 1 DY/DT

class Alarm1 : public BaseClock
{
public:
    enum AlarmRate : uint8_t
    {
        ONCE_PER_SECOND,
        WHEN_SECONDS_MATCH,
        WHEN_SECONDS_AND_MINUTES_MATCH,
        WHEN_SECONDS_AND_MINUTES_AND_HOURS_MATCH,
        WHEN_SECONDS_AND_MINUTES_AND_HOURS_AND_DAY_MATCH,
        WHEN_SECONDS_AND_MINUTES_AND_HOURS_AND_DAY_OF_WEEK_MATCH
    };

public:
    bool isOn() const;
    void turnOn(bool enableInterruption) const;
    void turnOff() const;
    bool wasItTriggered() const;
    void readAlarm();
    void writeAlarmOncePerSecond();
    void writeAlarm(uint8_t second);
    void writeAlarm(uint8_t minute, uint8_t second);
    void writeAlarm(uint8_t hour, uint8_t minute, uint8_t second);
    void writeAlarm(bool useDayOfWeek, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second);
    uint8_t getSecond() const;
    uint8_t getMinute() const;
    uint8_t getHour() const;
    uint8_t getDay() const;
    uint8_t getDayOfWeek() const;
    AlarmRate getAlarmRate() const;

private:
    uint8_t _second;
    uint8_t _minute;
    uint8_t _day;
    uint8_t _hour;
    uint8_t _dayOfWeek;
    AlarmRate _alarmRate;
};

}} //end of namespace
#endif //__UPSCALE_DS3231_ALARM1_H__
