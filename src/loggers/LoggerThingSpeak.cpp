/*
 *LoggerThingSpeak.cpp
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Initial library developement done by Sara Damiano (sdamiano@stroudcenter.org).
 *
 *This file is for the EnviroDIY logging functions - ie, sending JSON data to
 * http://data.enviroDIY.org
*/

#include "loggers/LoggerThingSpeak.h"


// ============================================================================
//  Functions for the EnviroDIY data portal receivers.
// ============================================================================

// Constructor
LoggerThingSpeak::LoggerThingSpeak(const char *loggerID, uint16_t loggingIntervalMinutes,
                                 int8_t SDCardPin, int8_t mcuWakePin,
                                 VariableArray *inputArray)
  : Logger(loggerID, loggingIntervalMinutes, SDCardPin, mcuWakePin, inputArray)
{
    _logModem = NULL;
}
// Destructor
LoggerThingSpeak::~LoggerThingSpeak(){}


// Constant values for MQTT publish
// I want to refer to these more than once while ensuring there is only one copy in memory
const char *LoggerThingSpeak::mqttServer = "mqtt.thingspeak.com";
const char *LoggerThingSpeak::mqttClient = THING_SPEAK_CLIENT_NAME;
const char *LoggerThingSpeak::mqttUser = THING_SPEAK_USER_NAME;

char LoggerThingSpeak::msgBuffer[MS_TS_SEND_BUFFER_SIZE] = {'\0'};


// Set up communications
// Adds a loggerModem objct to the logger
// loggerModem = TinyGSM modem + TinyGSM client + Modem On Off
void LoggerThingSpeak::attachModem(loggerModem& modem)
{
    _logModem = &modem;
    _mqttClient.setClient(*_logModem->_tinyClient);
    _mqttClient.setServer(mqttServer, 1883);
    PRINTOUT(F("A "), _logModem->getSensorName(),
             F(" has been tied to this logger!"));
}


// Takes advantage of the modem to synchronize the clock
bool LoggerThingSpeak::syncRTC()
{
    bool success = false;
    if (_logModem != NULL)
    {
        // Synchronize the RTC with NIST
        PRINTOUT(F("Attempting to synchronize RTC with NIST"));
        PRINTOUT(F("This may take up to two minutes!"));
        // Connect to the network
        if (_logModem->connectInternet(120000L))
        {
            success = syncRTClock(_logModem->getNISTTime());
            // Disconnect from the network
            _logModem->disconnectInternet();
        }
        // Turn off the modem
        _logModem->modemSleepPowerDown();
    }
    return success;
}


void LoggerThingSpeak::setMQTTKey(const char *thingSpeakMQTTKey)
{
    _thingSpeakMQTTKey = thingSpeakMQTTKey;
    MS_DBG(F("MQTT Key set!"));
}


void LoggerThingSpeak::setChannelID(const char *thingSpeakChannelID)
{
    _thingSpeakChannelID = thingSpeakChannelID;
    MS_DBG(F("Channel ID set!"));
}


void LoggerThingSpeak::setChannelKey(const char *thingSpeakChannelKey)
{
    _thingSpeakChannelKey = thingSpeakChannelKey;
    MS_DBG(F("Channel Key set!"));
}

// Sets all 3 ThingSpeak parameters
void LoggerThingSpeak::setThingSpeakParams(const char *MQTTKey,
                                           const char *channelID,
                                           const char *channelKey)
{
    setMQTTKey(MQTTKey);
    setChannelID(channelID);
    setChannelKey(channelKey);
}


// Empties the outgoing buffer
void LoggerThingSpeak::emptyMsgBuffer(void)
{
    MS_DBG(F("Dumping the TX Buffer"));
    for (int i = 0; i < MS_TS_SEND_BUFFER_SIZE; i++)
    {
        msgBuffer[i] = '\0';
    }
}


// This sends the data to ThingSpeak
bool LoggerThingSpeak::mqttThingSpeak(void)
{
    // do not continue if no modem!
    if (_logModem == NULL)
    {
        PRINTOUT(F("No modem attached, data cannot be sent out!"));
        return 504;
    }

    // Make sure we don't have too many fields
    // A channel can have a max of 8 fields
    if (_internalArray->getVariableCount() > 8)
    {
        MS_DBG(F("No more than 8 fields of data can be sent to a single ThingSpeak channel!"));
        MS_DBG(F("Only the first 8 channels worth of data will be sent."));
    }
    uint8_t numChannels = min(_internalArray->getVariableCount(), 8);
    MS_DBG(numChannels, F(" channels will be sent to ThingSpeak"));

    // Create a buffer for the portions of the request and response
    char tempBuffer[26] = "";

    char topicBuffer[42] = "channels/";
    strcat(topicBuffer, _thingSpeakChannelID);
    strcat(topicBuffer, "/publish/");
    strcat(topicBuffer, _thingSpeakChannelKey);
    MS_DBG(F("Topic: "), String(topicBuffer));

    emptyMsgBuffer();

    formatDateTime_ISO8601(markedEpochTime).toCharArray(tempBuffer, 26);
    strcat(msgBuffer, "created_at=");
    strcat(msgBuffer, tempBuffer);
    msgBuffer[strlen(msgBuffer)] = '&';

    for (uint8_t i = 0; i < numChannels; i++)
    {
        strcat(msgBuffer, "field");
        itoa(i+1, tempBuffer, 12);
        strcat(msgBuffer, tempBuffer);
        msgBuffer[strlen(msgBuffer)] = '=';
        _internalArray->arrayOfVars[i]->getValueString().toCharArray(tempBuffer, 26);
        strcat(msgBuffer, tempBuffer);
        if (i + 1 != numChannels)
        {
            msgBuffer[strlen(msgBuffer)] = '&';
        }
    }
    MS_DBG(F("Message: "), String(msgBuffer));

    // Make the MQTT connection
    // Note:  the client id and the user name do not mean anything for ThingSpeak
    if(_mqttClient.connect(mqttClient, mqttUser, _thingSpeakMQTTKey))
    {
        if (_mqttClient.publish(topicBuffer, msgBuffer))
        {
            MS_DBG(F("Topic published!  Current state: "), _mqttClient.state());
            return true;
        }
        else
        {
            MS_DBG(F("MQTT publish failed with state: "), _mqttClient.state());
            return false;
        }
    }
    else
    {
        MS_DBG(F("MQTT connection failed with state: "), _mqttClient.state());
        return false;
    }
}


// ===================================================================== //
// Convience functions to call several of the above functions
// ===================================================================== //


// This is a one-and-done to log data
void LoggerThingSpeak::logDataAndSend(void)
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
       if (_autoFileName) generateAutoFileName();
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
                // Post the data to the WebSDL
                mqttThingSpeak();

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
            else {MS_DBG(F("  Could not connect to the internet!"));}
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
