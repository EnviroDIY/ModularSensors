/*
 *LoggerEnviroDIY.h
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Initial library developement done by Sara Damiano (sdamiano@stroudcenter.org).
 *
 *This file is for the EnviroDIY logging functions - ie, sending JSON data to
 * http://data.enviroDIY.org
*/

#ifndef LoggerEnviroDIY_h
#define LoggerEnviroDIY_h

#include <Arduino.h>
#include "LoggerBase.h"
#include "ModemSupport.h"

// Defines the "Logger" Class
class LoggerEnviroDIY : public Logger
{
public:
    // Set up communications
    void setToken(const char *registrationToken)
    {
        _registrationToken = registrationToken;
        DBGVA(F("Registration token set!\n"));
    }

    void setSamplingFeature(const char *samplingFeature)
    {
        _samplingFeature = samplingFeature;
        DBGVA(F("Sampling feature token set!\n"));
    }

    void setUUIDs(const char *UUIDs[])
    {
        _UUIDs = UUIDs;
        DBGVA(F("UUID array set!\n"));
    }

    // // This adds extra data to the datafile header
    String generateFileHeader(void)
    {
        String dataHeader = "";
        dataHeader += Logger::generateFileHeader();
        dataHeader += F("\r\n");

        // Add additional UUID information
        dataHeader += F("\"Sampling Feature: ");
        dataHeader += _samplingFeature;
        dataHeader += F("\"");
        for (uint8_t i = 0; i < _variableCount; i++)
        {
            dataHeader += F("\"");
            dataHeader += String(_UUIDs[i]);
            dataHeader += F("\"");
            if (i + 1 != _variableCount)
            {
                dataHeader += F(", ");
            }
        }
        return dataHeader;
    }

    // This generates a properly formatted JSON for EnviroDIY
    String generateSensorDataJSON(void)
    {
        String jsonString = F("{");
        jsonString += F("\"sampling_feature\": \"");
        jsonString += String(_samplingFeature) + F("\", ");
        jsonString += F("\"timestamp\": \"");
        jsonString += String(Logger::markedISO8601Time) + F("\", ");

        for (int i = 0; i < Logger::_variableCount; i++)
        {
            jsonString += F("\"");
            jsonString += String(_UUIDs[i]) + F("\": ");
            jsonString += Logger::_variableList[i]->getValueString();
            if (i + 1 != Logger::_variableCount)
            {
                jsonString += F(", ");
            }
        }

        jsonString += F(" }");
        return jsonString;
    }


#if defined(TINY_GSM_MODEM_SIM800) || defined(TINY_GSM_MODEM_SIM900) || \
    defined(TINY_GSM_MODEM_A6) || defined(TINY_GSM_MODEM_A7) || \
    defined(TINY_GSM_MODEM_M590) || defined(TINY_GSM_MODEM_ESP8266) || \
    defined(TINY_GSM_MODEM_XBEE)

    // Create the modem instance
    loggerModem modem;

    // Public function to send data
    int postDataEnviroDIY(void)
    {
        modem.connect("data.envirodiy.org", 80);

        // Send the request to the serial for debugging
        PRINTOUT(F("\n \\/---- Post Request to EnviroDIY ----\\/ \n"));
        streamEnviroDIYRequest(&Serial);  // for debugging
        Serial.flush();  // for debugging

        // Send the request to the modem stream
        modem.dumpBuffer(modem._modemStream);
        streamEnviroDIYRequest(modem._modemStream);
        modem._modemStream->flush();  // wait for sending to finish

        // Wait for at least the first 12 characters to make it across
        unsigned long timeout = 1500;
        for (unsigned long start = millis(); millis() - start < timeout; )
        {
            if (modem._modemStream->available() >= 12) break;
        }

        // Process the HTTP response
        int responseCode = 0;
        if (modem._modemStream->available() >= 12)
        {
            modem._modemStream->readStringUntil(' ');  // Throw away the "HTTP/1.1"
            responseCode = modem._modemStream->readStringUntil(' ').toInt();
            modem.dumpBuffer(modem._modemStream);
        }
        else responseCode=504;

        PRINTOUT(F(" -- Response Code -- \n"));
        PRINTOUT(responseCode, F("\n"));

        modem.stop();

        return responseCode;
    }

    // Convience functions to do it all
    virtual void log(void) override
    {
        // Check of the current time is an even interval of the logging interval
        if (checkInterval())
        {
            // Print a line to show new reading
            PRINTOUT(F("------------------------------------------\n"));
            // Turn on the LED to show we're taking a reading
            digitalWrite(_ledPin, HIGH);

            // Turn on the modem to let it start searching for the network
            LoggerEnviroDIY::modem.modemOnOff->on();

            // Wake up all of the sensors
            // I'm not doing as part of sleep b/c it may take up to a second or
            // two for them all to wake which throws off the checkInterval()
            sensorsWake();
            // Update the values from all attached sensors
            updateAllSensors();
            // Immediately put sensors to sleep to save power
            sensorsSleep();

            // Create a csv data record and save it to the log file
            logToSD(generateSensorDataCSV());

            // Connect to the network
            if (modem.connectNetwork())
            {
                // Post the data to the WebSDL
                postDataEnviroDIY();

                // Print the response from the WebSDL
                // int result = postDataEnviroDIY();
                // modem.printHTTPResult(result);  // for debugging
            }
            // Disconnect from the network
            modem.disconnectNetwork();

            // Turn on the modem off
            modem.modemOnOff->off();

            // Turn off the LED
            digitalWrite(_ledPin, LOW);
            // Print a line to show reading ended
            PRINTOUT(F("------------------------------------------\n\n"));
        }

        // Sleep
        if(_sleep){systemSleep();}
    }

#endif


private:
    // Communication functions
    void streamEnviroDIYRequest(Stream *stream)
    {
        stream->print(String(F("POST /api/data-stream/ HTTP/1.1")));
        stream->print(String(F("\r\nHost: data.envirodiy.org")));
        stream->print(String(F("\r\nTOKEN: ")) + String(_registrationToken));
        // stream->print(String(F("\r\nCache-Control: no-cache")));
        // stream->print(String(F("\r\nConnection: close")));
        stream->print(String(F("\r\nContent-Length: ")) + String(generateSensorDataJSON().length()));
        stream->print(String(F("\r\nContent-Type: application/json\r\n\r\n")));
        stream->print(String(generateSensorDataJSON()));
        stream->print(String(F("\r\n\r\n")));
    }

    // Tokens and UUID's for EnviroDIY
    const char *_registrationToken;
    const char *_samplingFeature;
    const char **_UUIDs;
};

#endif
