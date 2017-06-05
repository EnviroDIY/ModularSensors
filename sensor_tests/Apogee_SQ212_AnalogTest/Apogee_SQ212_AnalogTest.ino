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
    voltage = (adc0 * 3.3)/17585.0;  // in units of V
        //Calibration Factor (Reciprocal of Sensitivity) = 1.0 μmol m-2 s-1 per mV;
    PhotosyntheticPhotonFluxDensity =  1 * voltage * 1000 ;  // in units of μmol m-2 s-1

    Serial.print(F("ads.readADC_SingleEnded(0): "));
    Serial.print(adc0);
    Serial.print("\t\t");

    Serial.print(F("Voltage: "));
    Serial.print(voltage);
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

    // Turn off sensor power
    digitalWrite(22, LOW);

    // Turn off the LED to show we're done with the reading
    digitalWrite(8, LOW);

    delay(1000);
}
