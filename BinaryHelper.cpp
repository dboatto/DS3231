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
