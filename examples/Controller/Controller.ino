/**
 * This example shows how to control DS3231 from this library.
 * Most of the code here is used to interface with the Serial
 * port. Nevertheless, all methods of RealTimeClockController
 * are very simple and each of them require just a single
 * line to work.
 *
 * More information: https://github.com/dboatto/DS3231
 *
 * In order to use this example, open the Serial Monitor on
 * Arduino IDE (Ctrl+Shit+M).
 *
 * The following commands (they are case-sensitive) are available
 * (type them on Serial Monitor):
 *
 * enable battery
 *     Enables the battery-backed mode.
 *
 * disable battery
 *     Disables the battery-backed mode.
 *
 * enable 32khz
 *     Enables an output of a 32.768 kHz square-wave signal on the
 *     correspondent pin of DS3231.
 *
 * disable 32khz
 *     Disables the 32 KHz output.
 *
 * enable sqw <frequency>
 *     Enables the square-wave output at a given frequency. Check
 *     the documentation for the frquencies available. Example:
 *     enable sqw 4096
 *
 * disable sqw
 *     Disables the square-wave output.
 *
 * enable btsqw <frequency>
 *     Enables the battery-backed square-wave output. Example:
 *     enable btsqw 8192
 *
 * disable btsqw
 *     Disables the battery-backed square-wave mode.
 *
 * calibration <value>
 *     Writes a value in the aging offset register. Example:
 *     calibration 75
 */
#include <Arduino.h>
#include "RealTimeClockController.h"

//All library classes are inside namespaces.
//Thefore, use the following statement to import them.
using namespace Ampliar::DS3231;

//This statement creates an instance of RealTimeClockController, used to
//access DS3231 advanced control features.
RealTimeClockController controller;

// ---------------------------------------------------------------------------
// Function prototypes
// ---------------------------------------------------------------------------
RealTimeClockController::Frequency parseFrequency(const String& commandPrefix, const String& command);
void readCommands();
void printStatus();
// ---------------------------------------------------------------------------

void setup()
{
    Serial.begin(9600);
}

void loop()
{
    readCommands();
    printStatus();
    delay (1000); //delays 1 (one) second
}

/**
 * Reads commands inputed by the serial interface and executes
 * the correspondent function on DS3231.
 */
void readCommands()
{
    if (Serial.available() > 0)
    {
        String command = Serial.readString();

        if (command == "enable battery")
        {
            controller.enableBattery();
        }
        else if (command == "disable battery")
        {
            controller.disableBattery();
        }
        else if (command == "enable 32khz")
        {
            controller.enable32khzOutput();
        }
        else if (command == "disable 32khz")
        {
            controller.disable32khzOutput();
        }
        else if (command.startsWith("enable sqw"))
        {
            controller.enableSquareWave(parseFrequency("enable sqw", command));
        }
        else if (command == "disable sqw")
        {
            controller.disableSquareWave();
        }
        else if (command.startsWith("enable btsqw"))
        {
            controller.enableBatteryBackedSquareWave(parseFrequency("enable btsqw", command));
        }
        else if (command == "disable btsqw")
        {
            controller.disableBatteryBackedSquareWave();
        }
        else if (command.startsWith("calibration"))
        {
            int8_t calibration = (int8_t)command.substring(11).toInt();
            controller.writeCalibration(calibration);
        }
        else
        {
            Serial.println("");
            Serial.print("Unknown command: ");
            Serial.println(command);
            delay (2000); //delays 1 (one) second
        }
    }
}

/**
 * Prints internal states of DS3231 on serial interface.
 */
void printStatus()
{
    Serial.print("battery: ");
    Serial.print(controller.isBatteryEnabled() ? "yes" : "no");
    //
    Serial.print("; 32kHz output: ");
    Serial.print(controller.is32khzOutputEnabled() ? "yes" : "no");
    //
    Serial.print("; SQ wave: ");
    Serial.print(controller.isSquareWaveEnabled() ? "yes" : "no");
    //
    Serial.print("; freq: ");
    switch (controller.getSquareWaveFrequency())
    {
        case RealTimeClockController::FREQ_1HZ:
            Serial.print("1 Hz");
            break;

        case RealTimeClockController::FREQ_1024KHZ:
            Serial.print("1 kHz");
            break;

        case RealTimeClockController::FREQ_4096KHZ:
            Serial.print("4096 kHz");
            break;

        case RealTimeClockController::FREQ_8192KHZ:
            Serial.print("8192 kHz");
            break;
    }
    //
    Serial.print("; Battery-Backed SQ wave: ");
    Serial.print(controller.isBatteryBackedSquareWaveEnabled() ? "yes" : "no");
    //
    Serial.print("; Calibration: ");
    Serial.println(controller.readCalibration());
}

/**
 * Parses the frequency informed in a command.
 *
 * If an invalid frequency is provided, it returns 1 Hz.
 *
 * @param  commandPrefix The command text before the actual frequency string.
 *                       For instance: "enable sqw" for an expected command
 *                       like "enable sqw 4096".
 * @param  command       The command received from the serial interface.
 * @return               The square-wave frequency.
 */
RealTimeClockController::Frequency parseFrequency(const String& commandPrefix, const String& command)
{
    long frequency = command.substring(commandPrefix.length()).toInt();

    switch (frequency)
    {
        case 1024:
            return RealTimeClockController::FREQ_1024KHZ;

        case 4096:
            return RealTimeClockController::FREQ_4096KHZ;

        case 8192:
            return RealTimeClockController::FREQ_8192KHZ;

        case 1:
        default:
            return RealTimeClockController::FREQ_1HZ;
    }
}
