/*****************************************************************************
Apogee_SQ212_AnalogTest.ino
Written By:  Anthony Aufdenkampe
Adapted from mayfly_turbidity_test1.ino by Sara Damiano (sdamiano@stroudcenter.org)
Development Environment: PlatformIO 3.4
Hardware Platform: EnviroDIY Mayfly Arduino Datalogger
Software License: BSD-3.
  Copyright (c) 2017, Stroud Water Research Center (SWRC)
  and the EnviroDIY Development Team

Apogee SQ-212 Quantum Light sensor measures photosynthetically active radiation (PAR)
and is typically defined as total radiation across a range of 400 to 700 nm.
PAR is often expressed as photosynthetic photon flux density (PPFD):
photon flux in units of micromoles per square meter per second (μmol m-2 s-1,
equal to microEinsteins per square meter per second) summed from 400 to 700 nm.
https://www.apogeeinstruments.com/sq-212-amplified-0-2-5-volt-sun-calibration-quantum-sensor/

For derivation of the Analog to Digital Conversion Factor (17585) see:
https://github.com/EnviroDIY/ModularSensors/issues/54#issuecomment-306180076
https://github.com/adafruit/Adafruit_ADS1X15/blob/master/examples/singleended/singleended.pde

*****************************************************************************/

#include <Arduino.h>
#include <Adafruit_ADS1015.h>
Adafruit_ADS1115 ads;     /* Use this for the 16-bit version */

float PhotosyntheticPhotonFluxDensity;   //variable(s) to hold the calculated values
int16_t adc0; // adc1, adc2, adc3;      //tells which channels are to be read
float voltage;

void analogPAR()     // function that takes reading from analog OBS3+ turbidity sensor
{
    adc0 = ads.readADC_SingleEnded(0);
    //now convert bits into millivolts. Calibrated Output Range = 0 to 2.5 V
    voltage = adc0 * 0.1875/1000;  // in units of V. See Adafruit_ADS1X15 library for AD conversion factors.
              // Alternately = = adc0 * 3.3V / 17600.0 register bits,
              // where 3.3V is the voltage of the Mayfly board and 17600 = 3.3V / 0.1875 mV/bit
    //Calibration Factor (Reciprocal of Sensitivity) = 1.0 μmol m-2 s-1 per mV;
    PhotosyntheticPhotonFluxDensity =  1 * voltage * 1000 ;  // in units of μmol m-2 s-1

    Serial.print(F("ads.readADC_SingleEnded(0): "));
    Serial.print(adc0);
    Serial.print("\t\t");

    Serial.print(F("Voltage: "));
    Serial.print(voltage, 4);
    Serial.print("\t\t");

    Serial.print(F("calibResult: "));
    Serial.println(PhotosyntheticPhotonFluxDensity);
}


void setup() {
    Serial.begin(57600);
    ads.begin();       //begin adafruit ADS1015
    pinMode(22, OUTPUT);
    pinMode(8, OUTPUT);
}

void loop() {
    Serial.println("---------------------------------");
    // Power the sensor
    digitalWrite(22, HIGH);
    delay(1000);

    // Turn on the LED to show we're taking a reading
    digitalWrite(8, HIGH);

    analogPAR();
    delay(5000);

    // Turn off sensor power
    digitalWrite(22, LOW);

    // Turn off the LED to show we're done with the reading
    digitalWrite(8, LOW);

    delay(1000);
}
