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

using namespace Ampliar::DS3231;
using namespace Ampliar::BinaryHelper;

/**
 * Checks whether the battery is enabled or not.
 *
 * It checks if the battery is enabled when DS3231 switches to the battery, i.e., when the main power supply
 * is turned off.
 *
 * @return True if it is enabled.
 */
bool RealTimeClockController::isBatteryEnabled()
{
    return !isBitSet(readRegister(RTC_ADDR_CONTROL), RTC_REG_CONTROL_EOSC);
}

/**
 * Enables the battery-backed mode.
 *
 * This methods enables the battery when DS3231 switches to the battery, i.e., when the main power supply
 * is turned off.
 */
void RealTimeClockController::enableBattery()
{
    toggleBattery(true);
}

/**
 * Disables the battery-backed mode.
 *
 * This methods disables the battery when DS3231 switches to the battery, i.e., when the main power supply
 * is turned off.
 *
 * \b Note: If you disable the battery and the power supply is cut off, the oscillator (clock) will stop. Therefore,
 * next time you start using your board, you will need to set the date/time again.
 */
void RealTimeClockController::disableBattery()
{
    toggleBattery(false);
}

/**
 * Toggles the battery-backed mode.
 *
 * This method toggles the battery-backed mode by changing the EOSC bit of the control register.
 *
 * @param on True to enable; false to disable.
 */
void RealTimeClockController::toggleBattery(bool on)
{
    uint8_t controlRegister = readRegister(RTC_ADDR_CONTROL);
    if (on)
    {
        setBitOff(controlRegister, RTC_REG_CONTROL_EOSC);
    }
    else
    {
        setBitOn(controlRegister, RTC_REG_CONTROL_EOSC);
    }
    writeRegister(RTC_ADDR_CONTROL, controlRegister);
}

/**
 * Checks whether the 32 kHz is enabled or not.
 *
 * It checks if the 32 kHz output is enabled.
 *
 * @return True if it is enabled.
 */
bool RealTimeClockController::is32khzOutputEnabled()
{
    return isBitSet(readRegister(RTC_ADDR_STATUS), RTC_REG_STATUS_EN32KHZ);
}

/**
 * Enables the 32 KHz output.
 *
 * This method enables an output of a 32.768 kHz square-wave signal on the correspondent pin of DS3231.
 */
void RealTimeClockController::enable32khzOutput()
{
    toggle32khzOutput(true);
}

/**
 * Disables the 32 KHz output.
 *
 * This methods disables the 32 kHz output and the correspondent pin goes to a high-impedance state.
 */
void RealTimeClockController::disable32khzOutput()
{
    toggle32khzOutput(false);
}

/**
 * Toggles the 32 KHz output pin.
 *
 * This method toggles the 32 KHz output pin by changing the EN32kHz bit of the status register.
 *
 * @param on True to enable; false to disable.
 */
void RealTimeClockController::toggle32khzOutput(bool on)
{
    uint8_t statusRegister = readRegister(RTC_ADDR_STATUS);
    if (on)
    {
        setBitOn(statusRegister, RTC_REG_STATUS_EN32KHZ);
    }
    else
    {
        setBitOff(statusRegister, RTC_REG_STATUS_EN32KHZ);

    }
    writeRegister(RTC_ADDR_STATUS, statusRegister);
}

/**
 * Checks whether the battery-backed square-wave output is enabled or not.
 *
 * It checks if the square-wave output is enabled when DS3231 switches to the battery, i.e., when the main power supply
 * is turned off.
 *
 * @return True if it is enabled;
 */
bool RealTimeClockController::isBatteryBackedSquareWaveEnabled()
{
    return isBitSet(readRegister(RTC_ADDR_CONTROL), RTC_REG_CONTROL_BBSQW);
}

/**
 * Enables the battery-backed square-wave output.
 *
 * It enables the square-wave output when DS3231 switches to the battery, i.e., when the main power supply is turned
 * off.
 *
 * This method also enables the square-wave output pin.
 *
 * @param frequency The frequency of the square-wave.
 */
void RealTimeClockController::enableBatteryBackedSquareWave(Frequency frequency)
{
    toggleBatteryBackedSquareWave(true);
    enableSquareWave(frequency);
}

/**
 * Disables the battery-backed square-wave mode.
 *
 * It disables the square-wave output when DS3231 switches to the battery, i.e., when the main power supply is turned
 * off.
 *
 * \b Note: This method does \b NOT turn off the square-wave output pin.
 */
void RealTimeClockController::disableBatteryBackedSquareWave()
{
    toggleBatteryBackedSquareWave(false);
}

/**
 * Toggles the battery-backed square-wave mode.
 *
 * This method toggles the battery-backed square-wave output by changing the BBSQW bit of the control register.
 *
 * @param on True to enable; false to disable.
 */
void RealTimeClockController::toggleBatteryBackedSquareWave(bool on)
{
    uint8_t controlRegister = readRegister(RTC_ADDR_CONTROL);
    if (on)
    {
        setBitOn(controlRegister, RTC_REG_CONTROL_BBSQW);
    }
    else
    {
        setBitOff(controlRegister, RTC_REG_CONTROL_BBSQW);
    }
    writeRegister(RTC_ADDR_CONTROL, controlRegister);
}

/**
 * Checks whether the square-wave mode is enabled or not.
 *
 * It checks if the square-wave output is enabled.
 *
 * @return True if it is enabled.
 */
bool RealTimeClockController::isSquareWaveEnabled()
{
    return !isBitSet(readRegister(RTC_ADDR_CONTROL), RTC_REG_CONTROL_INTCN);
}

/**
 * Enables the square-wave output at a given frequency.
 *
 * This method enables an output of a square-wave signal, at a given frequency, in the correspondent pin.
 *
 * @param frequency The frequency  of the square-wave.
 */
void RealTimeClockController::enableSquareWave(Frequency frequency)
{
    toggleSquareWave(true, frequency);
}

/**
 * Disables the square-wave output.
 *
 * It disables the square-wave output signal.
 *
 * \b Note: This method disables the battery-backed square-wave mode.
 */
void RealTimeClockController::disableSquareWave()
{
    //it will ignore the frequency when disabled
    toggleSquareWave(false, FREQ_1HZ);
    disableBatteryBackedSquareWave();
}

/**
 * Toggles the square-wave output pin.
 *
 * This method toggles the square-wave output pin by changing the INTCN bit of the control register.
 *
 * @param on        True to enable; false to disable.
 * @param frequency The frequency of the square-wave. This parameter will be ignored if the parameter "on" is false.
 */
void RealTimeClockController::toggleSquareWave(bool on, Frequency frequency)
{
    uint8_t controlRegister = readRegister(RTC_ADDR_CONTROL);

    if (on)
    {
        switch (frequency)
        {
            case FREQ_1HZ:
                setBitOff(controlRegister, RTC_REG_CONTROL_RS1);
                setBitOff(controlRegister, RTC_REG_CONTROL_RS2);
                break;

            case FREQ_1024KHZ:
                setBitOn(controlRegister, RTC_REG_CONTROL_RS1);
                setBitOff(controlRegister, RTC_REG_CONTROL_RS2);
                break;

            case FREQ_4096KHZ:
                setBitOff(controlRegister, RTC_REG_CONTROL_RS1);
                setBitOn(controlRegister, RTC_REG_CONTROL_RS2);
                break;

            case FREQ_8192KHZ:
                setBitOn(controlRegister, RTC_REG_CONTROL_RS2);
                setBitOn(controlRegister, RTC_REG_CONTROL_RS2);
                break;
        }
        setBitOff(controlRegister, RTC_REG_CONTROL_INTCN);
    }
    else
    {
        setBitOn(controlRegister, RTC_REG_CONTROL_INTCN);
    }
    writeRegister(RTC_ADDR_CONTROL, controlRegister);
}

/**
 * Returns the frequency of the square-wave signal.
 *
 * This method returns the frequency of the square-wave signal even when the square-wave output is disabled.
 *
 * @return Frequency of the square-wave signal.
 */
RealTimeClockController::Frequency RealTimeClockController::getSquareWaveFrequency()
{
    uint8_t controlRegister = readRegister(RTC_ADDR_CONTROL);
    bool rs1 = isBitSet(controlRegister, RTC_REG_CONTROL_RS1);
    bool rs2 = isBitSet(controlRegister, RTC_REG_CONTROL_RS2);

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

/**
 * Writes a value in the aging offset register.
 *
 * The aging offset register takes a user-provided value to add to or subtract from the codes in the capacitance array
 * registers. (source: DS3231 datasheet, Maxim Integrated, 2015)
 *
 * Positive aging values add capacitance to the array, slowing the oscillator frequency. Negative values remove
 * capacitance from the array, increasing the oscillator frequency. (source: DS3231 datasheet, Maxim Integrated, 2015)
 *
 * Please referrer to the manufacture's datasheet for more information.
 */
void RealTimeClockController::writeCalibration(int8_t value)
{
    writeRegister(RTC_ADDR_AGING, value);
}

/**
 * Reads the value in the aging offset register.
 *
 * @return user-provided value used to add to or subtract from the codes in the capacitance array registers.
 */
int8_t RealTimeClockController::readCalibration()
{
    return readRegister(RTC_ADDR_AGING);
}
