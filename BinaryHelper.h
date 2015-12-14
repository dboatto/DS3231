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
