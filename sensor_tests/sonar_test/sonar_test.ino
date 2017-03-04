
#include <Arduino.h>
#include <SoftwareSerial_PCINT12.h>

// Change to the proper excite (power) and recieve pin for MaxBotix Sonar.
int _dataPin = 10;
int _powerPin = 22;    // sensor power is pin 22 on Mayfly
int _triggerPin = -1;    // sensor power is pin 22 on Mayfly
bool useTrigger = true;
const int MAX_INPUT = 30;

// define serial port for recieving data
// output from maxSonar is inverted requiring true to be set.
SoftwareSerial sonarSerial(_dataPin, -1);


// here to process incoming serial data after a terminator received
void process_data (const char * data)
  {
  // for now just display it
  // (but you could compare it to some value, convert to an integer, etc.)
  Serial.println(data);
  }  // end of process_data

void processIncomingByte (const byte inByte)
  {
  static char input_line [MAX_INPUT];
  static unsigned int input_pos = 0;

  switch (inByte)
    {

    case '\r':   // end of text
      input_line [input_pos] = 0;  // terminating null byte

      // terminator reached! process input_line here ...
      process_data (input_line);

      // reset buffer for next time
      input_pos = 0;
      break;

    case '\n':   // discard carriage return
      break;

    default:
      // keep adding if not full ... allow for terminating null byte
      if (input_pos < (MAX_INPUT - 1))
        input_line [input_pos++] = inByte;
      break;

    }  // end of switch

  } // end of processIncomingByte


int SonarRead_char(void)
{
    // Sonar sends a result just above it's max range when it gets a bad reading
    // For 10m models, this is 9999, for 5m models it's 4999
    int badResult = 9999;
    int result = badResult;  // initialize the result with a bad reading
    char inData[5];  // char array to read data into
    int index = 0;
    bool stringComplete = false;
    int rangeAttempts = 0;

    Serial.println(F("Beginning detection for Sonar"));  // debug line
    sonarSerial.flush();  // Clear cache ready for next reading
    while (stringComplete == false)
    {
        while (sonarSerial.available())
        {
            delay(3);  // It just works better with this delay.  4 is too much, 2 is too little.
            char rByte = sonarSerial.read();  //read serial input for "R" to mark start of data
            if(rByte == 'R')
            {
                Serial.println(F("'R' Byte found, reading next 4 characters:")); // Debug line
                while (index < 4)  //read next three character for range from sensor
                {
                    if (sonarSerial.available())
                    {
                        inData[index] = sonarSerial.read();
                        Serial.print(inData[index]);  // Debug line
                        index++;  // Increment where to write next
                    }
                }
                inData[index] = 0x00;  //add a padding byte at end for atoi() function
                Serial.println();  // Debug line
            }
            rByte = 0;  // Reset the rByte ready for next reading
            index = 0;  // Reset index ready for next reading

            // Make sure R is not part of the header, part number, or RoHS warning line
            // ie, "HRXL-MaxSonar-WRL" or "RoHS 1.8b078  0713"
            if (inData[0] == 0) {}
            else if (inData[1] != 'X' && inData[1] != 'L' && inData[1] != 'S' &&
                     inData[1] != 'o' && inData[1] != 'H' && inData[1] != '\r')
            {
                stringComplete = true;  // Set completion of read to true
                result = atoi(inData);  // Changes string data into an integer for use
                memset(&inData[0], 0, sizeof(inData));  // Empty the inData array.
                if ((result == 300 || result == 500 || result == 4999 || result == 9999)
                    && rangeAttempts < 30)
                {
                    result = badResult;
                    stringComplete = false;
                    rangeAttempts++;
                    Serial.print(F("Bad or Suspicious Result, Retry Attempt #")); // Debug line
                    Serial.println(rangeAttempts); // Debug line
                }
            }
            else
                Serial.println(F("Ignoring header line")); // Debug line
                memset(&inData[0], 0, sizeof(inData));  // Empty the inData array.

        }
    }
    sonarSerial.flush();  // Clear cache ready for next reading
    return result;
}

void readMaxbotixHeader(void)
{
    Serial.println(F("Parsing Header Lines"));  // For debugging
    while (sonarSerial.available())
    {
        Serial.println(sonarSerial.readStringUntil('\r'));
        // sonarSerial.readStringUntil('\r');
    }
    Serial.println(F("  -----  "));  // For debugging
}

int readMaxbotixData(void)
{
    // Sonar sends a result just above it's max range when it gets a bad reading
    // For 10m models, this is 9999, for 5m models it's 4999
    int badResult = 9999;
    int result = badResult;  // initialize the result with a bad reading
    bool stringComplete = false;
    int rangeAttempts = 0;

    Serial.println(F("Beginning detection for Sonar"));  // For debugging
    while (stringComplete == false && rangeAttempts < 50)
    {
        if(useTrigger)
        {
            Serial.println(F("Triggering Sonar"));  // For debugging
            digitalWrite(_triggerPin, HIGH);
            delay(1);
            digitalWrite(_triggerPin, LOW);
            delay(160);  // Published return time is 158ms
        }

        result = sonarSerial.parseInt();
        sonarSerial.read();  // To throw away the carriage return
        Serial.println(result);  // For debugging
        rangeAttempts++;

        if (result == 0 || result == 300 || result == 500 || result == 4999 || result == 9999)
        {
            result = badResult;
            Serial.print(F("Bad or Suspicious Result, Retry Attempt #"));  // For debugging
            Serial.println(rangeAttempts);  // For debugging
        }
        else
        {
            Serial.println(F("Good result found"));  // For debugging
            stringComplete = true;  // Set completion of read to true
        }
    }
    return result;
}


void setup()
{
    pinMode(_powerPin, OUTPUT);
    pinMode(_dataPin, INPUT);
    digitalWrite(_powerPin, LOW);
    if(useTrigger)
    {
        pinMode(_triggerPin, OUTPUT);
        digitalWrite(_triggerPin, LOW);
    }
    else
    {
        pinMode(_triggerPin, OUTPUT);
        digitalWrite(_triggerPin, HIGH);
    }

    Serial.begin(9600);
    sonarSerial.begin(9600);
}

void loop()
{
    Serial.println(F("-------------------"));  // For debugging

    digitalWrite(_powerPin, HIGH);
    delay(2000);  // Published start-up time is 160ms;

    readMaxbotixHeader();
    readMaxbotixData();

    // SonarRead_char();
    // while (sonarSerial.available())
    // {
    //     processIncomingByte(sonarSerial.read());
    // }
    digitalWrite(_powerPin, LOW);
    Serial.println(F("------------------\n"));  // For debugging

    delay(5000);
}
