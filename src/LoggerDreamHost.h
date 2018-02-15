/*
 *LoggerDreamHost.h
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Initial library developement done by Sara Damiano (sdamiano@stroudcenter.org).
 *
 *This file is for the EnviroDIY logging functions - ie, sending get requests to DreamHost
*/

#ifndef LoggerDreamHost_h
#define LoggerDreamHost_h

#include <Arduino.h>

// #define DEBUGGING_SERIAL_OUTPUT Serial
#include "ModSensorDebugger.h"

#include "LoggerEnviroDIY.h"


// ============================================================================
//  Functions for the SWRC Sensors DreamHost data receivers.
// ============================================================================
class LoggerDreamHost : public LoggerEnviroDIY
{
public:
    // Functions for private SWRC server
    void setDreamHostPortalRX(const char *URL)
    {
        _DreamHostPortalRX = URL;
        MS_DBG(F("Dreamhost portal URL set!\n"));
    }

    // This creates all of the URL parameters
    String generateSensorDataDreamHost(void)
    {
        String dhString = String(_DreamHostPortalRX);
        dhString += F("?LoggerID=");
        dhString += String(Logger::_loggerID);
        dhString += F("&Loggertime=");
        dhString += String(Logger::markedEpochTime - 946684800);  // Coorect time from epoch to y2k

        for (int i = 0; i < Logger::_variableCount; i++)
        {
            dhString += F("&");
            dhString += Logger::_variableList[i]->getVarCode();
            dhString += F("=");
            dhString += Logger::_variableList[i]->getValueString();
        }
        return dhString;
    }

    // Communication functions
    void streamDreamHostRequest(Stream *stream)
    {
        stream->print(String(F("GET ")));
        stream->print(generateSensorDataDreamHost());
        stream->print(String(F("  HTTP/1.1")));
        stream->print(String(F("\r\nHost: swrcsensors.dreamhosters.com")));
        stream->print(String(F("\r\n\r\n")));
    }

#if defined(USE_TINY_GSM)
    // Post the data to dream host.
    int postDataDreamHost(void)
    {
        // Create a buffer for the response
        char response_buffer[12] = "";
        int did_respond = 0;

        // Open a TCP/IP connection to DreamHost
        if(_logModem.openTCP("swrcsensors.dreamhosters.com", 80))
        {
            // Send the request to the serial for debugging
            #if defined(STANDARD_SERIAL_OUTPUT)
                PRINTOUT(F("\n \\/------ Data to DreamHost ------\\/ \n"));
                streamDreamHostRequest(&STANDARD_SERIAL_OUTPUT);  // for debugging
                STANDARD_SERIAL_OUTPUT.flush();  // for debugging
            #endif

            // Send the request to the modem stream
            streamDreamHostRequest(_logModem._client);
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
        else PRINTOUT(F("\n -- Unable to Establish Connection to DreamHost -- \n"));

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

                // Post the data to DreamHost
                postDataDreamHost();

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
    const char *_DreamHostPortalRX;
};

#endif
