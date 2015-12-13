#ifndef BINARYHELPER_H_
#define BINARYHELPER_H_
#include <stdint.h>

class BinaryHelper
{
public:
	static uint8_t setBitOn(uint8_t value, uint8_t bit);
	static uint8_t setBitOff(uint8_t value, uint8_t bit);
	static uint8_t toggleBit(uint8_t value, uint8_t bit);
	static bool istBitSet(uint8_t value, uint8_t bit);
    static int16_t fromDecimalToBcd(int16_t value);
    static int16_t fromBcdToDecimal(int16_t value);

private:
	BinaryHelper();
};

#endif /* BINARYHELPER_H_ */
