#include <Arduino.h>
#include <Adafruit_ADS1015.h>
Adafruit_ADS1115 ads;     /* Use this for the 16-bit version */

float lowturbidity, highturbidity;   //variables to hold the calculated NTU values
int16_t adc0, adc1; //  adc2, adc3;      //tells which channels are to be read
float lowvoltage, highvoltage;

void analogturbidity()     // function that takes reading from analog OBS3+ turbidity sensor
{

    adc0 = ads.readADC_SingleEnded(0);
    //now convert bits into millivolts
    lowvoltage = (adc0 * 3.3)/17585.0;
        //calibration information below if only for instrument SN# S9916
    lowturbidity =  (2.7323 * square (lowvoltage)) + (93.43 * lowvoltage) - 0.11966;

    Serial.print(F("ads.readADC_SingleEnded(0): "));
    Serial.print(adc0);
    Serial.print("\t\t");

    Serial.print(F("Voltage: "));
    Serial.print(lowvoltage);
    Serial.print("\t\t");

    Serial.print(F("calibResult: "));
    Serial.println(lowturbidity);


    adc1 = ads.readADC_SingleEnded(1);
    highvoltage = (adc1 * 3.3)/17585.0;
    //now convert bits into millivolts
        //calibration information below if only for instrument SN# S9916
    highturbidity = (20.042 * square (highvoltage)) + (389.08 * highvoltage) - 1.2897;

    Serial.print(F("ads.readADC_SingleEnded(1): "));
    Serial.print(adc1);
    Serial.print("\t\t");

    Serial.print(F("Voltage: "));
    Serial.print(highvoltage);
    Serial.print("\t\t");

    Serial.print(F("calibResult: "));
    Serial.println(highturbidity);
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

    analogturbidity();

    // Turn of sensor power
    digitalWrite(22, LOW);

    if (highturbidity < 255){
        Serial.print("    Diff: ");
        Serial.println(highturbidity - lowturbidity);
    }

    // Turn off the LED to show we're done with the reading
    digitalWrite(8, LOW);

    delay(1000);
}
