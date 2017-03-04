#include <Arduino.h>
#include <SensorBase.h>
#include <DecagonCTD.h>
#include <Decagon5TM.h>

// Decagon CTD: pin settings
// sdi-12 data pin is usually, pin 7 on shield 3.0
const char *CTDSDI12address = "1";  // The SDI-12 Address of the CTD
const int numberReadings = 10;  // The number of readings to average
const int CTDData = 7;  // The pin the CTD is attached to
const int switchedPower = 22;  // sensor power is pin 22 on Mayfly

// Decagon 5TM: pin settings
// sdi-12 data pin is usually, pin 7 on shield 3.0
const char *TMSDI12address = "2";  // The SDI-12 Address of the 5-TM
const int TMData = 7;  // The pin the 5TM is attached to
// const int switchedPower = 22;  // sensor power is pin 22 on Mayfly

Decagon5TM_Ea ftm_ea(*TMSDI12address, switchedPower, TMData);
Decagon5TM_Temp ftm_temp(*TMSDI12address, switchedPower, TMData);
Decagon5TM_VWC ftm_vwc(*TMSDI12address, switchedPower, TMData);
DecagonCTD_Depth ctd_depth(*CTDSDI12address, switchedPower, CTDData);
DecagonCTD_Temp ctd_temp(*CTDSDI12address, switchedPower, CTDData);
DecagonCTD_Cond ctd_cond(*CTDSDI12address, switchedPower, CTDData);


const int GREEN_LED = 8;  // Pin for the green LED
const int RED_LED = 9;  // Pin for the red LED

// Flashes to Mayfly's LED's
void greenred4flash()
{
  for (int i = 1; i <= 4; i++) {
    digitalWrite(GREEN_LED, HIGH);
    digitalWrite(RED_LED, LOW);
    delay(50);
    digitalWrite(GREEN_LED, LOW);
    digitalWrite(RED_LED, HIGH);
    delay(50);
  }
}


// -----------------------------------------------
// Main setup function
// -----------------------------------------------
void setup()
{
    // Start the primary serial connection
    Serial.begin(9600);

    // Set up pins for the LED's
    pinMode(GREEN_LED, OUTPUT);
    pinMode(RED_LED, OUTPUT);
    // Blink the LEDs to show the board is on and starting up
    greenred4flash();

    // Print a start-up note to the first serial port
    Serial.println(F("WebSDL Device: EnviroDIY Mayfly"));
}

// -----------------------------------------------
// Main loop function
// -----------------------------------------------
void loop()
{
    // Print a line to show new reading
    Serial.println(F("------------------------------------------"));
    // Turn on the LED
    digitalWrite(GREEN_LED, HIGH);
    // Power the sensors;
    digitalWrite(switchedPower, HIGH);

    Serial.print("ftm_ea.getValue() ");
    Serial.println(ftm_ea.getValue());
    delay(random(5000));
    Serial.print("ctd_depth.getValue() ");
    Serial.println(ctd_depth.getValue());
    delay(random(5000));
    Serial.print("ctd_depth.getValue() ");
    Serial.println(ctd_depth.getValue());
    Serial.print("ftm_ea.getValue() ");
    Serial.println(ftm_ea.getValue());

    // Cut Power to the sensors;
    digitalWrite(switchedPower, LOW);
    // Turn off the LED
    digitalWrite(GREEN_LED, LOW);
    // Print a to close it off
    Serial.println(F("------------------------------------------\n"));

    // Wait for the next reading
    delay(random(10000));
}
