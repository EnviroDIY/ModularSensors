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

// To prevent compiler/linker crashes with Enable interrupt
#define LIBCALL_ENABLEINTERRUPT
// To handle external and pin change interrupts
#include <EnableInterrupt.h>


// ============================================================================
//  Functions for the EnviroDIY data portal receivers.
// ============================================================================

// Constructor
LoggerEnviroDIY::LoggerEnviroDIY(const char *loggerID, uint16_t loggingIntervalMinutes,
                                 int8_t SDCardPin, int8_t mcuWakePin,
                                 VariableArray *inputArray)
  : Logger(loggerID, loggingIntervalMinutes, SDCardPin, mcuWakePin, inputArray)
{
    _logModem = NULL;
}
// Destructor
LoggerEnviroDIY::~LoggerEnviroDIY(){}


// Set up communications
// Adds a loggerModem objct to the logger
// loggerModem = TinyGSM modem + TinyGSM client + Modem On Off
void LoggerEnviroDIY::attachModem(loggerModem& modem)
{
    _logModem = &modem;
    MS_DBG(F("Modem attached!"));
}


void LoggerEnviroDIY::setToken(const char *registrationToken)
{
    _registrationToken = registrationToken;
    MS_DBG(F("Registration token set!"));
}


void LoggerEnviroDIY::setSamplingFeatureUUID(const char *samplingFeature)
{
    _samplingFeature = samplingFeature;
    MS_DBG(F("Sampling feature UUID set!"));
}


// This is a PRE-PROCESSOR MACRO to speed up generating header rows
// Again, THIS IS NOT A FUNCTION, it is a pre-processor macro
#define STREAM_CSV_ROW(firstCol, function) \
    stream->print("\""); \
    stream->print(firstCol); \
    stream->print("\","); \
    for (uint8_t i = 0; i < _internalArray->getVariableCount(); i++) \
    { \
        stream->print("\""); \
        stream->print(function); \
        stream->print("\""); \
        if (i + 1 != _internalArray->getVariableCount()) \
        { \
            stream->print(","); \
        } \
    } \
    stream->println();

// This adds extra data to the datafile header
void LoggerEnviroDIY::printFileHeader(Stream *stream)
{
    // Very first line of the header is the logger ID
    stream->print(F("Data Logger: "));
    stream->println(_loggerID);

    // Next we're going to print the current file name
    stream->print(F("Data Logger File: "));
    stream->println(_fileName);

    // Next we're going to print the sampling feature UUID
    // NOTE:  This is the only line different from in Logger::printFileHeader
    stream->print(F("Sampling Feature UUID: "));
    stream->println(_samplingFeature);

    // Next line will be the parent sensor names
    STREAM_CSV_ROW(F("Sensor Name:"), _internalArray->arrayOfVars[i]->getParentSensorName())
    // Next comes the ODM2 variable name
    STREAM_CSV_ROW(F("Variable Name:"), _internalArray->arrayOfVars[i]->getVarName())
    // Next comes the ODM2 unit name
    STREAM_CSV_ROW(F("Result Unit:"), _internalArray->arrayOfVars[i]->getVarUnit())
    // Next comes the variable UUIDs
    STREAM_CSV_ROW(F("Result UUID:"), _internalArray->arrayOfVars[i]->getVarUUID())

    // We'll finish up the the custom variable codes
    String dtRowHeader = F("Date and Time in UTC");
    if (_timeZone > 0) dtRowHeader += '+' + _timeZone;
    else if (_timeZone < 0) dtRowHeader += _timeZone;
    STREAM_CSV_ROW(dtRowHeader, _internalArray->arrayOfVars[i]->getVarCode());
}


// This prints a properly formatted JSON for EnviroDIY to an Arduino stream
uint16_t LoggerEnviroDIY::printSensorDataJSON(Stream *stream)
{
    uint16_t txChars;
    txChars = stream->print(String(F("{")));
    txChars +=stream->print(String(F("\"sampling_feature\": \"")));
    txChars +=stream->print(String(_samplingFeature));
    txChars +=stream->print(String(F("\", \"timestamp\": \"")));
    txChars +=stream->print(String(formatDateTime_ISO8601(markedEpochTime)) + F("\", "));

    for (uint8_t i = 0; i < _internalArray->getVariableCount(); i++)
    {
        txChars +=stream->print(String(F("\"")) + _internalArray->arrayOfVars[i]->getVarUUID() + String(F("\": ")) + _internalArray->arrayOfVars[i]->getValueString());
        if (i + 1 != _internalArray->getVariableCount())
        {
            txChars +=stream->print(F(", "));
        }
    }

    txChars +=stream->print(F("}"));
    return txChars;
}


// This prints a fully structured post request for EnviroDIY to the
// specified stream.
uint16_t LoggerEnviroDIY::printEnviroDIYRequest(Stream *stream)
{
    uint16_t txChars=0;
    // First we need to calculate how long the json string is going to be
    // This is needed for the "Content-Length" header
    uint16_t jsonLength = 22;  // {"sampling_feature": "
    jsonLength += 36;  // sampling feature UUID
    jsonLength += 17;  // ", "timestamp": "
    jsonLength += 25;  // markedISO8601Time
    jsonLength += 3;  //  ",_

    for (uint8_t i = 0; i < _internalArray->getVariableCount(); i++)
    {
        jsonLength += 1;  //  "
        jsonLength += 36;  // variable UUID
        jsonLength += 3;  //  ":_
        jsonLength += _internalArray->arrayOfVars[i]->getValueString().length();
        if (i + 1 != _internalArray->getVariableCount())
        {
            jsonLength += 2;  // ,_
        }
    }
    jsonLength += 1;  // }

    // Stream the HTTP headers for the post request
    txChars += stream->print(String(F("POST /api/data-stream/ HTTP/1.1")));
    txChars += stream->print(String(F("\r\nHost: data.envirodiy.org")));
    txChars += stream->print(String(F("\r\nTOKEN: ")) + String(_registrationToken));
    // tx_chars += stream->print(String(F("\r\nCache-Control: no-cache")));
    // tx_chars += stream->print(String(F("\r\nConnection: close")));
    txChars += stream->print(String(F("\r\nContent-Length: ")) + String(jsonLength));
    txChars += stream->print(String(F("\r\nContent-Type: application/json\r\n\r\n")));

    // Stream the JSON itself
    txChars +=printSensorDataJSON(stream);
    //tx_chars += txChars;
    return txChars;
}


// This writes the post request to a "queue" file for later
bool LoggerEnviroDIY::queueDataEnviroDIY(void)
{
    String queueFile = "EnviroDIYQueue.txt";
    // First attempt to open the file without creating a new one
    if (!openFile(queueFile, true, false))
    {
        // Next try to create the file, bail if we couldn't create it
        // This will not attempt to generate a new file name or add a header!
        if (!openFile(queueFile, true, false))
        {
            PRINTOUT(F("Unable to write to SD card!"));
            return false;
        }
    }

    // If we could successfully open or create the file, write the request to it
    printEnviroDIYRequest(&logFile);
    // Echo the line to the serial port
    #if defined(STANDARD_SERIAL_OUTPUT)
    PRINTOUT(F("\n \\/---- Queued POST request ----\\/ "));
        printEnviroDIYRequest(&STANDARD_SERIAL_OUTPUT);
        PRINTOUT('\n');
    #endif

    // Set write/modification date time
    setFileTimestamp(logFile, T_WRITE);
    // Set access date time
    setFileTimestamp(logFile, T_ACCESS);
    // Close the file to save it
    logFile.close();
    return true;
}


// This utilizes an attached modem to make a TCP connection to the
// EnviroDIY/ODM2DataSharingPortal and then streams out a post request
// over that connection.
// The return is the http status code of the response.
int16_t LoggerEnviroDIY::postDataEnviroDIY(void)
{
    // do not continue if no modem!
    if (_logModem == NULL)
    {
        PRINTOUT(F("No modem attached, data cannot be sent out!"));
        return 504;
    }

    // Create a buffer for the response
    char response_buffer[12] = "";
    uint16_t did_respond = 0;

    // Open a TCP/IP connection to the Enviro DIY Data Portal (WebSDL)
    if(_logModem->_tinyClient->connect("data.envirodiy.org", 80))
    {
        // Send the request to the serial for debugging
        #if defined(STANDARD_SERIAL_OUTPUT)
            PRINTOUT(F("\n \\/---- Post Request to EnviroDIY ----\\/ "));
            printEnviroDIYRequest(&STANDARD_SERIAL_OUTPUT);
            PRINTOUT('\n');
            STANDARD_SERIAL_OUTPUT.flush();
        #endif

        // Send the request to the modem stream
        tx_chars += printEnviroDIYRequest(_logModem->_tinyClient);
        _logModem->_tinyClient->flush();  // wait for sending to finish

        uint32_t start_timer = millis();
        while ((millis() - start_timer) < 10000L && _logModem->_tinyClient->available() < 12)
        {delay(10);}

        // Read only the first 12 characters of the response
        // We're only reading as far as the http code, anything beyond that
        // we don't care about.
        did_respond = _logModem->_tinyClient->readBytes(response_buffer, 12);

        // Close the TCP/IP connection as soon as the first 12 characters are read
        // We don't need anything else and stoping here should save data use.
        _logModem->_tinyClient->stop();
    }
    else PRINTOUT(F("\n -- Unable to Establish Connection to EnviroDIY Data Portal -- "));

    // Process the HTTP response
    int16_t responseCode = 0;
    if (did_respond > 0)
    {
        char responseCode_char[4];
        for (uint8_t i = 0; i < 3; i++)
        {
            responseCode_char[i] = response_buffer[i+9];
        }
        responseCode = atoi(responseCode_char);
    }
    else responseCode=504;

    PRINTOUT(F(" -- Response Code -- "));
    PRINTOUT(responseCode);

    return responseCode;
}


// ===================================================================== //
// Public functions for a "sensor testing" mode
// ===================================================================== //

// This defines what to do in the testing mode
void LoggerEnviroDIY::testingMode()
{
    // Flag to notify that we're in testing mode
    Logger::isTestingNow = true;
    // Unset the startTesting flag
    Logger::startTesting = false;

    PRINTOUT(F("------------------------------------------"));
    PRINTOUT(F("Entering sensor testing mode"));
    delay(100);  // This seems to prevent crashes, no clue why ....

    if (_logModem != NULL)
    {
        // Turn on the modem to let it start searching for the network
        _logModem->modemPowerUp();
    }

    // Power up all of the sensors
    _internalArray->sensorsPowerUp();

    // Wake up all of the sensors
    _internalArray->sensorsWake();

    // Update the sensors and print out data 25 times
    for (uint8_t i = 0; i < 25; i++)
    {
        PRINTOUT(F("------------------------------------------"));
        // Update the values from all attached sensors
        // NOTE:  NOT using complete update because we want everything left
        // on between iterations in testing mode.
        _internalArray->updateAllSensors();
        // Print out the current logger time
        PRINTOUT(F("Current logger time is "), formatDateTime_ISO8601(getNowEpoch()));
        PRINTOUT(F("    -----------------------"));
        // Print out the sensor data
        #if defined(STANDARD_SERIAL_OUTPUT)
            _internalArray->printSensorData(&STANDARD_SERIAL_OUTPUT);
        #endif
        PRINTOUT(F("    -----------------------"));

        if (_logModem != NULL)
        {
            // Specially highlight the modem signal quality in the debug mode
            _logModem->update();
            PRINTOUT(F("Current modem signal is "), _logModem->getSignalPercent(), "%");
        }

        delay(5000);
    }

    // Put sensors to sleep
    _internalArray->sensorsSleep();
    _internalArray->sensorsPowerDown();

    if (_logModem != NULL)
    {
        // Disconnect from the network
        _logModem->disconnectInternet();
        // Turn off the modem
        _logModem->modemSleepPowerDown();
    }

    // Unset testing mode flag
    Logger::isTestingNow = false;

    // Sleep
    if(_mcuWakePin >= 0){systemSleep();}
}


// ===================================================================== //
// Convience functions to call several of the above functions
// ===================================================================== //

// This calls all of the setup functions - must be run AFTER init
void LoggerEnviroDIY::beginLogger(void)
{
    // Set up pins for the LED and button
    if (_ledPin >= 0) pinMode(_ledPin, OUTPUT);
    if (_buttonPin >= 0) pinMode(_buttonPin, INPUT_PULLUP);

    #if defined ARDUINO_ARCH_SAMD
        zero_sleep_rtc.begin();
    #else
        rtc.begin();
        delay(100);
    #endif

    // Print out the current time
    PRINTOUT(F("Current RTC time is: "), formatDateTime_ISO8601(getNowEpoch()));

    PRINTOUT(F("Setting up logger "), _loggerID, F(" to record at "),
             _loggingIntervalMinutes, F(" minute intervals."));

    PRINTOUT(F("This logger has a variable array with "),
             _internalArray->getVariableCount(), F(" variables, of which "),
             _internalArray->getVariableCount() - _internalArray->getCalculatedVariableCount(),
             F(" come from "),_internalArray->getSensorCount(), F(" sensors and "),
             _internalArray->getCalculatedVariableCount(), F(" are calculated."));

     // Turn on the modem to let it start searching for the network

     // Set up the sensors, this includes the modem
     PRINTOUT(F("Setting up sensors..."));
     _internalArray->setupSensors();

    // Mark sensors as having been setup
    _areSensorsSetup = 1;

   // Create the log file, adding the default header to it
   if (_autoFileName) generateAutoFileName();
   if (createLogFile(true)) PRINTOUT(F("Data will be saved as "), _fileName);
   else PRINTOUT(F("Unable to create a file to save data to!"));

    // Setup sleep mode
    if(_mcuWakePin >= 0){setupSleep();}

    // Set up the interrupt to be able to enter sensor testing mode
    if (_buttonPin >= 0)
    {
        enableInterrupt(_buttonPin, Logger::testingISR, CHANGE);
        PRINTOUT(F("Push button on pin "), _buttonPin,
                 F(" at any time to enter sensor testing mode."));
    }

    // Make sure all sensors are powered down at the end
    // The should be, but just in case
    _internalArray->sensorsPowerDown();

    PRINTOUT(F("Logger setup finished!"));
    PRINTOUT(F("------------------------------------------\n"));

}

void LoggerEnviroDIY::timeSync(void)
{
     // Turn on the modem to let it start searching for the network
     if (_logModem != NULL) _logModem->modemPowerUp();

    if (_logModem != NULL)
    {
        // Print out the modem info
        PRINTOUT(F("This logger is tied to a "), _logModem->getSensorName(),
                 F(" for internet connectivity."));

        // Synchronize the RTC with NIST
        PRINTOUT(F("Attempting to synchronize RTC with NIST"));
        PRINTOUT(F("This may take up to two minutes!"));
        // Connect to the network
        if (_logModem->connectInternet(120000L))
        {
            syncRTClock(_logModem->getNISTTime());
            // Disconnect from the network
            _logModem->disconnectInternet();
        }
        // Turn off the modem
        _logModem->modemSleepPowerDown();
    }

}

// ===================================================================== //
// Parse an ini file for customization
// ===================================================================== //

#define INI_USE_STACK 1
/* Maximum line length for any line in INI file (stack or heap). Note that
   this must be 3 more than the longest line (due to '\r', '\n', and '\0'). */
//#define INI_MAX_LINE 200
#define INI_MAX_LINE 100
#define MAX_SECTION 50
#define MAX_NAME 50
/* Nonzero to allow multi-line value parsing, in the style of Python's
   configparser. If allowed, ini_parse() will call the handler with the same
   name for each subsequent line parsed. */
#ifndef INI_ALLOW_MULTILINE
#define INI_ALLOW_MULTILINE 1
#endif

/* Nonzero to allow a UTF-8 BOM sequence (0xEF 0xBB 0xBF) at the start of
   the file. See https://github.com/benhoyt/inih/issues/21 */
#ifndef INI_ALLOW_BOM
#define INI_ALLOW_BOM 1
#endif

/* Chars that begin a start-of-line comment. Per Python configparser, allow
   both ; and # comments at the start of a line by default. */
#ifndef INI_START_COMMENT_PREFIXES
#define INI_START_COMMENT_PREFIXES ";#"
#endif

/* Nonzero to allow inline comments (with valid inline comment characters
   specified by INI_INLINE_COMMENT_PREFIXES). Set to 0 to turn off and match
   Python 3.2+ configparser behaviour. */
#ifndef INI_ALLOW_INLINE_COMMENTS
#define INI_ALLOW_INLINE_COMMENTS 1
#endif
#ifndef INI_INLINE_COMMENT_PREFIXES
#define INI_INLINE_COMMENT_PREFIXES ";"
#endif

/* Strip whitespace chars off end of given string, in place. Return s. */
static char* rstrip(char* s)
{
    char* p = s + strlen(s);
    while (p > s && isspace((unsigned char)(*--p)))
        *p = '\0';
    return s;
}

/* Return pointer to first non-whitespace char in given string. */
static char* lskip(const char* s)
{
    while (*s && isspace((unsigned char)(*s)))
        s++;
    return (char*)s;
}

/* Return pointer to first char (of chars) or inline comment in given string,
   or pointer to null at end of string if neither found. Inline comment must
   be prefixed by a whitespace character to register as a comment. */
static char* find_chars_or_comment(const char* s, const char* chars)
{
#if INI_ALLOW_INLINE_COMMENTS
    int was_space = 0;
    while (*s && (!chars || !strchr(chars, *s)) &&
           !(was_space && strchr(INI_INLINE_COMMENT_PREFIXES, *s))) {
        was_space = isspace((unsigned char)(*s));
        s++;
    }
#else
    while (*s && (!chars || !strchr(chars, *s))) {
        s++;
    }
#endif
    return (char*)s;
}

/* Version of strncpy that ensures dest (size bytes) is null-terminated. */
static char* strncpy0(char* dest, const char* src, size_t size)
{
    strncpy(dest, src, size - 1);
    dest[size - 1] = '\0';
    return dest;
}

/* Parse given INI-style file. 
   May have [section]s, 
   name=value pairs (whitespace stripped), and 
   comments starting with ';' (semicolon). 
   Section  is "" if name=value pair parsed before any section heading. 
   name:value     pairs are also supported as a concession to Python's configparser.

   For each name=value pair parsed, call handler function with given user
   pointer as well as section, name, and value (data only valid for duration
   of handler call). 
   Handler should return nonzero on success, zero on error.

   Returns 0 on success, 
     line number of last error on parse error (doesn't stop on first error), 

   https://en.wikipedia.org/wiki/INI_file
   https://github.com/benhoyt/inih
*/
int8_t LoggerEnviroDIY::inihParseFile(ini_handler handler_fn)
{
    /* Uses a fair bit of stack (use heap instead if you need to) */
#if INI_USE_STACK
    char line[INI_MAX_LINE];
    int max_line = INI_MAX_LINE;
#else
    char* line;
    int max_line = INI_INITIAL_ALLOC;
#endif
#if INI_ALLOW_REALLOC && !INI_USE_STACK
    char* new_line;
    int offset;
#endif
    char section[MAX_SECTION] = "";
    char prev_name[MAX_NAME] = "";

    char* start;
    char* end;
    char* name;
    char* value;
    int lineno = 0;
    int error = 0;

#if !INI_USE_STACK
    line = (char*)malloc(INI_INITIAL_ALLOC);
    if (!line) {
        return -2;
    }
#endif

    /* Scan through stream line by line */
#define reader_fn(line1,max_line1) logFile.fgets(line1,max_line1)
    while (reader_fn(line, max_line) != 0) 
    {
#if INI_ALLOW_REALLOC && !INI_USE_STACK
        offset = strlen(line);
        while (offset == max_line - 1 && line[offset - 1] != '\n') {
            max_line *= 2;
            if (max_line > INI_MAX_LINE)
                max_line = INI_MAX_LINE;
            new_line = realloc(line, max_line);
            if (!new_line) {
                free(line);
                return -2;
            }
            line = new_line;
            if (reader_fn(line + offset, max_line - offset) == NULL)
                break;
            if (max_line >= INI_MAX_LINE)
                break;
            offset += strlen(line + offset);
        }
#endif

        lineno++;

        start = line;
#if INI_ALLOW_BOM
        if (lineno == 1 && (unsigned char)start[0] == 0xEF &&
                           (unsigned char)start[1] == 0xBB &&
                           (unsigned char)start[2] == 0xBF) {
            start += 3;
        }
#endif
        start = lskip(rstrip(start));

        if (strchr(INI_START_COMMENT_PREFIXES, *start)) {
            /* Start-of-line comment */
        }
#if INI_ALLOW_MULTILINE
        else if (*prev_name && *start && start > line) {
            /* Non-blank line with leading whitespace, treat as continuation
               of previous name's value (as per Python configparser). */
            if (!handler_fn( section, prev_name, start) && !error)
                error = lineno;
        }
#endif
        else if (*start == '[') {
            /* A "[section]" line */
            end = find_chars_or_comment(start + 1, "]");
            if (*end == ']') {
                *end = '\0';
                strncpy0(section, start + 1, sizeof(section));
                *prev_name = '\0';
            }
            else if (!error) {
                /* No ']' found on section line */
                error = lineno;
            }
        }
        else if (*start) {
            /* Not a comment, must be a name[=:]value pair */
            end = find_chars_or_comment(start, "=:");
            if (*end == '=' || *end == ':') {
                *end = '\0';
                name = rstrip(start);
                value = end + 1;
#if INI_ALLOW_INLINE_COMMENTS
                end = find_chars_or_comment(value, NULL);
                if (*end)
                    *end = '\0';
#endif
                value = lskip(value);
                rstrip(value);

                /* Valid name[=:]value pair found, call handler */
                strncpy0(prev_name, name, sizeof(prev_name));
                if (!handler_fn(section, name, value) && !error)
                    error = lineno;
            }
            else if (!error) {
                /* No '=' or ':' found on name[=:]value line */
                error = lineno;
            }
        }

#if INI_STOP_ON_FIRST_ERROR
        if (error)
            break;
#endif
    }

#if !INI_USE_STACK
    free(line);
#endif

    return error;
}

bool LoggerEnviroDIY::parseIniSd(const char *ini_fn,ini_handler unhandledFn)
{
    uint8_t ini_err;
    // Initialise the SD card
    // skip everything else if there's no SD card, otherwise it might hang
    if (!initializeSDCard()) return false;

    if (logFile.open(ini_fn))
    {
        ini_err = inihParseFile(unhandledFn); //handle return errors
        logFile.close();
        PRINTOUT(F("Parse ini; "),ini_fn);
        if (ini_err) {
            PRINTOUT(F("Error on line :"),ini_err);
        } else {
            PRINTOUT(F("Completed."));
        }
    } else
    {
        PRINTOUT(F("Parse ini; No file "),ini_fn);
        return false;
    }
    return true;
}
void LoggerEnviroDIY::logDataAndSend(void)
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
                postDataEnviroDIY();

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
