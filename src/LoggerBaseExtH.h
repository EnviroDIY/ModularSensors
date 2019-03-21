#ifndef LoggerBaseExtH_h
#define LoggerBaseExtH_h

    // ===================================================================== //
    // Public functions extensions for basic logging paramters
    // ===================================================================== //

    //void setLoggingInterval_atl485(uint16_t loggingIntervalMinutes);

    void setLoggerId(const char *newLoggerId,bool copyId=false,uint8_t LoggerIdSize=NEW_LOGGERID_MAX_SIZE);

    // ===================================================================== //
    // Public functions for a "reading .ini" file
    // ===================================================================== //

public:
    bool parseIniSd(const char *ini_fn,ini_handler_atl485 handler_fn);
    //virtual bool parseIniSd(const char *ini_fn,ini_handler handler_fn);

private:
    //int inihandlerFn( const char* section, const char* name, const char* value);
    // Parse the supplied File for ModularSensors [Section] name=value pairs
    // return !0 for error with line number
    virtual int8_t inihParseFile(ini_handler_atl485 handler_fn);
    // Tokens and UUID's for EnviroDIY
    const char *_registrationToken;
    const char *_samplingFeature;
    const char *_LoggerId_buf=NULL;

#endif //LoggerBaseExt_h
