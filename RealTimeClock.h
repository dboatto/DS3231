#ifndef __REALTIMECLOCK_H__
#define __REALTIMECLOCK_H__

#include <stdint.h>
#include <Wire.h>
#include "BinaryHelper.h"

enum SquareWaveFrequency
{
    FREQ_1HZ,
    FREQ_1024KHZ,
    FREQ_4096KHZ,
    FREQ_8192KHZ
};

class RealTimeClock
{
public:
	RealTimeClock();
	~RealTimeClock();
    //
    void setDateTime(int16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second);
    void syncDateTime();
    bool wasItStopped() const;
    //
    float getTemperature() const;
    //
    void enableBattery() const;
    void disableBattery() const;
    bool isBatteryEnabled() const;
    //
    void enable32khzOutput() const;
    void disable32khzOutput() const;
    bool is32khzOutputEnabled() const;
    //
    void enableSquareWave(SquareWaveFrequency frequency) const;
    void disableSquareWave() const;
    bool isSquareWaveEnabled() const;
    SquareWaveFrequency getSquareWaveFrequency() const;
    //
    void enableBatteryBackedSquareWave(SquareWaveFrequency frequency) const;
    void disableBatteryBackedSquareWave() const;
    bool isBatteryBackedSquareWaveEnabled() const;
    //
    uint8_t getSecond() const;
    uint8_t getMinute() const;
    uint8_t getHour() const;
    uint8_t getDay() const;
    uint8_t getMonth() const;
    int16_t getYear() const;
    uint8_t getDayOfWeek() const;

private:
    uint8_t _second;
    uint8_t _minute;
    uint8_t _day;
    uint8_t _hour;
    uint8_t _month;
    uint8_t _dayOfWeek;
    int16_t _year;
    uint8_t calculateDayOfWeek(int16_t year, uint8_t month, uint8_t day) const;
    void clearOscillatorStopFlag() const;
    //
    void toggleBattery(bool on) const;
    void toggle32khzOutput(bool on) const;
    void toggleSquareWave(bool on, SquareWaveFrequency frequency) const;
    void toggleBatteryBackedSquareWave(bool on, SquareWaveFrequency frequency) const;
    //
    uint8_t getRegister(uint8_t address) const;
    void setRegister(uint8_t address, uint8_t value) const;
};

#endif //__REALTIMECLOCK_H__
