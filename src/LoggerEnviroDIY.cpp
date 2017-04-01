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


// Communication set up
void LoggerEnviroDIY::setToken(const char *registrationToken)
{
    _registrationToken = registrationToken;
}

void LoggerEnviroDIY::setSamplingFeature(const char *samplingFeature)
{
    _samplingFeature = samplingFeature;
}

void LoggerEnviroDIY::setUUIDs(const char *UUIDs[])
{
    _UUIDs = UUIDs;
}

void LoggerEnviroDIY::setupModem(modemType modType,
                               Stream *modemStream,
                               int vcc33Pin,
                               int status_CTS_pin,
                               int onoff_DTR_pin,
                               const char *APN)
{
    _modemType = modType;
    _modemStream = modemStream;
    _APN = APN;

    Serial.println(F("Setting up modem."));  // For debugging

    // Initialize the modem
    switch(modType)
    {
        case GPRSBee4:
        case Fona:
        {
            Serial.println(F("Modem uses pulsedOnOff."));  // For debugging
            static pulsedOnOff pulsed;
            _modemOnOff = &pulsed;
            pulsed.init(vcc33Pin, status_CTS_pin, onoff_DTR_pin);
            break;
        }
        case GPRSBee6:
        {
            Serial.println(F("Modem uses heldOnOff."));  // For debugging
            static heldOnOff held;
            _modemOnOff = &held;
            held.init(vcc33Pin, status_CTS_pin, onoff_DTR_pin);
            break;
        }
        case WIFIBee:
        {
            Serial.println(F("Modem is always on."));  // For debugging
            break;
        }
    }
    switch(_modemType)
    {
        case GPRSBee6:
        case GPRSBee4:
        case Fona:
        {
            Serial.println(F("Initializing GSM modem instance"));  // For debugging
            TinyGsm _modem(*modemStream);
            TinyGsmClient _client(_modem);
            _modem.init();
            break;
        }
        case WIFIBee:
        {
            Serial.println(F("Modem does not need to be initialized"));  // For debugging
            break;
        }
    }
};


// Adds the extra UUIDs to the header of the log file
void LoggerEnviroDIY::setupLogFile(void)
{
    // Set up the log file and add the major headers
    LoggerBase::setupLogFile();

    // Convert the string filename to a character file name for SdFat
    int fileNameLength = LoggerBase::_fileName.length() + 1;
    char charFileName[fileNameLength];
    LoggerBase::_fileName.toCharArray(charFileName, fileNameLength);

    // Re-open the file in write mode
    logFile.open(charFileName, O_WRITE | O_AT_END);

    // Add additional UUID information
    String dataHeader = F("\"Sampling Feature: ");
    dataHeader += _samplingFeature;
    dataHeader += F("\"");
    for (uint8_t i = 0; i < _sensorCount; i++)
    {
        dataHeader += F("\"");
        dataHeader += String(_UUIDs[i]);
        dataHeader += F("\"");
        if (i + 1 != _sensorCount)
        {
            dataHeader += F(", ");
        }
    }

    // Serial.println(dataHeader);  // for debugging
    logFile.println(dataHeader);

    //Close the file to save it
    logFile.close();
}

// ============================================================================
//  Functions for the EnviroDIY data receivers.
// ============================================================================


// Used to empty out the buffer after a post request.
// Removing this may cause communication issues. If you
// prefer to not see the std::out, remove the print statement
void LoggerEnviroDIY::dumpBuffer(Stream *stream, int timeDelay/* = 5*/, int timeout/* = 5000*/)
{
    while (timeout-- > 0 && stream->available() > 0)
    {
        while (stream->available() > 0)
        {
            // Serial.print(stream->readString());
            stream->read();
            delay(timeDelay);
        }
        delay(timeDelay);
    }
    stream->flush();
}


String LoggerEnviroDIY::generateSensorDataJSON(void)
{
    String jsonString = F("{");
    jsonString += F("\"sampling_feature\": \"");
    jsonString += String(_samplingFeature) + F("\", ");
    jsonString += F("\"timestamp\": \"");
    jsonString += String(LoggerBase::markedISO8601Time) + F("\", ");

    for (int i = 0; i < LoggerBase::_sensorCount; i++)
    {
        jsonString += F("\"");
        jsonString += String(_UUIDs[i]) + F("\": ");
        jsonString += LoggerBase::_sensorList[i]->getValueString();
        if (i + 1 != LoggerBase::_sensorCount)
        {
            jsonString += F(", ");
        }
    }

    jsonString += F(" }");
    return jsonString;
}


// This function generates the full POST request that gets sent to data.envirodiy.org
void LoggerEnviroDIY::streamEnviroDIYRequest(Stream *stream)
{
    stream->print(F("POST /api/data-stream/ HTTP/1.1"));
    stream->print(F("\r\nHost: data.envirodiy.org"));
    stream->print(F("\r\nTOKEN: "));
    stream->print(_registrationToken);
    stream->print(F("\r\nCache-Control: no-cache\r\nContent-Length: "));
    stream->print(generateSensorDataJSON().length());
    stream->print(F("\r\nConnection: close"));
    stream->print(F("\r\nContent-Type: application/json\r\n\r\n"));
    stream->print(generateSensorDataJSON());
    stream->print(F("\r\n\r\n"));
}

// This function makes an HTTP connection to the server and POSTs data - for WIFI
int LoggerEnviroDIY::postDataEnviroDIY(void)
{
    // Turn on the modem and connect to the network
    switch(_modemType)
    {
        case GPRSBee6:
        case GPRSBee4:
        case Fona:
        {
            Serial.println(F("Attempting to turn on modem."));  // For debugging
            _modemOnOff->on();
            // Serial.println(F("Attempting to connect."));  // For debugging
            // _modem->waitForNetwork();
            // _modem->gprsConnect(_APN, "", "");
            // _client->connect("data.envirodiy.org", 80);
            break;
        }
        case WIFIBee:
            {break;}
    }

    // Send the request to the serial for debugging
    Serial.println(F("\n \\/---- Post Request to EnviroDIY ----\\/ "));  // for debugging
    streamEnviroDIYRequest(&Serial);  // for debugging
    Serial.flush();  // for debugging

    // Send the request to the modem stream
    dumpBuffer(_modemStream);
    streamEnviroDIYRequest(_modemStream);
    _modemStream->flush();  // wait for sending to finish

    // Add a brief delay for at least the first 12 characters of the HTTP response
    int timeout = 1500;
    while ((timeout > 0) && _modemStream->available() < 12)
    {
      delay(1);
      timeout--;
    }

    // Process the HTTP response
    int responseCode = 0;
    if (timeout > 0 && _modemStream->available() >= 12)
    {
        _modemStream->readStringUntil(' ');
        responseCode = _modemStream->parseInt();
        Serial.println(F(" -- Response Code -- "));  // for debugging
        Serial.println(responseCode);  // for debugging

        dumpBuffer(_modemStream);
    }
    else responseCode=504;

    // Disconnect and turn off the modem
    switch(_modemType)
    {
        case GPRSBee6:
        case GPRSBee4:
        case Fona:
        {
            // _client->stop();
            // _modem->gprsDisconnect();
            _modemOnOff->off();
            break;
        }
        case WIFIBee:
            {break;}
    }

    return responseCode;
}


// Used only for debugging - can be removed
void LoggerEnviroDIY::printPostResult(int HTTPcode)
{
    switch (HTTPcode)
    {
        case 200:
        case 201:
        case 202:
        {
            Serial.println(F("\nData was sent successfully."));
            break;
        }

        case 301:
        case 302:
        {
            Serial.println(F("\nRequest was redirected."));
            break;
        }

        case 400:
        case 404:
        {
            Serial.println(F("\nFailed to send data."));
            break;
        }

        case 403:
        case 405:
        {
            Serial.print(F("\nAccess forbidden.  "));
            Serial.println(F("Check your reguistration token and UUIDs."));
            break;
        }

        case 500:
        case 503:
        {
            Serial.println(F("\nRequest caused an internal server error."));
            break;
        }

        case 504:
        {
            Serial.print(F("\nRequest timed out.  "));
            Serial.println(F("No response from server or insufficient signal to send message."));
            break;
        }

        default:
        {
            Serial.print(F("\nAn unknown error has occured, and we're pretty confused\n"));
            break;
        }
    }
}


// ============================================================================
//  Convience functions to call several of the above functions
// ============================================================================

void LoggerEnviroDIY::log(void)
{
    // Update the timer
    loggerTimer.update();

    // Check of the current time is an even interval of the logging interval
    if (checkInterval())
    {
        // Print a line to show new reading
        Serial.println(F("------------------------------------------"));  // for debugging
        // Turn on the LED to show we're taking a reading
        digitalWrite(LoggerBase::_ledPin, HIGH);

        // Update the static time variables with the current time
        markTime();
        // Update the values from all attached sensors
        updateAllSensors();
        // Immediately put sensors to sleep to save power
        sensorsSleep();

        //Save the data record to the log file
        logToSD(generateSensorDataCSV());

        // Post the data to the WebSDL
        int result = postDataEnviroDIY();

        // Print the response from the WebSDL
        printPostResult(result);  // for debugging

        // Turn off the LED
        digitalWrite(LoggerBase::_ledPin, LOW);
        // Print a line to show reading ended
        Serial.println(F("------------------------------------------\n"));  // for debugging
    }

    //Sleep
    if(LoggerBase::sleep){systemSleep();}
}
