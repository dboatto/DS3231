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

enum SquareWaveFrequency
{
    FREQ_1HZ,
    FREQ_1024KHZ,
    FREQ_4096KHZ,
    FREQ_8192KHZ
};

class RealTimeClock
{
public:
	RealTimeClock();
    //
    void setDateTime(int16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second);
    void syncDateTime();
    bool wasItStopped() const;
    //
    bool forceTemperatureUpdate() const;
    float getTemperature() const;
    //
    void enableBattery() const;
    void disableBattery() const;
    bool isBatteryEnabled() const;
    //
    void enable32khzOutput() const;
    void disable32khzOutput() const;
    bool is32khzOutputEnabled() const;
    //
    void enableSquareWave(SquareWaveFrequency frequency) const;
    void disableSquareWave() const;
    bool isSquareWaveEnabled() const;
    SquareWaveFrequency getSquareWaveFrequency() const;
    //
    void enableBatteryBackedSquareWave(SquareWaveFrequency frequency) const;
    void disableBatteryBackedSquareWave() const;
    bool isBatteryBackedSquareWaveEnabled() const;
    //
    void setCalibration(int8_t value) const;
    int8_t getCalibration() const;
    //
    uint8_t getSecond() const;
    uint8_t getMinute() const;
    uint8_t getHour() const;
    uint8_t getDay() const;
    uint8_t getMonth() const;
    int16_t getYear() const;
    uint8_t getDayOfWeek() const;

private:
    uint8_t _second;
    uint8_t _minute;
    uint8_t _day;
    uint8_t _hour;
    uint8_t _month;
    uint8_t _dayOfWeek;
    int16_t _year;
    uint8_t calculateDayOfWeek(int16_t year, uint8_t month, uint8_t day) const;
    void clearOscillatorStopFlag() const;
    //
    void toggleBattery(bool on) const;
    void toggle32khzOutput(bool on) const;
    void toggleSquareWave(bool on, SquareWaveFrequency frequency) const;
    void toggleBatteryBackedSquareWave(bool on) const;
    //
    uint8_t readRegister(uint8_t address) const;
    void writeRegister(uint8_t address, uint8_t value) const;
};

#endif //__REALTIMECLOCK_H__
