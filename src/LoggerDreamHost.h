/*
 *LoggerDreamHost.h
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Initial library developement done by Sara Damiano (sdamiano@stroudcenter.org).
 *
 *This file is for the EnviroDIY logging functions - ie, sending JSON data to
 * http://data.enviroDIY.org
*/

#ifndef LoggerDreamHost_h
#define LoggerDreamHost_h


#include <Arduino.h>
#include "LoggerEnviroDIY.h"

// Defines the "Logger" Class
class LoggerDreamHost : public virtual LoggerEnviroDIY
{
public:
    // Functions for private SWRC server
    void setDreamHostURL(const char *URL);
    String generateSensorDataDreamHost(void);
    int postDataDreamHost(void);

    // Convience functions to do it all
    void log(void) override;

private:
    const char *_DreamHostURL;
};

#endif
