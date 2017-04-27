#include <Arduino.h>
#include <AOSongAM2315.h>


AOSongAM2315 am2315;

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
    digitalWrite(RED_LED, LOW);
  }
}

Variable *variables[] = {
    new AOSongAM2315_Humidity(&am2315),
    new AOSongAM2315_Temp(&am2315)
};
int variableCount = sizeof(variables) / sizeof(variables[0]);


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

    for (uint8_t i = 0; i < variableCount; i++)
    {
        variables[i]->setup();
    }

    delay(1000);
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

    bool success = true;
    for (uint8_t i = 0; i < variableCount; i++)
    {
        Serial.println(F("Updating"));
        success &= variables[i]->parentSensor->update();

        // Check for and skip the updates of any identical sensors
        for (int j = i+1; j < variableCount; j++)
        {
            if (variables[i]->parentSensor->getSensorName() == variables[j]->parentSensor->getSensorName() &&
                variables[i]->parentSensor->getSensorLocation() == variables[j]->parentSensor->getSensorLocation())
            {i++;}
            else {break;}
        }
    }
    // am2315.update();
    for (int i = 0; i < variableCount; i++)
    {
        Serial.print(variables[i]->getValue());
        Serial.println();
    }

    // Turn off the LED
    digitalWrite(GREEN_LED, LOW);
    // Print a to close it off
    Serial.println(F("------------------------------------------\n"));

    // Wait for the next reading
    delay(random(15000));
}
