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
#ifndef __AMPLIAR_DS3231_REALTIME_CLOCK_CONTROLLER_H__
#define __AMPLIAR_DS3231_REALTIME_CLOCK_CONTROLLER_H__

#include <stdint.h>
#include "BaseClock.h"
#include "BinaryHelper.h"

namespace Ampliar { namespace DS3231 {

/**
 * This class controls advanced features of DS3231.
 *
 * This class provides access to all advanced features of DS3231. Most of them can be activated or deactivated.
 *
 * @author Daniel Murari Boatto
 */
class RealTimeClockController : public BaseClock
{
public:
    /**
     * Square-Wave Output Frequency
     */
    enum Frequency : uint8_t
    {
        FREQ_1HZ,     ///< 1 Hz
        FREQ_1024KHZ, ///< 1,024 kHz
        FREQ_4096KHZ, ///< 4,096 kHz
        FREQ_8192KHZ  ///< 8,192 kHz
    };

public:
    void enableBattery() const;
    void disableBattery() const;
    bool isBatteryEnabled() const;
    //
    void enable32khzOutput() const;
    void disable32khzOutput() const;
    bool is32khzOutputEnabled() const;
    //
    void enableSquareWave(Frequency frequency) const;
    void disableSquareWave() const;
    bool isSquareWaveEnabled() const;
    Frequency getSquareWaveFrequency() const;
    //
    void enableBatteryBackedSquareWave(Frequency frequency) const;
    void disableBatteryBackedSquareWave() const;
    bool isBatteryBackedSquareWaveEnabled() const;
    //
    void writeCalibration(int8_t value) const;
    int8_t readCalibration() const;

private:
    void toggleBattery(bool on) const;
    void toggle32khzOutput(bool on) const;
    void toggleBatteryBackedSquareWave(bool on) const;
    void toggleSquareWave(bool on, Frequency frequency) const;
};

}} //end of namespace
#endif //__AMPLIAR_DS3231_REALTIME_CLOCK_CONTROLLER_H__
