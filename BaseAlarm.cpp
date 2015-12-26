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

/**
 * Constructor.
 *
 * This constructor does not read any information from DS3231. It only performs member variables initialization.
 */
BaseAlarm::BaseAlarm(uint8_t alarmControlBit, uint8_t alarmStatusBit):
    _alarmControlBit(alarmControlBit),
    _alarmStatusBit(alarmStatusBit)
{
    //
}

/**
 * Virtual destructor.
 *
 * Currently, this method is empty.
 */
BaseAlarm::~BaseAlarm()
{
    //
}

/**
 * Checks whether the alarm is turned on or not.
 *
 * @return true, if the alarm is active.
 */
bool BaseAlarm::isOn() const
{
    Wire.beginTransmission(RTC_ADDR_I2C);
    Wire.write(RTC_ADDR_CONTROL);
    Wire.endTransmission();

    Wire.requestFrom(RTC_ADDR_I2C, 1);
    uint8_t controlRegister = Wire.read();

    return isBitSet(controlRegister, _alarmControlBit);
}

/**
 * Turn on the alarm.
 *
 * This method turns on the alarm and allows you to enable the hardware interruption output on the INT/SQW pin.
 *
 * @param enableInterruption if true, enables the the hardware interruption output on the INT/SQW pin.
 */
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

/**
 * Turns off the alarm.
 *
 * This method turns off the alarm and allows you to disable the hardware interruption output on the INT/SQW pin.
 *
 * @param disableInterruption if true, disables the the hardware interruption output on the INT/SQW pin.
 */
void BaseAlarm::turnOff(bool disableInterruption) const
{
    uint8_t controlRegister = readRegister(RTC_ADDR_CONTROL);
    setBitOff(controlRegister, _alarmControlBit);
    if (disableInterruption)
    {
        setBitOff(controlRegister, RTC_REG_CONTROL_INTCN);
    }
    writeRegister(RTC_ADDR_CONTROL, controlRegister);
}

/**
 * Checks whether the alarm was triggered or not.
 *
 * This method checks a specific flag in the status register to figure out whether the alarm was triggered or not. If
 * it was triggered, this method also clears the flag (set it to zero).
 *
 * If you don't want to call this method periodically to check the alarm status, you may choose to enable hardware
 * interruption when you turn on the alarm. Check the method turnOn() for more details.
 *
 * @return true if the alarm was triggered and false, otherwise.
 */
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
