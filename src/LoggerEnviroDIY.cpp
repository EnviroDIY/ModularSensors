/*
 *LoggerEnviroDIY.cpp
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Initial library developement done by Sara Damiano (sdamiano@stroudcenter.org).
 *
 *This file is for the EnviroDIY logging functions - ie, sending JSON data to
 * http://data.enviroDIY.org
*/

#include "LoggerEnviroDIY.h"


// ============================================================================
//  Functions for the EnviroDIY data portal receivers.
// ============================================================================

// Constructor
LoggerEnviroDIY::LoggerEnviroDIY()
{
    _modemAttached = false;
}


// Set up communications
// Adds a loggerModem objct to the logger
// loggerModem = TinyGSM modem + TinyGSM client + Modem On Off
void LoggerEnviroDIY::attachModem(loggerModem *modem)
{
    _logModem = modem;
    _modemAttached = true;
    MS_DBG(F("Modem attached!\n"));
}


void LoggerEnviroDIY::setToken(const char *registrationToken)
{
    _registrationToken = registrationToken;
    MS_DBG(F("Registration token set!\n"));
}


void LoggerEnviroDIY::setSamplingFeatureUUID(const char *samplingFeature)
{
    _samplingFeature = samplingFeature;
    MS_DBG(F("Sampling feature UUID set!\n"));
}


// This adds extra data to the datafile header
String LoggerEnviroDIY::generateFileHeader(void)
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
String LoggerEnviroDIY::generateSensorDataJSON(void)
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

    jsonString += F("}");
    return jsonString;
}


// This generates a fully structured POST request for EnviroDIY
String LoggerEnviroDIY::generateEnviroDIYPostRequest(String enviroDIYjson)
{
    String POSTstring = String(F("POST /api/data-stream/ HTTP/1.1"));
    POSTstring += String(F("\r\nHost: data.envirodiy.org"));
    POSTstring += String(F("\r\nTOKEN: ")) + String(_registrationToken);
    // POSTstring += String(F("\r\nCache-Control: no-cache"));
    // POSTstring += String(F("\r\nConnection: close"));
    POSTstring += String(F("\r\nContent-Length: ")) + String(enviroDIYjson.length());
    POSTstring += String(F("\r\nContent-Type: application/json\r\n\r\n"));
    POSTstring += String(enviroDIYjson);
    return POSTstring;
}
String LoggerEnviroDIY::generateEnviroDIYPostRequest(void)
{
    return generateEnviroDIYPostRequest(generateSensorDataJSON());
}

// This prints a fully structured post request for EnviroDIY to the
// specified stream using the specified json.
void LoggerEnviroDIY::streamEnviroDIYRequest(Stream *stream, String enviroDIYjson)
{
    stream->print(String(F("POST /api/data-stream/ HTTP/1.1")));
    stream->print(String(F("\r\nHost: data.envirodiy.org")));
    stream->print(String(F("\r\nTOKEN: ")) + String(_registrationToken));
    // stream->print(String(F("\r\nCache-Control: no-cache")));
    // stream->print(String(F("\r\nConnection: close")));
    stream->print(String(F("\r\nContent-Length: ")) + String(enviroDIYjson.length()));
    stream->print(String(F("\r\nContent-Type: application/json\r\n\r\n")));
    stream->print(String(enviroDIYjson));
}
void LoggerEnviroDIY::streamEnviroDIYRequest(Stream *stream)
{
    // first we need to calculate how long the json string is going to be
    int jsonLength = 22;  // {"sampling_feature": "
    jsonLength += 36;  // sampling feature GUID
    jsonLength += 17;  // ", "timestamp": "
    jsonLength += 25;  // markedISO8601Time
    jsonLength += 3;  //  ",_
    for (int i = 0; i < Logger::_variableCount; i++)
    {
        jsonLength += 1;  //  "
        jsonLength += 36;  // variable GUID
        jsonLength += 3;  //  ":_
        jsonLength += Logger::_variableList[i]->getValueString().length();
        if (i + 1 != Logger::_variableCount)
        {
            jsonLength += 2;  // ,_
        }
    }
    jsonLength += 1;  // }

    Serial.println(jsonLength);
    Serial.println(generateSensorDataJSON().length());

    stream->print(String(F("POST /api/data-stream/ HTTP/1.1")));
    stream->print(String(F("\r\nHost: data.envirodiy.org")));
    stream->print(String(F("\r\nTOKEN: ")) + String(_registrationToken));
    // stream->print(String(F("\r\nCache-Control: no-cache")));
    // stream->print(String(F("\r\nConnection: close")));
    stream->print(String(F("\r\nContent-Length: ")) + String(jsonLength));
    stream->print(String(F("\r\nContent-Type: application/json\r\n\r\n")));
    stream->print(String(F("{\"sampling_feature\": \"")));
    stream->print(String(_samplingFeature)); + F("");
    stream->print(String(F("\", \"timestamp\": \"")));
    stream->print(String(Logger::markedISO8601Time) + F("\", "));

    for (int i = 0; i < Logger::_variableCount; i++)
    {
        stream->print(String(F("\"")) + Logger::_variableList[i]->getVarUUID() + String(F("\": ")) + Logger::_variableList[i]->getValueString());
        if (i + 1 != Logger::_variableCount)
        {
            stream->print(F(", "));
        }
    }

    stream->print(F("}"));
}


// This utilizes an attached modem to make a TCP connection to the
// EnviroDIY/ODM2DataSharingPortal and then streams out a post request
// over that connection.
// The return is the http status code of the response.
int LoggerEnviroDIY::postDataEnviroDIY(String enviroDIYjson)
{
    // do not continue if no modem!
    if (!_modemAttached)
    {
        PRINTOUT(F("No modem attached, data cannot be sent out!\n"));
        return 504;
    }

    // Create a buffer for the response
    char response_buffer[12] = "";
    int did_respond = 0;

    // Open a TCP/IP connection to the Enviro DIY Data Portal (WebSDL)
    if(_logModem->openTCP("data.envirodiy.org", 80))
    {
        // Send the request to the serial for debugging
        #if defined(STANDARD_SERIAL_OUTPUT)
            PRINTOUT(F("\n \\/---- Post Request to EnviroDIY ----\\/ \n"));
            if (enviroDIYjson.length() > 1) streamEnviroDIYRequest(&STANDARD_SERIAL_OUTPUT, enviroDIYjson);
            else streamEnviroDIYRequest(&STANDARD_SERIAL_OUTPUT);
            PRINTOUT(F("\r\n\r\n"));
            STANDARD_SERIAL_OUTPUT.flush();
        #endif

        // Send the request to the modem stream
        if (enviroDIYjson.length() > 1) streamEnviroDIYRequest(_logModem->_client, enviroDIYjson);
        else streamEnviroDIYRequest(_logModem->_client);
        _logModem->_client->flush();  // wait for sending to finish

        uint32_t start_timer = millis();
        while ((millis() - start_timer) < 10000L && _logModem->_client->available() < 12)
        {delay(10);}

        // Read only the first 12 characters of the response
        // We're only reading as far as the http code, anything beyond that
        // we don't care about so we're not reading to save on total
        // data used for transmission.
        did_respond = _logModem->_client->readBytes(response_buffer, 12);

        // Close the TCP/IP connection as soon as the first 12 characters are read
        // We don't need anything else and stoping here should save data use.
        _logModem->closeTCP();
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

// This defines what to do in the testing mode
void LoggerEnviroDIY::testingMode()
{
    // Flag to notify that we're in testing mode
    Logger::_isTestingNow = true;
    // Unset the _startTesting flag
    Logger::_startTesting = false;

    PRINTOUT(F("------------------------------------------\n"));
    PRINTOUT(F("Entering sensor testing mode\n"));
    delay(100);  // This seems to prevent crashes, no clue why ....

    if (_modemAttached)
    {
        // Turn on the modem to let it start searching for the network
        // Turn on the modem
        _logModem->wake();
        // Connect to the network to make sure we have signal (only try for 10sec)
        _logModem->connectInternet(10000L);
    }

    // Power up all of the sensors
    sensorsPowerUp();

    // Wake up all of the sensors
    sensorsWake();

    // Update the sensors and print out data 25 times
    for (uint8_t i = 0; i < 25; i++)
    {
        PRINTOUT(F("------------------------------------------\n"));
        // Update the values from all attached sensors
        updateAllSensors();
        // Print out the current logger time
        PRINTOUT(F("Current logger time is "));
        PRINTOUT(formatDateTime_ISO8601(getNowEpoch()), F("\n"));
        PRINTOUT(F("    -----------------------\n"));
        // Print out the sensor data
        #if defined(STANDARD_SERIAL_OUTPUT)
            printSensorData(&STANDARD_SERIAL_OUTPUT);
        #endif
        PRINTOUT(F("    -----------------------\n"));

        if (_modemAttached)
        {
            // Specially highlight the modem signal quality in the debug mode
            _logModem->checkForUpdate();
            PRINTOUT(F("Current modem signal is "));
            PRINTOUT(_logModem->getSignalPercent());
            PRINTOUT(F("%\n"));
        }

        delay(5000);
    }

    // Put sensors to sleep
    sensorsSleep();
    sensorsPowerDown();

    if (_modemAttached)
    {
        // Disconnect from the network
        _logModem->disconnectInternet();
        // Turn off the modem
        _logModem->off();
    }

    // Unset testing mode flag
    Logger::_isTestingNow = false;
}


// This calls all of the setup functions - must be run AFTER init
void LoggerEnviroDIY::begin(void)
{
    // Set up pins for the LED's
    if (_ledPin > 0) pinMode(_ledPin, OUTPUT);

    #if defined ARDUINO_ARCH_SAMD
        zero_sleep_rtc.begin();
    #else
        rtc.begin();
        delay(100);
    #endif

    // Print out the current time
    PRINTOUT(F("Current RTC time is: "));
    PRINTOUT(formatDateTime_ISO8601(getNowEpoch()), F("\n"));

    if (_modemAttached)
    {
        // Synchronize the RTC with NIST
        PRINTOUT(F("Attempting to synchronize RTC with NIST\n"));
        // Turn on the modem
        _logModem->powerUp();
        _logModem->wake();
        // Connect to the network
        if (_logModem->connectInternet())
        {
            syncRTClock(_logModem->getNISTTime());
            // Disconnect from the network
            _logModem->disconnectInternet();
        }
        // Turn off the modem
        _logModem->off();
    }

    // Set up the sensors
    setupSensors();

    // Set the filename for the logger to save to, if it hasn't been done
    if(!_isFileNameSet){setFileName();}
    else if(_autoFileName){setFileName();}
    else setFileName(_fileName);  // This just for a nice print-out

    // Set up the log file
    setupLogFile();

    // Setup sleep mode
    if(_sleep){setupSleep();}

    PRINTOUT(F("Logger setup finished!\n"));
    PRINTOUT(F("------------------------------------------\n\n"));
}


// This is a one-and-done to log data
void LoggerEnviroDIY::log(void)
{
    // Assuming we were woken up by the clock, check if the current time is an
    // even interval of the logging interval
    if (checkInterval())
    {
        // Flag to notify that we're in already awake and logging a point
        Logger::_isLoggingNow = true;

        // Print a line to show new reading
        PRINTOUT(F("------------------------------------------\n"));
        // Turn on the LED to show we're taking a reading
        digitalWrite(_ledPin, HIGH);

        if (_modemAttached)
        {
            // Turn on the modem to let it start searching for the network
            _logModem->powerUp();
            _logModem->wake();
        }

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

        if (_modemAttached)
        {
            // Connect to the network
            if (_logModem->connectInternet())
            {
                // Post the data to the WebSDL
                postDataEnviroDIY();

                // Sync the clock every 288 readings (1/day at 5 min intervals)
                if (_numTimepointsLogged % 288 == 0)
                {
                    syncRTClock(_logModem->getNISTTime());
                }

                // Disconnect from the network
                _logModem->disconnectInternet();
            }
            // Turn the modem off
            _logModem->off();
        }

        // Create a csv data record and save it to the log file
        logToSD(generateSensorDataCSV());

        // Turn off the LED
        digitalWrite(_ledPin, LOW);
        // Print a line to show reading ended
        PRINTOUT(F("------------------------------------------\n\n"));

        // Unset flag
        Logger::_isLoggingNow = false;
    }

    // Check if it was instead the testing interrupt that woke us up
    if (Logger::_startTesting) testingMode();

    // Sleep
    if(_sleep){systemSleep();}
}
