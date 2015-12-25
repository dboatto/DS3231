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

using namespace Upscale;

/**
 * Converts a base-10 integer to BCD.
 *
 * This method converts a base-10 integer to BCD (Binary-Coded Decimal)
 *
 * @param value Base-10 integer
 * @return BCD representation of the value.
 */
int16_t BinaryHelper::fromDecimalToBcd(int16_t value)
{
    return (value / 10 * 16) + (value % 10);
}

/**
 * Converts a BCD value to a base-10 integer.
 *
 * This method converts a BCD (Binary-Coded Decimal) value to a base-10 integer.
 *
 * @param value BCD value
 * @return Base-10 integer
 */
int16_t BinaryHelper::fromBcdToDecimal(int16_t value)
{
    return (value / 16 * 10) + (value % 16);
}

/**
 * Sets a given bit on.
 *
 * This method sets one bit, at a given position, to 1 (one).
 *
 * @param value Value in which the bit will be set.
 * @param bit Position of the bit (zero based).
 */
void BinaryHelper::setBitOn(uint8_t& value, uint8_t bit)
{
    value |= ( 1 << bit);
}

/**
 * Sets a given bit off.
 *
 * This method sets one bit, at a given position, to 0 (one).
 *
 * @param value Value in which the bit will be set.
 * @param bit Position of the bit (zero based).
 */
void BinaryHelper::setBitOff(uint8_t& value, uint8_t bit)
{
    value &= ~( 1 << bit);
}

/**
 * Toggles a bit in a given position.
 *
 * This method toggles one bit at a given position. If the bit is 0 (zero), it will be set to 1 (one). On the other
 * hand, if the bit is 1 (one), it will be set to 0 (zero).
 *
 * @param value Value in which the bit will be set.
 * @param bit Position of the bit (zero based).
 */
void BinaryHelper::toggleBit(uint8_t& value, uint8_t bit)
{
    value ^= (1 << bit);
}

/**
 * Check if a given bit is set.
 *
 * This method checks if a given bit is set, i.e, if it is 1 (one).
 *
 * @param value Value to be checked.
 * @param bit Position of the bit (zero based).
 * @return true if the bit is set (1), false otherwise.
 */
bool BinaryHelper::isBitSet(uint8_t value, uint8_t bit)
{
    return (value >> bit) & 1;
}
