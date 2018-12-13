/*
 *LoggerDreamHost.cpp
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Initial library developement done by Sara Damiano (sdamiano@stroudcenter.org).
 *
 *This file is for the EnviroDIY logging functions - ie, sending get requests to DreamHost
*/

#include "LoggerDreamHost.h"


// ============================================================================
//  Functions for the SWRC Sensors DreamHost data receivers.
// ============================================================================

// Constructor
LoggerDreamHost::LoggerDreamHost(const char *loggerID, uint16_t loggingIntervalMinutes,
                                 int8_t SDCardPin, int8_t mcuWakePin,
                                 VariableArray *inputArray)
  : LoggerEnviroDIY(loggerID, loggingIntervalMinutes, SDCardPin, mcuWakePin, inputArray)
{}
// Destructor
LoggerDreamHost::~LoggerDreamHost(){}


// Constant portions of the requests
const char *LoggerDreamHost::getHeader = "GET ";
const char *LoggerDreamHost::dreamhostHost = "swrcsensors.dreamhosters.com";
const char *LoggerDreamHost::loggerTag = "?LoggerID=";
const char *LoggerDreamHost::timestampTagDH = "&Loggertime=";


// Functions for private SWRC server
void LoggerDreamHost::setDreamHostPortalRX(const char *URL)
{
    _DreamHostPortalRX = URL;
    MS_DBG(F("Dreamhost portal URL set!"));
}


// This prints the URL out to an Arduino stream
void LoggerDreamHost::printSensorDataDreamHost(Stream *stream)
{
    stream->print(_DreamHostPortalRX);
    stream->print(loggerTag);
    stream->print(_loggerID);
    stream->print(timestampTagDH);
    stream->print(String(Logger::markedEpochTime - 946684800));  // Correct time from epoch to y2k

    for (uint8_t i = 0; i < _internalArray->getVariableCount(); i++)
    {
        stream->print('&');
        stream->print(_internalArray->arrayOfVars[i]->getVarCode());
        stream->print('=');
        stream->print(_internalArray->arrayOfVars[i]->getValueString());
    }
}


// This prints a fully structured GET request for DreamHost to the
// specified stream
void LoggerDreamHost::printDreamHostRequest(Stream *stream)
{
    // Start the request
    stream->print(getHeader);

    // Stream the full URL with parameters
    printSensorDataDreamHost(stream);

    // Send the rest of the HTTP header
    stream->print(HTTPtag);
    stream->print(hostHeader);
    stream->print(dreamhostHost);
    stream->print(F("\r\n\r\n"));
}


// Post the data to dream host.
int16_t LoggerDreamHost::postDataDreamHost(void)
{
    // do not continue if no modem!
    if (_logModem == NULL)
    {
        PRINTOUT(F("No modem attached, data cannot be sent out!"));
        return 504;
    }

    // Create a buffer for the portions of the request and response
    char tempBuffer[37] = "";
    uint16_t did_respond = 0;

    // Open a TCP/IP connection to DreamHost
    if(_logModem->_tinyClient->connect(dreamhostHost, 80))
    {
        // copy the initial post header into the tx buffer
        strcpy(txBuffer, getHeader);

        // add in the dreamhost receiver URL
        strcat(txBuffer, _DreamHostPortalRX);

        // start the URL parameters
        if (bufferFree() < 16) printTxBuffer(_logModem->_tinyClient);
        strcat(txBuffer, loggerTag);
        strcat(txBuffer, _loggerID);

        if (bufferFree() < 22) printTxBuffer(_logModem->_tinyClient);
        strcat(txBuffer, timestampTagDH);
        itoa(Logger::markedEpochTime - 946684800, tempBuffer, 10);
        strcat(txBuffer, tempBuffer);

        for (uint8_t i = 0; i < _internalArray->getVariableCount(); i++)
        {
            // Once the buffer fills, send it out
            if (bufferFree() < 47) printTxBuffer(_logModem->_tinyClient);

            txBuffer[strlen(txBuffer)] = '&';
            _internalArray->arrayOfVars[i]->getVarCode().toCharArray(tempBuffer, 37);
            strcat(txBuffer, tempBuffer);
            txBuffer[strlen(txBuffer)] = '=';
            _internalArray->arrayOfVars[i]->getValueString().toCharArray(tempBuffer, 37);
            strcat(txBuffer, tempBuffer);
        }

        // add the rest of the HTTP GET headers to the outgoing buffer
        if (bufferFree() < 52) printTxBuffer(_logModem->_tinyClient);
        strcat(txBuffer, HTTPtag);
        strcat(txBuffer, hostHeader);
        strcat(txBuffer, enviroDIYHost);
        txBuffer[strlen(txBuffer)] = '\r';
        txBuffer[strlen(txBuffer)] = '\n';
        txBuffer[strlen(txBuffer)] = '\r';
        txBuffer[strlen(txBuffer)] = '\n';

        // Send out the finished request (or the last unsent section of it)
        printTxBuffer(_logModem->_tinyClient);

        uint32_t start_timer = millis();
        while ((millis() - start_timer) < 10000L && _logModem->_tinyClient->available() < 12)
        {delay(10);}

        // Read only the first 12 characters of the response
        // We're only reading as far as the http code, anything beyond that
        // we don't care about.
        did_respond = _logModem->_tinyClient->readBytes(tempBuffer, 12);

        // Close the TCP/IP connection as soon as the first 12 characters are read
        // We don't need anything else and stoping here should save data use.
        _logModem->_tinyClient->stop();
    }
    else PRINTOUT(F("\n -- Unable to Establish Connection to DreamHost -- "));

    // Process the HTTP response
    int16_t responseCode = 0;
    if (did_respond > 0)
    {
        char responseCode_char[4];
        for (uint8_t i = 0; i < 3; i++)
        {
            responseCode_char[i] = tempBuffer[i+9];
        }
        responseCode = atoi(responseCode_char);
    }
    else responseCode=504;

    PRINTOUT(F(" -- Response Code -- "));
    PRINTOUT(responseCode);

    return responseCode;
}


// This prevents the logging function from dual-posting to EnviroDIY
void LoggerDreamHost::disableDualPost(void)
{
    _dualPost = false;
}


// ===================================================================== //
// Convience functions to call several of the above functions
// ===================================================================== //

// This is a one-and-done to log data
void LoggerDreamHost::logDataAndSend(void)
{
    // If the number of intervals is negative, then the sensors and file on
    // the SD card haven't been setup and we want to set them up.
    // NOTE:  Unless it completed in less than one second, the sensor set-up
    // will take the place of logging for this interval!
    if (!_areSensorsSetup)
    {
        // Set up the sensors
        PRINTOUT(F("Sensors and data file had not been set up!  Setting them up now."));
        _internalArray->setupSensors();

       // Create the log file, adding the default header to it
       if (createLogFile(true)) PRINTOUT(F("Data will be saved as "), _fileName);
       else PRINTOUT(F("Unable to create a file to save data to!"));

       // Mark sensors as having been setup
       _areSensorsSetup = 1;
    }

    // Assuming we were woken up by the clock, check if the current time is an
    // even interval of the logging interval
    if (checkInterval())
    {
        // Flag to notify that we're in already awake and logging a point
        Logger::isLoggingNow = true;

        // Print a line to show new reading
        PRINTOUT(F("------------------------------------------"));
        // Turn on the LED to show we're taking a reading
        if (_ledPin >= 0) digitalWrite(_ledPin, HIGH);

        // Turn on the modem to let it start searching for the network
        if (_logModem != NULL) _logModem->modemPowerUp();

        // Do a complete sensor update
        MS_DBG(F("    Running a complete sensor update..."));
        _internalArray->completeUpdate();

        // Create a csv data record and save it to the log file
        logToSD();

        if (_logModem != NULL)
        {
            // Connect to the network
            MS_DBG(F("  Connecting to the Internet..."));
            if (_logModem->connectInternet())
            {
                if(_dualPost)
                {
                    // Post the data to the WebSDL
                    postDataEnviroDIY();
                }

                // Post the data to DreamHost
                postDataDreamHost();

                // Sync the clock at midnight
                if (markedEpochTime != 0 && markedEpochTime % 86400 == 0)
                {
                    MS_DBG(F("  Running a daily clock sync..."));
                    syncRTClock(_logModem->getNISTTime());
                }

                // Disconnect from the network
                MS_DBG(F("  Disconnecting from the Internet..."));
                _logModem->disconnectInternet();
            }
            // Turn the modem off
            _logModem->modemSleepPowerDown();
        }

        // Turn off the LED
        if (_ledPin >= 0) digitalWrite(_ledPin, LOW);
        // Print a line to show reading ended
        PRINTOUT(F("------------------------------------------\n"));

        // Unset flag
        Logger::isLoggingNow = false;
    }

    // Check if it was instead the testing interrupt that woke us up
    if (Logger::startTesting) testingMode();

    // Sleep
    if(_mcuWakePin >= 0){systemSleep();}
}
