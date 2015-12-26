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
#ifndef __AMPLIAR_DS3231_BASE_CLOCK_H__
#define __AMPLIAR_DS3231_BASE_CLOCK_H__

#include <stdint.h>
#include <Wire.h>

namespace Ampliar { namespace DS3231 {

#define RTC_ADDR_I2C         0x68 ///< DS3231 (Slave) Address
#define RTC_ADDR_DATE        0x00 ///< Date/Time Register Address
#define RTC_ADDR_ALARM1      0x07 ///< Alarm 1 Register Address
#define RTC_ADDR_ALARM2      0x0B ///< Alarm 2 Register Address
#define RTC_ADDR_TEMPERATURE 0x11 ///< Temperature Register Address
#define RTC_ADDR_STATUS      0x0F ///< Status Register Address
#define RTC_ADDR_CONTROL     0x0E ///< Control Register Address
#define RTC_ADDR_AGING       0x10 ///< Aging Register Address

#define RTC_REG_STATUS_A1F     0 ///< Alarm 1 Flag (A1F)
#define RTC_REG_STATUS_A2F     1 ///< Alarm 2 Flag (A2F)
#define RTC_REG_STATUS_BSY     2 ///< Busy (BSY)
#define RTC_REG_STATUS_EN32KHZ 3 ///< Enable 32kHz Output (EN32kHz)
#define RTC_REG_STATUS_OSF     7 ///< Oscillator Stop Flag (OSF)

#define RTC_REG_CONTROL_A1IE   0 ///< Alarm 1 Interrupt Enable (A1IE)
#define RTC_REG_CONTROL_A2IE   1 ///< Alarm 2 Interrupt Enable (A2IE)
#define RTC_REG_CONTROL_INTCN  2 ///< Interrupt Control (INTCN)
#define RTC_REG_CONTROL_RS1    3 ///< Rate Select (RS1)
#define RTC_REG_CONTROL_RS2    4 ///< Rate Select (RS2)
#define RTC_REG_CONTROL_CONV   5 ///< Convert Temperature (CONV)
#define RTC_REG_CONTROL_BBSQW  6 ///< Battery-Backed Square-Wave Enable (BBSQW)
#define RTC_REG_CONTROL_EOSC   7 ///< Enable Oscillator (EOSC)

/**
 * Abstract class conceived to encapsulate low-level operations and configuration.
 *
 * This header file contains all registers addresses and flags bit-mapping of DS3231. Additionally, it contains methods
 * to abstract the I2C low-level operations to read and write registers.
 *
 * @author Daniel Murari Boatto
 */
class BaseClock
{
protected:
    BaseClock();
    static uint8_t readRegister(uint8_t address);
    static void writeRegister(uint8_t address, uint8_t value);
};

}} //end of namespace
#endif //__AMPLIAR_DS3231_BASE_CLOCK_H__
