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

// #define DEBUGGING_SERIAL_OUTPUT Serial
#include "ModSensorDebugger.h"

#include "LoggerBase.h"

// ============================================================================
//  Functions for the EnviroDIY data portal receivers.
// ============================================================================
class LoggerEnviroDIY : public Logger
{
public:
    // Set up communications
    void setToken(const char *registrationToken)
    {
        _registrationToken = registrationToken;
        MS_DBG(F("Registration token set!\n"));
    }

    void setSamplingFeatureUUID(const char *samplingFeature)
    {
        _samplingFeature = samplingFeature;
        MS_DBG(F("Sampling feature UUID set!\n"));
    }

    // This adds extra data to the datafile header
    String generateFileHeader(void)
    {
        String dataHeader = "";

        // Add additional UUID information
        String  SFHeaderString = F("Sampling Feature: ");
        SFHeaderString += _samplingFeature;
        makeHeaderRowMacro(SFHeaderString, _variableList[i]->getVarUUID())

        // Put the basic header below
        dataHeader += Logger::generateFileHeader();

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
            jsonString += Logger::_variableList[i]->getVarUUID() + F("\": ");
            jsonString += Logger::_variableList[i]->getValueString();
            if (i + 1 != Logger::_variableCount)
            {
                jsonString += F(", ");
            }
        }

        jsonString += F(" }");
        return jsonString;
    }

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
    }

#if defined(USE_TINY_GSM)
    // Public function to send data
    int postDataEnviroDIY(void)
    {
        // Create a buffer for the response
        char response_buffer[12] = "";
        int did_respond = 0;

        // Open a TCP/IP connection to the Enviro DIY Data Portal (WebSDL)
        if(_logModem.openTCP("data.envirodiy.org", 80))
        {
            // Send the request to the serial for debugging
            #if defined(STANDARD_SERIAL_OUTPUT)
                PRINTOUT(F("\n \\/---- Post Request to EnviroDIY ----\\/ \n"));
                streamEnviroDIYRequest(&STANDARD_SERIAL_OUTPUT);  // for debugging
                PRINTOUT(F("\r\n\r\n"));
                STANDARD_SERIAL_OUTPUT.flush();  // for debugging
            #endif

            // Send the request to the modem stream
            streamEnviroDIYRequest(_logModem._client);
            _logModem._client->flush();  // wait for sending to finish

            uint32_t start_timer;
            if (millis() < 4294957296) start_timer = millis();  // In case of roll-over
            else start_timer = 0;
            while ((millis() - start_timer) < 10000L && _logModem._client->available() < 12)
            {delay(10);}

            // Read only the first 12 characters of the response
            // We're only reading as far as the http code, anything beyond that
            // we don't care about so we're not reading to save on total
            // data used for transmission.
            did_respond = _logModem._client->readBytes(response_buffer, 12);

            // Close the TCP/IP connection as soon as the first 12 characters are read
            // We don't need anything else and stoping here should save data use.
            _logModem.closeTCP();
        }
        else PRINTOUT(F("\n -- Unable to Establish Connection to EnviroDIY Data Portal -- \n"));

        // Process the HTTP response
        int responseCode = 0;
        if (did_respond > 0)
        {
            char responseCode_char[4];
            for (int i = 0; i < 3; i++)
            {
                responseCode_char[i] = response_buffer[i+9];
            }
            responseCode = atoi(responseCode_char);
        }
        else responseCode=504;

        PRINTOUT(F(" -- Response Code -- \n"));
        PRINTOUT(responseCode, F("\n"));

        return responseCode;
    }

    // ===================================================================== //
    // Convience functions to call several of the above functions
    // ===================================================================== //

    // This is a one-and-done to log data
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
            _logModem.wake();

            // Send power to all of the sensors
            MS_DBG(F("    Powering sensors...\n"));
            sensorsPowerUp();
            // Wake up all of the sensors
            MS_DBG(F("    Waking sensors...\n"));
            sensorsWake();
            // Update the values from all attached sensors
            MS_DBG(F("  Updating sensor values...\n"));
            updateAllSensors();
            // Put sensors to sleep
            MS_DBG(F("  Putting sensors back to sleep...\n"));
            sensorsSleep();
            // Cut sensor power
            MS_DBG(F("  Cutting sensor power...\n"));
            sensorsPowerDown();

            // Connect to the network
            if (_logModem.connectInternet())
            {
                // Post the data to the WebSDL
                postDataEnviroDIY();

                // Sync the clock every 288 readings (1/day at 5 min intervals)
                if (_numTimepointsLogged % 288 == 0)
                {
                    syncRTClock(_logModem.getNISTTime());
                }

                // Disconnect from the network
                _logModem.disconnectInternet();
            }

            // Turn the modem off
            _logModem.off();

            // Create a csv data record and save it to the log file
            logToSD(generateSensorDataCSV());

            // Turn off the LED
            digitalWrite(_ledPin, LOW);
            // Print a line to show reading ended
            PRINTOUT(F("------------------------------------------\n\n"));
        }

        // Sleep
        if(_sleep){systemSleep();}
    }
#endif /* USE_TINY_GSM */

private:
    // Tokens and UUID's for EnviroDIY
    const char *_registrationToken;
    const char *_samplingFeature;
};

#endif
