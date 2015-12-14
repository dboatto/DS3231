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
#include "BinaryHelper.h"

BinaryHelper::BinaryHelper()
{
}

int16_t BinaryHelper::fromDecimalToBcd(int16_t value)
{
    return (value / 10 * 16) + (value % 10);
}

int16_t BinaryHelper::fromBcdToDecimal(int16_t value)
{
    return (value / 16 * 10) + (value % 16);
}

uint8_t BinaryHelper::setBitOn(uint8_t value, uint8_t bit)
{
    return value | ( 1 << bit);
}

uint8_t BinaryHelper::setBitOff(uint8_t value, uint8_t bit)
{
    return value & ~( 1 << bit);
}

uint8_t BinaryHelper::toggleBit(uint8_t value, uint8_t bit)
{
    return value ^ (1 << bit);
}

bool BinaryHelper::istBitSet(uint8_t value, uint8_t bit)
{
    return (value >> bit) & 1;
}
