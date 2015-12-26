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
#include "BaseAlarm.h"

using namespace Ampliar::DS3231;
using Ampliar::BinaryHelper::setBitOn;
using Ampliar::BinaryHelper::setBitOff;
using Ampliar::BinaryHelper::isBitSet;

BaseAlarm::BaseAlarm(uint8_t alarmControlBit, uint8_t alarmStatusBit):
    _alarmControlBit(alarmControlBit),
    _alarmStatusBit(alarmStatusBit)
{
    //
}

BaseAlarm::~BaseAlarm()
{
    //
}

bool BaseAlarm::isOn() const
{
    Wire.beginTransmission(RTC_ADDR_I2C);
    Wire.write(RTC_ADDR_CONTROL);
    Wire.endTransmission();

    Wire.requestFrom(RTC_ADDR_I2C, 1);
    uint8_t controlRegister = Wire.read();

    return isBitSet(controlRegister, _alarmControlBit);
}

void BaseAlarm::turnOn(bool enableInterruption) const
{
    uint8_t controlRegister = readRegister(RTC_ADDR_CONTROL);
    if (enableInterruption)
    {
        setBitOn(controlRegister, RTC_REG_CONTROL_INTCN);
    }
    setBitOn(controlRegister, _alarmControlBit);
    writeRegister(RTC_ADDR_CONTROL, controlRegister);
}

void BaseAlarm::turnOff() const
{
    uint8_t controlRegister = readRegister(RTC_ADDR_CONTROL);
    setBitOff(controlRegister, _alarmControlBit);
    writeRegister(RTC_ADDR_CONTROL, controlRegister);
}

bool BaseAlarm::wasItTriggered() const
{
    uint8_t statusRegister = readRegister(RTC_ADDR_STATUS);
    bool triggered = isBitSet(statusRegister, _alarmStatusBit);

    //If it was triggered, it is necessary to reset it
    if (triggered)
    {
        setBitOff(statusRegister, _alarmStatusBit);
        writeRegister(RTC_ADDR_STATUS, statusRegister);
    }

    return triggered;
}
