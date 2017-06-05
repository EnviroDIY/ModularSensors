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

#include "LoggerBase.h"
#include "ModemSupport.h"

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

    // This adds extra data to the datafile header
    String generateFileHeader(void)
    {
        String dataHeader = "";

        // Add additional UUID information
        String  SFHeaderString = F("Sampling Feature: ");
        SFHeaderString += _samplingFeature;
        makeHeaderRowMacro(SFHeaderString, String(_UUIDs[i]))

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


#if defined(USE_TINY_GSM)

    // Create the modem instance
    loggerModem modem;

    // This defines what to do in the debug mode
    virtual void debugMode(Stream *stream = &Serial)
    {
        PRINTOUT(F("------------------------------------------\n"));
        PRINTOUT(F("Entering debug mode\n"));

        // Turn on the modem to let it start searching for the network
        modem.on();

        // Update the sensors and print out data 25 times
        for (uint8_t i = 0; i < 25; i++)
        {
            stream->println(F("------------------------------------------"));
            // Wake up all of the sensors
            sensorsWake();
            // Update the values from all attached sensors
            updateAllSensors();
            // Immediately put sensors to sleep to save power
            sensorsSleep();
            // Print out the current logger time
            stream->print(F("Current logger time is "));
            stream->println(formatDateTime_ISO8601(getNow()));
            stream->println(F("    -----------------------"));
            // Print out the sensor data
            printSensorData(stream);
            stream->println(F("    -----------------------"));

            #if defined(TINY_GSM_MODEM_SIM800) || defined(TINY_GSM_MODEM_SIM900) || \
                defined(TINY_GSM_MODEM_A6) || defined(TINY_GSM_MODEM_A7) || \
                defined(TINY_GSM_MODEM_M590)
            // Print out the modem connection strength
            int signalQual = modem._modem->getSignalQuality();
            stream->print(F("Current modem signal is "));
            stream->print(signalQual);
            stream->print(F(" ("));
            stream->print(modem.getPctFromCSQ(signalQual));
            stream->println(F("%)"));
            #endif
            delay(5000);
        }

        // Turn off the modem
        modem.off();
    }

    // Public function to send data
    int postDataEnviroDIY(void)
    {
        modem.connect("data.envirodiy.org", 80);

        // Send the request to the serial for debugging
        PRINTOUT(F("\n \\/---- Post Request to EnviroDIY ----\\/ \n"));
        streamEnviroDIYRequest(&Serial);  // for debugging
        Serial.flush();  // for debugging

        // Send the request to the modem stream
        modem.dumpBuffer(modem.stream);
        streamEnviroDIYRequest(modem.stream);
        modem.stream->flush();  // wait for sending to finish

        // Wait for at least the first 12 characters to make it across
        unsigned long timeout = 1500;
        for (unsigned long start = millis(); millis() - start < timeout; )
        {
            if (modem.stream->available() >= 12) break;
        }

        // Process the HTTP response
        int responseCode = 0;
        if (modem.stream->available() >= 12)
        {
            modem.stream->readStringUntil(' ');  // Throw away the "HTTP/1.1"
            responseCode = modem.stream->readStringUntil(' ').toInt();
            modem.dumpBuffer(modem.stream);
        }
        else responseCode=504;

        PRINTOUT(F(" -- Response Code -- \n"));
        PRINTOUT(responseCode, F("\n"));

        modem.stop();

        return responseCode;
    }

    // ===================================================================== //
    // Convience functions to call several of the above functions
    // ===================================================================== //
    // This calls all of the setup functions - must be run AFTER init
    virtual void begin(void) override
    {
        // Print a start-up note to the first serial port
        PRINTOUT(F("Beginning logger "), _loggerID, F("\n"));

        // Start the Real Time Clock
        rtc.begin();
        delay(100);

        // Set up pins for the LED's
        pinMode(_ledPin, OUTPUT);

        // Set up the sensors
        setupSensors();

        // Set up the log file
        setupLogFile();

        // Sync the clock with NIST
        PRINTOUT(F("Current RTC time is: "));
        PRINTOUT(formatDateTime_ISO8601(getNow()), F("\n"));
        // Turn on the modem
        modem.on();
        // Connect to the network
        if (modem.connectNetwork())
        {
            // Synchronize the RTC
            modem.syncDS3231();
            // Disconnect from the network
            modem.disconnectNetwork();
        }
        // Turn off the modem
        modem.off();

        // Setup sleep mode
        if(_sleep){setupSleep();}

        PRINTOUT(F("Logger setup finished!\n"));
        PRINTOUT(F("------------------------------------------\n\n"));
    }

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
            modem.on();

            // Wake up all of the sensors
            // I'm not doing as part of sleep b/c it may take up to a second or
            // two for them all to wake which throws off the checkInterval()
            sensorsWake();
            // Update the values from all attached sensors
            updateAllSensors();
            // Immediately put sensors to sleep to save power
            sensorsSleep();

            // Connect to the network
            if (modem.connectNetwork())
            {
                // Post the data to the WebSDL
                postDataEnviroDIY();

                // Sync the clock every 288 readings (1/day at 5 min intervals)
                if (_numReadings % 288 == 0)
                {
                    modem.syncDS3231();
                }

                // Disconnect from the network
                modem.disconnectNetwork();
            }

            // Turn on the modem off
            modem.off();

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

#endif


private:
    // Tokens and UUID's for EnviroDIY
    const char *_registrationToken;
    const char *_samplingFeature;
    const char **_UUIDs;
};

#endif
