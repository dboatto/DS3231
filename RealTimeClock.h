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
#ifndef __REALTIMECLOCK_H__
#define __REALTIMECLOCK_H__

#include <stdint.h>
#include <Wire.h>
#include "BinaryHelper.h"
#include "BaseClock.h"

/**
 * This class abstracts basic date/time and temperature reading functionalities of DS3231.
 *
 * The objective of this class is provide basic date/time and temperature reading methods, while keeping a small
 * footprint, so it can be used in space constrained microcontrollers, like those ones used by Arduino boards.
 *
 * @author Daniel Murari Boatto
 */
class RealTimeClock : public BaseClock
{
public:
    void readDateTime();
    void writeDateTime(int16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second);
    static bool wasItStopped();
    //
    static bool forceTemperatureUpdate();
    static float readTemperature();
    //
    uint8_t getSecond() const;
    uint8_t getMinute() const;
    uint8_t getHour() const;
    uint8_t getDay() const;
    uint8_t getMonth() const;
    uint8_t getDayOfWeek() const;
    int16_t getYear() const;

private:
    uint8_t _second;
    uint8_t _minute;
    uint8_t _day;
    uint8_t _hour;
    uint8_t _month;
    uint8_t _dayOfWeek;
    int16_t _year;
    static void clearOscillatorStopFlag();
    static uint8_t calculateDayOfWeek(int16_t year, uint8_t month, uint8_t day);
};

#endif //__REALTIMECLOCK_H__
