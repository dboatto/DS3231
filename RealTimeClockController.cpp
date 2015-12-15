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
#include "RealTimeClockController.h"

bool RealTimeClockController::isBatteryEnabled()
{
    return !BinaryHelper::istBitSet(readRegister(RTC_ADDR_CONTROL), RTC_REG_CONTROL_EOSC);
}

void RealTimeClockController::enableBattery()
{
    toggleBattery(true);
}

void RealTimeClockController::disableBattery()
{
    toggleBattery(false);
}

void RealTimeClockController::toggleBattery(bool on)
{
    uint8_t controlRegister = readRegister(RTC_ADDR_CONTROL);
    if (on)
    {
        controlRegister = BinaryHelper::setBitOff(controlRegister, RTC_REG_CONTROL_EOSC);
    }
    else
    {
        controlRegister = BinaryHelper::setBitOn(controlRegister, RTC_REG_CONTROL_EOSC);

    }
    writeRegister(RTC_ADDR_CONTROL, controlRegister);
}

/*************************************************************************************************/

bool RealTimeClockController::is32khzOutputEnabled()
{
    return BinaryHelper::istBitSet(readRegister(RTC_ADDR_STATUS), RTC_REG_STATUS_EN32KHZ);
}

void RealTimeClockController::enable32khzOutput()
{
    toggle32khzOutput(true);
}

void RealTimeClockController::disable32khzOutput()
{
    toggle32khzOutput(false);
}

void RealTimeClockController::toggle32khzOutput(bool on)
{
    uint8_t statusRegister = readRegister(RTC_ADDR_STATUS);
    if (on)
    {
        statusRegister = BinaryHelper::setBitOn(statusRegister, RTC_REG_STATUS_EN32KHZ);
    }
    else
    {
        statusRegister = BinaryHelper::setBitOff(statusRegister, RTC_REG_STATUS_EN32KHZ);

    }
    writeRegister(RTC_ADDR_STATUS, statusRegister);
}

/*************************************************************************************************/

bool RealTimeClockController::isBatteryBackedSquareWaveEnabled()
{
    return BinaryHelper::istBitSet(readRegister(RTC_ADDR_CONTROL), RTC_REG_CONTROL_BBSQW);
}

void RealTimeClockController::enableBatteryBackedSquareWave(SquareWaveFrequency frequency)
{
    toggleBatteryBackedSquareWave(true);
    enableSquareWave(frequency);
}

void RealTimeClockController::disableBatteryBackedSquareWave()
{
    toggleBatteryBackedSquareWave(false);
}

void RealTimeClockController::toggleBatteryBackedSquareWave(bool on)
{
    uint8_t controlRegister = readRegister(RTC_ADDR_CONTROL);
    if (on)
    {
        controlRegister = BinaryHelper::setBitOn(controlRegister, RTC_REG_CONTROL_BBSQW);
    }
    else
    {
        controlRegister = BinaryHelper::setBitOff(controlRegister, RTC_REG_CONTROL_BBSQW);
    }
    writeRegister(RTC_ADDR_CONTROL, controlRegister);
}

/*************************************************************************************************/

bool RealTimeClockController::isSquareWaveEnabled()
{
    return !BinaryHelper::istBitSet(readRegister(RTC_ADDR_CONTROL), RTC_REG_CONTROL_INTCN);
}

void RealTimeClockController::enableSquareWave(SquareWaveFrequency frequency)
{
    toggleSquareWave(true, frequency);
}

void RealTimeClockController::disableSquareWave()
{
    //it will ignore the frequency when disabled
    toggleSquareWave(false, FREQ_1HZ);
    disableBatteryBackedSquareWave();
}

void RealTimeClockController::toggleSquareWave(bool on, SquareWaveFrequency frequency)
{
    uint8_t controlRegister = readRegister(RTC_ADDR_CONTROL);

    if (on)
    {
        switch (frequency)
        {
            case FREQ_1HZ:
                controlRegister = BinaryHelper::setBitOff(controlRegister, RTC_REG_CONTROL_RS1);
                controlRegister = BinaryHelper::setBitOff(controlRegister, RTC_REG_CONTROL_RS2);
                break;

            case FREQ_1024KHZ:
                controlRegister = BinaryHelper::setBitOn(controlRegister, RTC_REG_CONTROL_RS1);
                controlRegister = BinaryHelper::setBitOff(controlRegister, RTC_REG_CONTROL_RS2);
                break;

            case FREQ_4096KHZ:
                controlRegister = BinaryHelper::setBitOff(controlRegister, RTC_REG_CONTROL_RS1);
                controlRegister = BinaryHelper::setBitOn(controlRegister, RTC_REG_CONTROL_RS2);
                break;

            case FREQ_8192KHZ:
                controlRegister = BinaryHelper::setBitOn(controlRegister, RTC_REG_CONTROL_RS2);
                controlRegister = BinaryHelper::setBitOn(controlRegister, RTC_REG_CONTROL_RS2);
                break;
        }
        controlRegister = BinaryHelper::setBitOff(controlRegister, RTC_REG_CONTROL_INTCN);
    }
    else
    {
        controlRegister = BinaryHelper::setBitOn(controlRegister, RTC_REG_CONTROL_INTCN);
    }
    writeRegister(RTC_ADDR_CONTROL, controlRegister);
}

SquareWaveFrequency RealTimeClockController::getSquareWaveFrequency()
{
    uint8_t controlRegister = readRegister(RTC_ADDR_CONTROL);
    bool rs1 = BinaryHelper::istBitSet(controlRegister, RTC_REG_CONTROL_RS1);
    bool rs2 = BinaryHelper::istBitSet(controlRegister, RTC_REG_CONTROL_RS2);

    if (rs1 == 0 && rs2 == 0)
    {
        return FREQ_1HZ;
    }
    else if (rs1 == 1 && rs2 == 0)
    {
        return FREQ_1024KHZ;
    }
    else if (rs1 == 0 && rs2 == 1)
    {
        return FREQ_4096KHZ;
    }
    else
    {
        return FREQ_8192KHZ;
    }
}

/*************************************************************************************************/

void RealTimeClockController::setCalibration(int8_t value)
{
    writeRegister(RTC_ADDR_AGING, value);
}

int8_t RealTimeClockController::getCalibration()
{
    return readRegister(RTC_ADDR_AGING);
}
