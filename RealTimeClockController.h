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
#ifndef __REALTIMECLOCKCONTROLLER_H__
#define __REALTIMECLOCKCONTROLLER_H__

#include <stdint.h>
#include "BaseClock.h"
#include "BinaryHelper.h"

class RealTimeClockController : public BaseClock
{
public:
    enum Frequency : uint8_t
    {
        FREQ_1HZ,
        FREQ_1024KHZ,
        FREQ_4096KHZ,
        FREQ_8192KHZ
    };

public:
    static void enableBattery();
    static void disableBattery();
    static bool isBatteryEnabled();
    //
    static void enable32khzOutput();
    static void disable32khzOutput();
    static bool is32khzOutputEnabled();
    //
    static void enableSquareWave(Frequency frequency);
    static void disableSquareWave();
    static bool isSquareWaveEnabled();
    static uint8_t getSquareWaveFrequency();
    //
    static void enableBatteryBackedSquareWave(Frequency frequency);
    static void disableBatteryBackedSquareWave();
    static bool isBatteryBackedSquareWaveEnabled();
    //
    static void setCalibration(int8_t value);
    static int8_t getCalibration();

private:
	RealTimeClockController();
    static void toggleBattery(bool on);
    static void toggle32khzOutput(bool on);
    static void toggleBatteryBackedSquareWave(bool on);
    static void toggleSquareWave(bool on, Frequency frequency);
};

#endif /* __REALTIMECLOCKCONTROLLER_H__ */
