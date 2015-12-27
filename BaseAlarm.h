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
#ifndef __AMPLIAR_DS3231_BASEALARM_H__
#define __AMPLIAR_DS3231_BASEALARM_H__

#include <stdint.h>
#include <Wire.h>
#include "BaseClock.h"
#include "BinaryHelper.h"

namespace Ampliar { namespace DS3231 {

/**
 * Base class for Alarm1 and Alarm2.
 *
 * This class provides common methods used by the two alarms available on DS3231.
 *
 * @author Daniel Murari Boatto
 */
class BaseAlarm : public BaseClock
{
public:
    bool isOn() const;
    void turnOn(bool enableInterruption) const;
    void turnOff(bool disableInterruption) const;
    bool wasItTriggered() const;
    virtual void readAlarm() = 0;

protected:
    BaseAlarm(uint8_t alarmControlBit, uint8_t alarmStatusBit);
    virtual ~BaseAlarm();

private:
    /**
     * Bit in the control register used to activate the alarm.
     */
    uint8_t _alarmControlBit;

    /**
     * Bit in the status register used to figure out whether the alarm was triggered or not.
     */
    uint8_t _alarmStatusBit;
};

}} //end of namespace
#endif //__AMPLIAR_DS3231_BASEALARM_H__
