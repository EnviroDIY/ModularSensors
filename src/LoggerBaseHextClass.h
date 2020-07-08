#ifndef LoggerBaseExtH_h
#define LoggerBaseExtH_h

// ===================================================================== //
// Public functions extensions for basic logging paramters
// ===================================================================== //

// void setLoggingInterval_atl485(uint16_t loggingIntervalMinutes);

void setLoggerId(const char* newLoggerId, bool copyId = false,
                 uint8_t LoggerIdSize = NEW_LOGGERID_MAX_SIZE);

// ===================================================================== //
// Public functions for a "reading .ini" file
// ===================================================================== //

public:
bool parseIniSd(const char* ini_fn, ini_handler_atl485 handler_fn);
#ifdef USE_MS_SD_INI
void setPs_cache(persistent_store_t* ps_ram);
void printFileHeaderExtra(Stream* stream);
#endif  // USE_MS_SD_INI
// virtual bool parseIniSd(const char *ini_fn,ini_handler handler_fn);
void        SDusbPoll(uint8_t sdActions);
#if defined USE_RTCLIB
USE_RTCLIB* rtcExtPhyObj();
#endif  // USE_RTCLIB
static bool usbDriveActive(void);
// Time woken up
uint32_t wakeUpTime_secs;

private:
static int32_t sd1_card_read_cb(uint32_t lba, void* buffer, uint32_t bufsize);
static int32_t sd1_card_write_cb(uint32_t lba, uint8_t* buffer,
                                 uint32_t bufsize);
static void    sd1_card_flush_cb(void);
static int32_t sdq_flashspi_read_cb(uint32_t lba, void* buffer,
                                    uint32_t bufsize);
static int32_t sdq_flashspi_write_cb(uint32_t lba, uint8_t* buffer,
                                     uint32_t bufsize);
static void    sdq_flashspi_flush_cb(void);
static bool    sdq_ready(void);  // Called when USB present and checking of
                                 // readiness

persistent_store_t* ps_cache = NULL;

// void print_rootdir(File* rdir);
bool SDextendedInit(bool sdSuccess);
// int inihandlerFn( const char* section, const char* name, const char* value);
// Parse the supplied File for ModularSensors [Section] name=value pairs
// return !0 for error with line number
virtual int8_t inihParseFile(ini_handler_atl485 handler_fn);
// Tokens and UUID's for EnviroDIY
const char* _registrationToken;
const char* _samplingFeature;
const char* _LoggerId_buf = NULL;

// The SD card and file
#if 0  // defined BOARD_SDQ_QSPI_FLASH
       // This can be on the Adafruit Express options
       //#include "Adafruit_SPIFlash.h"
#if defined(__SAMD51__) || defined(NRF52840_XXAA)
        static Adafruit_FlashTransport_QSPI sdq_flashspi_transport_QSPI_phy; //Uses default pin assignments
#else
#if (SPI_INTERFACES_COUNT == 1)
                Adafruit_FlashTransport_SPI sdq_flashspi_transport_QSPI_phy(SS, &SPI);
#else
                Adafruit_FlashTransport_SPI sdq_flashspi_transport_QSPI_phy(SS1, &SPI1);
#endif
#endif  //__SAMD51__ ..
        // Adafruit_SPIFlash sdq_flashspi_phy(&sdq_flashspi_transport_QSPI_phy);
        // The following needs to be extended to allow
        // sdq_flashspi_transport_QSPI to be added at runtime This is in
        // Adafruit_SPIFlashBase(Adafruit_SPIFlashBase)
        static Adafruit_SPIFlash sdq_flashspi_phy; ** needs to init sdq_flashspi_transport_QSPI_phy
        static FatFileSystem sdq_card_phy; //// sdq_flashspi_fatfs;  File system object on external flash from SdFat
#endif  // SDQ_QSPI_FLASH
#endif  // LoggerBaseExt_h
