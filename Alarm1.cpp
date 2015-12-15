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

Alarm1::Alarm1()
{
}

void Alarm1::turnOn()
{
    uint8_t controlRegister = readRegister(RTC_ADDR_CONTROL);
    controlRegister = BinaryHelper::setBitOn(controlRegister, RTC_REG_CONTROL_INTCN);
    controlRegister = BinaryHelper::setBitOn(controlRegister, RTC_REG_CONTROL_A1IE);
    writeRegister(RTC_ADDR_CONTROL, controlRegister);
}

void Alarm1::turnOff()
{
    uint8_t controlRegister = readRegister(RTC_ADDR_CONTROL);
    controlRegister = BinaryHelper::setBitOff(controlRegister, RTC_REG_CONTROL_A1IE);
    writeRegister(RTC_ADDR_CONTROL, controlRegister);
}

void Alarm1::readAlarm()
{
    Wire.beginTransmission(RTC_ADDR_I2C);
    Wire.write(RTC_ADDR_ALARM1);
    Wire.endTransmission();

    Wire.requestFrom(RTC_ADDR_I2C, 7);
    _second    = BinaryHelper::fromBcdToDecimal(Wire.read());
    _minute    = BinaryHelper::fromBcdToDecimal(Wire.read());
    _hour      = BinaryHelper::fromBcdToDecimal(Wire.read());
    _dayOfWeek = BinaryHelper::fromBcdToDecimal(Wire.read());
    _day       = BinaryHelper::fromBcdToDecimal(Wire.read());
}

void Alarm1::writeAlarm(uint8_t dayOfWeek, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second)
{
    _second    = second;
    _minute    = minute;
    _hour      = hour;
    _day       = day;
    _dayOfWeek = dayOfWeek;

    Wire.beginTransmission(RTC_ADDR_I2C);
    Wire.write(RTC_ADDR_ALARM1);
    Wire.write(BinaryHelper::fromDecimalToBcd(second));
    Wire.write(BinaryHelper::fromDecimalToBcd(minute));
    Wire.write(BinaryHelper::fromDecimalToBcd(hour));
    Wire.write(BinaryHelper::fromDecimalToBcd(dayOfWeek));
    Wire.write(BinaryHelper::fromDecimalToBcd(day));
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
