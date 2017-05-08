#include <Arduino.h>
#include <Adafruit_ADS1015.h>
Adafruit_ADS1115 ads;     /* Use this for the 16-bit version */

float lowturbidity, highturbidity;   //variables to hold the calculated NTU values


void setup() {
  // put your setup code here, to run once:
  Serial.begin(57600);
  ads.begin();       //begin adafruit ADS1015
  pinMode(22, OUTPUT);
  digitalWrite(22, HIGH);

}

void loop() {

            analogturbidity();

  Serial.print("Low: ");
  Serial.print(lowturbidity);
  Serial.print("    High: ");
  Serial.println(highturbidity);


  delay(500);
  // put your main code here, to run repeatedly:

}



void analogturbidity()     // function that takes reading from analog OBS3+ turbidity sensor
{
 int16_t adc0, adc1; //  adc2, adc3;      //tells which channels are to be read

  adc0 = ads.readADC_SingleEnded(0);
  adc1 = ads.readADC_SingleEnded(1);

//now convert bits into millivolts
 float lowvoltage = (adc0 * 3.3)/17585.0;
 float highvoltage = (adc1 * 3.3)/17585.0;

  //calibration information below if only for instrument SN# S9916
 lowturbidity =  (2.7323 * square (lowvoltage)) + (93.43 * lowvoltage) - 0.11966;
 highturbidity = (20.042 * square (highvoltage)) + (389.08 * highvoltage) - 1.2897;
}
