// 
// 
// 

#include <math.h>
#include "MayFlyExampleSensor1.h"
#define ANALOG_TEMP_PIN A5

MayFlyExampleSensor1::MayFlyExampleSensor1(void)
    : Sensor<float>("WindDir_Std")
{
    // Do nothing. Not because we can't, but we don't need to.
}

bool MayFlyExampleSensor1::update(void)
{
    Serial.println("Reading temperature");
    double pinValue = analogRead(ANALOG_TEMP_PIN);
    Serial.print("Raw: ");
    Serial.println(pinValue);

    double calculatedTemp = log(10000.0*((1024.0 / pinValue - 1)));
    Serial.println(calculatedTemp);
    calculatedTemp = 1 / (0.001129148 + (0.000234125 + (0.0000000876741 * calculatedTemp * calculatedTemp))* calculatedTemp);
    calculatedTemp = calculatedTemp - 273.15;
    calculatedTemp = (calculatedTemp * 9.0) / 5.0 + 32.0; // Convert to farenheit
    Serial.println(calculatedTemp);
    return true;
}

String MayFlyExampleSensor1::getValueAsString()
{
    return String(int(sensorValue));
}