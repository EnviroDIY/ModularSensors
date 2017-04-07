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
{ _registrationToken = registrationToken; }

void LoggerEnviroDIY::setSamplingFeature(const char *samplingFeature)
{ _samplingFeature = samplingFeature; }

void LoggerEnviroDIY::setUUIDs(const char *UUIDs[])
{ _UUIDs = UUIDs; }

loggerModem LoggerEnviroDIY::modem;
void LoggerEnviroDIY::setupModem(modemType modType,
                                 Stream *modemStream,
                                int vcc33Pin,
                                int status_CTS_pin,
                                int onoff_DTR_pin,
                                const char *APN)
{
    modem.setupModem(modType, modemStream,
                     vcc33Pin, status_CTS_pin, onoff_DTR_pin,
                     APN);
}


// Adds the extra UUIDs to the header of the log file
String LoggerEnviroDIY::generateFileHeader(void)
{
    String dataHeader = "";
    dataHeader += LoggerBase::generateFileHeader();
    dataHeader += F("\r\n");

    // Add additional UUID information
    dataHeader += F("\"Sampling Feature: ");
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
    return dataHeader;
}



// ============================================================================
//  Functions for the EnviroDIY data formatting and sending
// ============================================================================
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


// This function makes an HTTP connection to the server and POSTs data - for WIFI
int LoggerEnviroDIY::postDataEnviroDIY(void)
{
    modem.connect("data.envirodiy.org", 80);

    // Send the request to the serial for debugging
    Serial.println(F("\n \\/---- Post Request to EnviroDIY ----\\/ "));  // for debugging
    streamEnviroDIYRequest(&Serial);  // for debugging
    Serial.flush();  // for debugging

    // Send the request to the modem stream
    modem.dumpBuffer();
    streamEnviroDIYRequest(modem._modemStream);
    modem._modemStream->flush();  // wait for sending to finish


    streamEnviroDIYRequest(&Serial1);
    Serial1.flush();  // wait for sending to finish
    delay(100);
    while (Serial1.available()) Serial.print(Serial1.readString());

    // Add a brief delay for at least the first 12 characters of the HTTP response
    int timeout = 1500;
    while ((timeout > 0) && modem._modemStream->available() < 12)
    {
      delay(1);
      timeout--;
    }

    // Process the HTTP response
    int responseCode = 0;
    if (timeout > 0 && modem._modemStream->available() >= 12)
    {
        Serial.println("****" + modem._modemStream->readStringUntil(' ') + "****");  // for debugging
        Serial.println("****" + modem._modemStream->readStringUntil(' ') + "****");  // for debugging
        // modem._modemStream->readStringUntil(' ');
        // responseCode = modem._modemStream->readStringUntil(' ').toInt();
        modem.dumpBuffer();
    }
    else responseCode=504;

    Serial.println(F(" -- Response Code -- "));  // for debugging
    Serial.println(responseCode);  // for debugging

    modem.stop();

    return responseCode;
}


// ============================================================================
//  Convience functions to call several of the above functions
// ============================================================================

void LoggerEnviroDIY::log(void)
{
    // Update the timer
    // This runs the timer's "now" function [in our case getNow()] and then
    // checks all of the registered timer events to see if they should run
    // loggerTimer.update();

    // Check of the current time is an even interval of the logging interval
    if (checkInterval())
    {
        // Print a line to show new reading
        Serial.println(F("------------------------------------------"));  // for debugging
        // Turn on the LED to show we're taking a reading
        digitalWrite(LoggerBase::_ledPin, HIGH);

        // Update the time variables with the current time
        markTime();
        // Update the values from all attached sensors
        updateAllSensors();
        // Immediately put sensors to sleep to save power
        sensorsSleep();

        //Save the data record to the log file
        logToSD(generateSensorDataCSV());

        // Connect to the network
        if (modem.connectNetwork())
        {
            // Post the data to the WebSDL
            postDataEnviroDIY();

            // Print the response from the WebSDL
            // int result = postDataEnviroDIY();
            // modem.printHTTPResult(result);  // for debugging

            // Disconnect from the network
            modem.disconnectNetwork();
        }

        // Turn off the LED
        digitalWrite(LoggerBase::_ledPin, LOW);
        // Print a line to show reading ended
        Serial.println(F("------------------------------------------\n"));  // for debugging
    }

    //Sleep
    if(_sleep){systemSleep();}
}
