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
    
    // The SD card and file
    #ifdef SDQ_QSPI_FLASH
    //This can be on the Adafruit Express options
    #include "Adafruit_SPIFlash.h"
    #if defined(__SAMD51__) || defined(NRF52840_XXAA)
    //Adafruit_FlashTransport_QSPI sdq_flashspi_transport_QSPI(PIN_QSPI_SCK, PIN_QSPI_CS, PIN_QSPI_IO0, PIN_QSPI_IO1, PIN_QSPI_IO2, PIN_QSPI_IO3);
    Adafruit_FlashTransport_QSPI sdq_flashspi_transport_QSPI;
    #endif
    //Adafruit_SPIFlash sdq_flashspi_phy(&sdq_flashspi_transport_QSPI);
    Adafruit_SPIFlash sdq_flashspi_phy(void);
    FatFileSystem sdq_card; //// sdq_flashspi_fatfs;  File system object on external flash from SdFat
    #endif //SDQ_QSPI_FLASH
#endif //LoggerBaseExt_h
