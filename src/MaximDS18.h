/*
 * MaximDS18.h
 * This file is part of the EnviroDIY modular sensors library for Arduino
 *
 * Initial library developement done by Sara Damiano (sdamiano@stroudcenter.org).
 *
 * This file is for the Maxim DS18B20, DS18S20, MAX31820, DS1822, and DS1820 Temperature sensors.
 * It is dependent on Dallas Temperature library, which itself is dependent on
 * the OneWire library.
 *
 * The resolution of the DS18B20, DS1822, and MAX31820 temperature sensors are
 * user-configurable to 9, 10, 11, or 12 bits, corresponding to increments of
 * 0.5°C, 0.25°C, 0.125°C, and 0.0625°C, respectively.
 * The default resolution at power-up is 12-bit.
 * The resolution of the DS18S20 is set at 9-bit
 *
 * Accuracy is ± 0.5°C from -10°C to +85°C for DS18S20 and DS18B20
 *             ± 2°C for DS1822 and MAX31820
 *
 * Time to take reading at 12-bit: 750ms (likely ready to begin conversion
 * much before that, but I can't find a number.)
*/

#ifndef MaximDS18_h
#define MaximDS18_h

#include "SensorBase.h"
#include "VariableBase.h"

// #define DEBUGGING_SERIAL_OUTPUT Serial
#include "ModSensorDebugger.h"

#include <OneWire.h>
#include <DallasTemperature.h>

#define DS18_NUM_MEASUREMENTS 1
#define DS18_WARM_UP 750
#define DS18_TEMP_VAR_NUM 0
#define DS18_TEMP_RESOLUTION 4

// The main class for the DS18
class MaximDS18 : public Sensor
{
public:
    MaximDS18(DeviceAddress OneWireAddress, int powerPin, int dataPin);
    MaximDS18(int powerPin, int dataPin);

    SENSOR_STATUS setup(void) override;
    String getSensorLocation(void) override;
    SENSOR_STATUS getStatus(void) override;

    bool update(void) override;

private:
    DeviceAddress _OneWireAddress;
    bool _addressKnown;
    // Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
    // OneWire oneWire;
    // Pass our oneWire reference to Dallas Temperature.
    // DallasTemperature tempSensors;
    // Turns the address into a printable string
    String getAddressString(DeviceAddress OneWireAddress);
};


// The class for the Temperature Variable
class MaximDS18_Temp : public Variable
{
public:
    MaximDS18_Temp(Sensor *parentSense,
                   String UUID = "", String customVarCode = "")
      : Variable(parentSense, DS18_TEMP_VAR_NUM,
               F("temperature"), F("degreeCelsius"),
               DS18_TEMP_RESOLUTION,
               F("DS18Temp"), UUID, customVarCode)
    {}
};

#endif
