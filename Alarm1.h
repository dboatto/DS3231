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
#ifndef __ALARM1_H__
#define __ALARM1_H__

#include <stdint.h>
#include <Wire.h>
#include "BaseClock.h"
#include "BinaryHelper.h"

class Alarm1 : public BaseClock
{
public:
    Alarm1();
    void turnOn();
    void turnOff();
    void readAlarm();
    void writeAlarm(uint8_t dayOfWeek, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second);
    uint8_t getSecond() const;
    uint8_t getMinute() const;
    uint8_t getHour() const;
    uint8_t getDay() const;
    uint8_t getDayOfWeek() const;

private:
    uint8_t _second;
    uint8_t _minute;
    uint8_t _day;
    uint8_t _hour;
    uint8_t _dayOfWeek;
};

 #endif //__ALARM1_H__
