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
#include "RealTimeClock.h"

#define RTC_ADDR_I2C         0x68
#define RTC_ADDR_DATE        0x00
#define RTC_ADDR_TEMPERATURE 0x11
#define RTC_ADDR_STATUS      0x0F
#define RTC_ADDR_CONTROL     0x0E
#define RTC_ADDR_AGING       0x10

#define RTC_REG_STATUS_A1F     0 //Alarm 1 Flag (A1F)
#define RTC_REG_STATUS_A2F     1 //Alarm 2 Flag (A2F)
#define RTC_REG_STATUS_BSY     2 //Busy (BSY)
#define RTC_REG_STATUS_EN32KHZ 3 //Enable 32kHz Output (EN32kHz)
#define RTC_REG_STATUS_OSF     7 //Oscillator Stop Flag (OSF)

#define RTC_REG_CONTROL_A1IE   0 //Alarm 1 Interrupt Enable (A1IE)
#define RTC_REG_CONTROL_A2IE   1 //Alarm 2 Interrupt Enable (A2IE)
#define RTC_REG_CONTROL_INTCN  2 //Interrupt Control (INTCN)
#define RTC_REG_CONTROL_RS1    3 //Rate Select (RS1)
#define RTC_REG_CONTROL_RS2    4 //Rate Select (RS2)
#define RTC_REG_CONTROL_CONV   5 //Convert Temperature (CONV)
#define RTC_REG_CONTROL_BBSQW  6 //Battery-Backed Square-Wave Enable (BBSQW)
#define RTC_REG_CONTROL_EOSC   7 //Enable Oscillator (EOSC)

RealTimeClock::RealTimeClock()
{
    Wire.begin();
}

/*************************************************************************************************/

bool RealTimeClock::wasItStopped() const
{
    return BinaryHelper::istBitSet(readRegister(RTC_ADDR_STATUS), RTC_REG_STATUS_OSF);
}

void RealTimeClock::setDateTime(int16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second)
{
    uint8_t century = 0;

    _second    = second;
    _minute    = minute;
    _hour      = hour;
    _day       = day;
    _month     = month;
    _year      = year;
    _dayOfWeek = calculateDayOfWeek(year, month, day);

    if (year > 2000)
    {
        century = 0x80;
        year -= 2000;
    }
    else
    {
        year -= 1900;
    }

    Wire.beginTransmission(RTC_ADDR_I2C);
    Wire.write(RTC_ADDR_DATE);
    Wire.write(BinaryHelper::fromDecimalToBcd(second));
    Wire.write(BinaryHelper::fromDecimalToBcd(minute));
    Wire.write(BinaryHelper::fromDecimalToBcd(hour));
    Wire.write(BinaryHelper::fromDecimalToBcd(_dayOfWeek));
    Wire.write(BinaryHelper::fromDecimalToBcd(day));
    Wire.write(BinaryHelper::fromDecimalToBcd(month) | century);
    Wire.write(BinaryHelper::fromDecimalToBcd(year));
    Wire.endTransmission();

    clearOscillatorStopFlag();
}

void RealTimeClock::syncDateTime()
{
    Wire.beginTransmission(RTC_ADDR_I2C);
    Wire.write(RTC_ADDR_DATE);
    Wire.endTransmission();

    Wire.requestFrom(RTC_ADDR_I2C, 7);
    _second    = BinaryHelper::fromBcdToDecimal(Wire.read());
    _minute    = BinaryHelper::fromBcdToDecimal(Wire.read());
    _hour      = BinaryHelper::fromBcdToDecimal(Wire.read());
    _dayOfWeek = BinaryHelper::fromBcdToDecimal(Wire.read());
    _day       = BinaryHelper::fromBcdToDecimal(Wire.read());

    uint8_t monthAndCentury = Wire.read();

    _month = BinaryHelper::fromBcdToDecimal(monthAndCentury & 0x1F);
    _year  = BinaryHelper::fromBcdToDecimal(Wire.read());
    _year += ((monthAndCentury & 0x80) != 0 ? 2000 : 1900);
}

uint8_t RealTimeClock::calculateDayOfWeek(int16_t year, uint8_t month, uint8_t day) const
{
    static uint8_t t[] = {0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4};
    year -= month < 3;
    return (year + year/4 - year/100 + year/400 + t[month-1] + day) % 7;
}

void RealTimeClock::clearOscillatorStopFlag() const
{
    uint8_t statusRegister = readRegister(RTC_ADDR_STATUS);
    statusRegister = BinaryHelper::setBitOff(statusRegister, RTC_REG_STATUS_OSF);
    writeRegister(RTC_ADDR_STATUS, statusRegister);
}

uint8_t RealTimeClock::getSecond() const
{
    return _second;
}

uint8_t RealTimeClock::getMinute() const
{
    return _minute;
}

uint8_t RealTimeClock::getHour() const
{
    return _hour;
}

uint8_t RealTimeClock::getDay() const
{
    return _day;
}

uint8_t RealTimeClock::getMonth() const
{
    return _month;
}

int16_t RealTimeClock::getYear() const
{
    return _year;
}

uint8_t RealTimeClock::getDayOfWeek() const
{
    return _dayOfWeek;
}

/*************************************************************************************************/

bool RealTimeClock::forceTemperatureUpdate() const
{
    uint8_t statusRegister = readRegister(RTC_ADDR_STATUS);
    if (BinaryHelper::istBitSet(statusRegister, RTC_REG_STATUS_BSY))
    {
        return false;
    }
    uint8_t controlRegister = readRegister(RTC_ADDR_CONTROL);
    controlRegister = BinaryHelper::setBitOn(controlRegister, RTC_REG_CONTROL_CONV);
    writeRegister(RTC_ADDR_CONTROL, controlRegister);
    return true;
}

float RealTimeClock::getTemperature() const
{
    uint8_t decimalPart, resolution;
    float temperature;

    Wire.beginTransmission(RTC_ADDR_I2C);
    Wire.write(RTC_ADDR_TEMPERATURE);
    Wire.endTransmission();

    Wire.requestFrom(RTC_ADDR_I2C, 2);
    decimalPart = Wire.read();
    resolution  = Wire.read();
    resolution  = resolution >> 6; //get only the upper nibble

    temperature = (decimalPart & 0x80)            //negative (two's complement)?
                ? (decimalPart | ~((1 << 8) - 1)) //convert it to a signed int
                : decimalPart;                    //positive number

    return temperature + (resolution * 0.25);
}

/*************************************************************************************************/

bool RealTimeClock::isBatteryEnabled() const
{
    return !BinaryHelper::istBitSet(readRegister(RTC_ADDR_CONTROL), RTC_REG_CONTROL_EOSC);
}

void RealTimeClock::enableBattery() const
{
    toggleBattery(true);
}

void RealTimeClock::disableBattery() const
{
    toggleBattery(false);
}

void RealTimeClock::toggleBattery(bool on) const
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

bool RealTimeClock::is32khzOutputEnabled() const
{
    return BinaryHelper::istBitSet(readRegister(RTC_ADDR_STATUS), RTC_REG_STATUS_EN32KHZ);
}

void RealTimeClock::enable32khzOutput() const
{
    toggle32khzOutput(true);
}

void RealTimeClock::disable32khzOutput() const
{
    toggle32khzOutput(false);
}

void RealTimeClock::toggle32khzOutput(bool on) const
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

bool RealTimeClock::isBatteryBackedSquareWaveEnabled() const
{
    return BinaryHelper::istBitSet(readRegister(RTC_ADDR_CONTROL), RTC_REG_CONTROL_BBSQW);
}

void RealTimeClock::enableBatteryBackedSquareWave(SquareWaveFrequency frequency) const
{
    toggleBatteryBackedSquareWave(true);
    enableSquareWave(frequency);
}

void RealTimeClock::disableBatteryBackedSquareWave() const
{
    toggleBatteryBackedSquareWave(false);
}

void RealTimeClock::toggleBatteryBackedSquareWave(bool on) const
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

bool RealTimeClock::isSquareWaveEnabled() const
{
    return !BinaryHelper::istBitSet(readRegister(RTC_ADDR_CONTROL), RTC_REG_CONTROL_INTCN);
}

void RealTimeClock::enableSquareWave(SquareWaveFrequency frequency) const
{
    toggleSquareWave(true, frequency);
}

void RealTimeClock::disableSquareWave() const
{
    //it will ignore the frequency when disabled
    toggleSquareWave(false, FREQ_1HZ);
    disableBatteryBackedSquareWave();
}

void RealTimeClock::toggleSquareWave(bool on, SquareWaveFrequency frequency) const
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

SquareWaveFrequency RealTimeClock::getSquareWaveFrequency() const
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

void RealTimeClock::setCalibration(int8_t value) const
{
    writeRegister(RTC_ADDR_AGING, value);
}

int8_t RealTimeClock::getCalibration() const
{
    return readRegister(RTC_ADDR_AGING);
}

/*************************************************************************************************/

uint8_t RealTimeClock::readRegister(uint8_t address) const
{
    Wire.beginTransmission(RTC_ADDR_I2C);
    Wire.write(address);
    Wire.endTransmission();
    Wire.requestFrom(RTC_ADDR_I2C, 1);
    return (uint8_t)Wire.read();
}

void RealTimeClock::writeRegister(uint8_t address, uint8_t value) const
{
    Wire.beginTransmission(RTC_ADDR_I2C);
    Wire.write(address);
    Wire.write(value);
    Wire.endTransmission();
}
