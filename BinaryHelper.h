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
#ifndef __UPSCALE_BINARY_HELPER_H__
#define __UPSCALE_BINARY_HELPER_H__

#include <stdint.h>

namespace Upscale {

/**
 * Bit manipulation functions namespace.
 *
 * Bit manipulation code can easily make the code difficult to read and maintain. Therefore, the purpose of these
 * functions is to provide descriptive and easy to use bit handling methods. Although it is not as fast as writing
 * the operations manually whenever necessary, they dramatically enhances the project readability.
 *
 * @author Daniel Murari Boatto
 */
namespace BinaryHelper {
    void setBitOn(uint8_t& value, uint8_t bit);
    void setBitOff(uint8_t& value, uint8_t bit);
    void toggleBit(uint8_t& value, uint8_t bit);
    bool isBitSet(uint8_t value, uint8_t bit);
    int16_t fromDecimalToBcd(int16_t value);
    int16_t fromBcdToDecimal(int16_t value);
}

} //end of namespace
#endif //__UPSCALE_BINARY_HELPER_H__
