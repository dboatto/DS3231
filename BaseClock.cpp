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
#include "BaseClock.h"

BaseClock::BaseClock()
{
    Wire.begin();
}

uint8_t BaseClock::readRegister(uint8_t address)
{
    Wire.beginTransmission(RTC_ADDR_I2C);
    Wire.write(address);
    Wire.endTransmission();
    Wire.requestFrom(RTC_ADDR_I2C, 1);
    return (uint8_t)Wire.read();
}

void BaseClock::writeRegister(uint8_t address, uint8_t value)
{
    Wire.beginTransmission(RTC_ADDR_I2C);
    Wire.write(address);
    Wire.write(value);
    Wire.endTransmission();
}
