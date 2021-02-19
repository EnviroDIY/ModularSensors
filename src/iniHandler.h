// ==========================================================================
// inihUnhandled
// For any Unhandled sections this is called
// ==========================================================================
#ifdef USE_MS_SD_INI
#include <errno.h>
// expect to be in near space
#define epc ps_ram
#define SIZE_UINT16_CRC16 sizeof(uint16_t)
//#define EDIY_PROGMEM PROGMEM
#define mCONST_UNI(p1) const char p1##_pm[] PROGMEM = #p1
const char BOOT_pm[] EDIY_PROGMEM         = "BOOT";
const char BOARD_NAME_pm[] EDIY_PROGMEM   = "BOARD_NAME";
const char BOARD_SN_pm[] EDIY_PROGMEM     = "BOARD_SN";
const char BOARD_REV_pm[] EDIY_PROGMEM    = "BOARD_REV";
const char SD_INIT_ID_pm[] EDIY_PROGMEM   = "SD_INIT_ID";
const char EEPROM_WRITE_pm[] EDIY_PROGMEM = "EEPROM_WRITE";
const char YES_pm[] EDIY_PROGMEM          = "YES";

const char COMMON_pm[] EDIY_PROGMEM    = "COMMON";
const char LOGGER_ID_pm[] EDIY_PROGMEM = "LOGGER_ID";
// mCONST_UNI(LOGGER_ID);// = "nh07k" ;
const char LOGGING_INTERVAL_MINUTES_pm[] EDIY_PROGMEM =
    "LOGGING_INTERVAL_MINUTES";
const char LOGGING_INTERVAL_MULTIPLIER_pm[] EDIY_PROGMEM =
    "LOGGING_INTERVAL_MULTIPLIER";
const char BATTERY_TYPE_pm[] EDIY_PROGMEM = "BATTERY_TYPE";
const char LIION_TYPE_pm[] EDIY_PROGMEM =
    "LIION_TYPE";  // FUT Supersede with BATTERY_TYPE
const char TIME_ZONE_pm[] EDIY_PROGMEM          = "TIME_ZONE";

const char GEOGRAPHICAL_ID_pm[] EDIY_PROGMEM    = "GEOGRAPHICAL_ID";

const char NETWORK_pm[] EDIY_PROGMEM = "NETWORK";
const char apn_pm[] EDIY_PROGMEM     = "apn";
const char WiFiId_pm[] EDIY_PROGMEM  = "WiFiId";
const char WiFiPwd_pm[] EDIY_PROGMEM = "WiFiPwd";
const char COLLECT_READINGS_pm[] EDIY_PROGMEM   = "COLLECT_READINGS";
const char SEND_OFFSET_MIN_pm[] EDIY_PROGMEM    = "SEND_OFFSET_MIN";

// INA219 configurable parms
const char INA219M_MA_MULT_pm[] EDIY_PROGMEM     = "INA219M_MA_MULT";
const char INA219M_V_THRESHLOW_pm[] EDIY_PROGMEM = "INA219M_V_THRESHLOW";

const char PROVIDER_MMW_pm[] EDIY_PROGMEM           = "PROVIDER_MMW";

const char CLOUD_ID_pm[] EDIY_PROGMEM           = "CLOUD_ID";
const char REGISTRATION_TOKEN_pm[] EDIY_PROGMEM = "REGISTRATION_TOKEN";
const char SAMPLING_FEATURE_pm[] EDIY_PROGMEM   = "SAMPLING_FEATURE";
const char TIMER_POST_TOUT_MS_pm[] EDIY_PROGMEM = "TIMER_POST_TOUT_MS";
const char TIMER_POST_PACE_MS_pm[] EDIY_PROGMEM = "TIMER_POST_PACE_MS";
const char POST_MAX_NUM_pm[] EDIY_PROGMEM       = "POST_MAX_NUM";

const char SENSORS_pm[] EDIY_PROGMEM = "SENSORS";
const char index_pm[] EDIY_PROGMEM   = "index";

const char USER_pm[] EDIY_PROGMEM   = "USER";
const char ACTION_pm[] EDIY_PROGMEM = "ACTION";
const char WRITE_pm[] EDIY_PROGMEM  = "WRITE";
const char DEFAULT_APP_EEPROM_pm[] EDIY_PROGMEM  = "DEFAULT_APP_EEPROM";
const char COPY_pm[] EDIY_PROGMEM   = "COPY";

static uint8_t uuid_index = 0;
static uint8_t uuid_ram_idx = 0;

#if defined USE_PS_EEPROM && defined ARDUINO_AVR_ENVIRODIY_MAYFLY
//
// Compute a 16 bit CRC.
//
#include <util/crc16.h>
uint16_t calc_crc16(const uint8_t* addr, uint16_t len) {
    uint16_t crc16 = 0;

    while (len--) {
        uint8_t inbyte = *addr++;
        crc16          = _crc16_update(crc16, inbyte);
    }
    return crc16;
}
#endif  // USE_PS_EEPROM
#if defined(ARDUINO_AVR_ENVIRODIY_MAYFLY)
#define RAM_AVAILABLE ramAvailable();
#define RAM_REPORT_LEVEL 1
void ramAvailable() {
    extern int16_t __heap_start, *__brkval;
    uint16_t       top_stack = (int)&top_stack -
        (__brkval == 0 ? (int)&__heap_start : (int)__brkval);
    SerialStd.print(F(" Ram available:"));
    SerialStd.println(top_stack);  // Stack and heap ??
}
#elif defined(ARDUINO_ARCH_SAMD)
extern "C" char* sbrk(int i);
#define RAM_AVAILABLE ramAvailable();
#define RAM_REPORT_LEVEL 1
void             ramAvailable() {
    char stack_dummy = 0;
    SerialStd.print(F(" Ram available:"));
    SerialStd.println(&stack_dummy - sbrk(0));  // Stack and heap ??
}
#endif  // ARDUINO_AVR_ENVIRODIY_MAYFLY
void       ledflash(uint8_t numFlash = 4, unsigned long onTime_ms = 75,
                    unsigned long offTime_ms = 150);
void localAppInit();

// Wrte the epc.app to EEPROM
void localEepromAppWr(const char *srcdbg) 
{
    PRINTOUT(F("ACTION Write app EEPROM started("),srcdbg,F(")"));
    uint16_t crc16 = calc_crc16(
        (const uint8_t*)((int)&epc.app + SIZE_UINT16_CRC16),
        ((uint16_t)sizeof(epc.app) - SIZE_UINT16_CRC16));
        epc.app.crc16 = crc16;
    EEPROM.put(EP_PERSISTENT_STORE_ADDR, epc.app);

    MS_DBG(F("app eeprom wr"), sizeof(epc), F("crc="), epc.app.crc16
        // F("sz="),epc.app.struct_size,
        // F("ver="),epc.app.struct_ver
        // F("name="),(char *)hw_boot.board_name,
        // F("ini="),(char *)epc.hw_boot.sd_boot_ini
    );
    SerialStd.println(F("EEPROM Write finished"));
}

/* Parse the pesistent configuration data.
 * The data has been read from EEPROM into a ram cache, 
 * and then if exists in the .ini file, overwritten in the ram
 * Now 
 * ~ instate them in classes where needed
 *  ~ Searh for name:value pairs that match a UUD that have not already been found
 * 
*/
static void epcParser() {
    int8_t uuid_vl_idx = 0;
    int eeprom_idx=0;

    MS_DBG(F("epcParser assign from cached eeprom or ini"));


    if (isalnum(epc_logger_id1st))
    {
        for (int i = 0; epc.app.msc.s.logger_id[i] != '\0'; i++)
        {
            if (!isprint(epc.app.msc.s.logger_id[i]) )
            {
                PRINTOUT(F("Error !alnum logger file pos"),i,F("setting to '_', found "),epc.app.msc.s.logger_id[i]);
                epc.app.msc.s.logger_id[i] = '_';
            }
        }
        PRINTOUT(F("COMMON LoggerId Set: "), epc_logger_id);
        dataLogger.setLoggerId(epc_logger_id , false);

    }

    dataLogger.setLoggingInterval(epc_logging_interval_min);
    PRINTOUT(F("COMMON Logginterval: "), epc_logging_interval_min);

    mcuBoard.setBatteryType((ps_liion_rating_t)epc_battery_type);
    PRINTOUT(F("COMMON Battery Type: "), epc_battery_type);

    Logger::setLoggerTimeZone(epc.app.msc.s.time_zone);

    /// Used  in uSD print epc.app.msc.s.geolocation_id

    #if defined DigiXBeeCellularTransparent_Module
    if (isalnum(epc_apn1st))
    {
            epc.app.msn.s.network_type=MSCN_TYPE_CELL;
            SerialStd.print(F("NETWORK APN was '"));
            SerialStd.print(modemPhy.getApn());
            modemPhy.setApn(epc_apn, false);
            SerialStd.print(F("', now set to '"));
            SerialStd.print(modemPhy.getApn());
            SerialStd.println("'");
    }
    #endif  // DigiXBeeCellularTransparent_Module
    #if defined DigiXBeeWifi_Module
    // cheeck for WiFiId and WiFiPwd
    if (isalnum(epc_WiFiId1st))
    {
        SerialStd.print(F("NETWORK WiFiId: was '"));
        SerialStd.print(modemPhy.getWiFiId());
        modemPhy.setWiFiId(epc_WiFiId, false);
        SerialStd.print(F("' now '"));
        SerialStd.print(modemPhy.getWiFiId());
        SerialStd.println("'");
    } 
    if( isalnum(epc_WiFiPwd1st)) 
    {
            SerialStd.print(F("NETWORK WiFiPwd: was '"));
            SerialStd.print(modemPhy.getWiFiPwd());
            modemPhy.setWiFiPwd(epc_WiFiPwd, false);
            SerialStd.print(F("' now '"));
            SerialStd.print(modemPhy.getWiFiPwd());
            SerialStd.println("'");
    }
    #endif // DigiXBeeWifi_Module

    PRINTOUT(F("NETWORK CollectReadings"),epc.app.msn.s.collectReadings_num );
    PRINTOUT(F("NETWORK sendOffset_min"),epc.app.msn.s.sendOffset_min);
    
    //[Provider]
    //CLOUD_ID=data.enviroDIY.com - hard coded
    PRINTOUT(F("PROVIDER_MMW CloudId"),             epc.app.provider.s.ed.cloudId);
    PRINTOUT(F("PROVIDER_MMW Registration Token"),  epc.app.provider.s.ed.registration_token);
    PRINTOUT(F("PROVIDER_MMW Samping Feature"),     epc.app.provider.s.ed.sampling_feature);
    PRINTOUT(F("PROVIDER_MMW timerPost Tout(ms)"),  epc.app.provider.s.ed.timerPostTout_ms);
    PRINTOUT(F("PROVIDER_MMW timerPost Pacing(ms)"),epc.app.provider.s.ed.timerPostPace_ms);
    //POST_MAX_NUM=0; Fut Not used

    //Check for any unassigned NAME:VALUE
    int idx_break=0;
    do {
        if (isalnum(epc.app.provider.s.ed.uuid[eeprom_idx].value[0]) 
        && isalnum(epc.app.provider.s.ed.uuid[eeprom_idx].name[0])) 
        {
            //Found alpha values in table, search for a match
            MS_DEEP_DBG(F("search"),uuid_name(eeprom_idx),eeprom_idx, F("?"),uuid_vl_idx,
            (const char*)variableList[uuid_vl_idx]->getVarUUID().c_str()  );
            if (strcmp((const char*)variableList[uuid_vl_idx]
                        ->getVarUUID().c_str(),
                        uuid_name(eeprom_idx)) == 0) 
            {  // Found a match
                variableList[uuid_vl_idx]->setVarUUID_atl((char*)epc.app.provider.s.ed.uuid[eeprom_idx].value, false);
                PRINTOUT(F("PROVIDER_MMW"),uuid_name(eeprom_idx),uuid_value(eeprom_idx),F("->"),
                 variableList[uuid_vl_idx]->getVarCode());
                eeprom_idx++;
                uuid_vl_idx = -1; ///Reset to start at beging of variable_list
            }
            #ifdef MS_TU_CTD_DEBUG
            delay(100); //Don;t overpower debug output.
            #endif
        } else {
            MS_DBG(F("search !isalnum eeprom["),eeprom_idx, F("] "),
            isalnum(epc.app.provider.s.ed.uuid[eeprom_idx].value[0]),
            isalnum(epc.app.provider.s.ed.uuid[eeprom_idx].name[0]) ) ;
        }
        uuid_vl_idx++;
        if (uuid_vl_idx>=variableCount) { 
            eeprom_idx++;
            uuid_vl_idx = 0;
        }
        if (++idx_break > (UUIDE_SENSOR_CNT_MAX_SZ*UUIDE_SENSOR_CNT_MAX_SZ)) {
            PRINTOUT(F("Search error! break out of loop"));
            break;
        }

    }while (eeprom_idx < UUIDE_SENSOR_CNT_MAX_SZ );
}

static int inihUnhandledFn(const char* section, const char* name,
                           const char* value) {
#if RAM_REPORT_LEVEL > 1
    bool ram_track = true;
#endif
    char* endptr;
    errno = 0;
// MS_DBG(F("inih "),section," ",name," ",value);
#if defined USE_PS_Provider
    if (strcmp_P(section, PROVIDER_MMW_pm) == 0) {
        //This [PROVIDER] only defined for EnviroDIY
        // For other providers needs to be extended/switch
        if (strcmp_P(name, REGISTRATION_TOKEN_pm) == 0) {
            strcpy(epc.app.provider.s.ed.registration_token, value);
            MS_DBG(F("PROVIDER_MMW Setting registration token: "),
            epc.app.provider.s.ed.registration_token);
        } else if (strcmp_P(name, CLOUD_ID_pm) == 0) {
            strcpy(epc.app.provider.s.ed.cloudId, value);
            MS_DBG(F("PROVIDER_MMW Setting cloudId: "),
            epc.app.provider.s.ed.cloudId);
        } else if (strcmp_P(name, SAMPLING_FEATURE_pm) == 0) {
            strcpy(epc.app.provider.s.ed.sampling_feature, value);
            MS_DBG(F("PROVIDER_MMW Setting SamplingFeature: "),
            epc.app.provider.s.ed.sampling_feature);
        }else if (strcmp_P(name, TIMER_POST_TOUT_MS_pm) == 0) {
            // convert  str to num with error checking
            long timerPostTimeout_local = strtol(value, &endptr, 10);
            if ((timerPostTimeout_local <= 30000) &&
                (timerPostTimeout_local >= 100) && (errno != ERANGE)) 
            {

            } else {
                PRINTOUT(F("PROVIDER_MMW Set TIMER_POST_TOUT_MS error; (range "
                                  "100 : 30000) read:"),timerPostTimeout_local);
                timerPostTimeout_local =MMW_TIMER_POST_TIMEOUT_MS_DEF;
            }
            epc.app.provider.s.ed.timerPostTout_ms = timerPostTimeout_local;
            MS_DBG(F("PROVIDER_MMW Set TIMER_POST_TOUT_MS : "),timerPostTimeout_local);

        } else  if (strcmp_P(name, TIMER_POST_PACE_MS_pm) == 0) {
            // convert  str to num with error checking
            long timerPostPacing_local = strtol(value, &endptr, 10);
            if ((timerPostPacing_local <= 5000) &&
                (timerPostPacing_local >= 0) && (errno != ERANGE)) 
            {
            } else {
                PRINTOUT(F("PROVIDE_MMWSet TIMER_POST_PACE_MS error; (range "
                                  "0 : 5000) read:"),timerPostPacing_local);
                timerPostPacing_local= MMW_TIMER_POST_PACING_MS_DEF;
            }
            epc.app.provider.s.ed.timerPostPace_ms = timerPostPacing_local;
            MS_DBG(F("PROVIDE_MMWSet TIMER_POST_PACE_MS: "),timerPostPacing_local);

        } else if (strcmp_P(name, POST_MAX_NUM_pm) == 0) {
            // convert  str to num with error checking
            long postMax_num_local = strtol(value, &endptr, 10);
            if ((postMax_num_local <= 50) && (postMax_num_local >= 0) &&
                (errno != ERANGE)) 
            {

            } else {
                PRINTOUT(
                    F("PROVIDER_MMW Set POST_MAX_NUM error; (range 0 : 50) read:"),postMax_num_local);
                    postMax_num_local=MMW_TIMER_POST_MAX_MUM_DEF;
            }
            //postMax_num = (uint8_t)postMax_num_local;
            epc.app.provider.s.ed.postMax_num = postMax_num_local;
            MS_DBG(F("PROVIDER_MMW Set POST_MAX_NUM: "),postMax_num_local);

        } else {

            /* UUIDs are applied to internal sensor Array as follows:
            1) "UUID_label"="UUID"
            eg ASQ212_PAR="UUID"
            search variableList for UUID_label and if found replace with "UUID"
            2) index="UUID"
            if the word "index" is there with a UUID, then the UUID is applied in
            sequence. Any UUID_label's found also increment the counted 'index'
            */
  
            uint8_t uuid_search_i = 0;

            //SerialStd.print(F(""));
            SerialStd.print(uuid_index);
            SerialStd.print(":");
            SerialStd.print(name);
            SerialStd.print(F("={"));
            SerialStd.print(value);
            do {
                MS_DEEP_DBG(F("\n ["), uuid_search_i, F("]"),
                            variableList[uuid_search_i]->getVarUUID().c_str(), "#");
                if (strcmp((const char*)variableList[uuid_search_i]
                            ->getVarUUID()
                            .c_str(),
                        name) == 0) {  // Found a match

                    // Add to epc where it can be referenced
                    strcpy(uuid_name(uuid_ram_idx),name);
                    strcpy(uuid_value(uuid_ram_idx),value);
                    #if 0
                    variableList[uuid_search_i]->setVarUUID_atl(uuid_value(uuid_ram_idx), false);
                    MS_DEEP_DBG(F("set"), name, F(" for ["), uuid_search_i, F("]"),
                                variableList[uuid_search_i]->getVarUUID().c_str());
                    #endif
                    uuid_search_i = variableCount;
                    uuid_ram_idx++;
                }
                uuid_search_i++;
            } while (uuid_search_i < variableCount);

            if (uuid_search_i > variableCount) {
                SerialStd.println(F("} match  & added."));

    #if 0//superseded
            } else if (strcmp_P(name, index_pm) ==
                    0) {  // Check if index and then simple reference
                if (uuid_index < variableCount) {
                    SerialStd.print(F("} replacing {"));
                    SerialStd.print(variableList[uuid_index]->getVarUUID());
                    SerialStd.println(F("}"));
                    variableList[uuid_index]->setVarUUID_atl((char*)value, true);
                } else {
                    SerialStd.println(F("} out of range. Notused"));
                }
    #endif //0
            } else {
                SerialStd.println(F("} not supported"));
            }
            uuid_index++;
        }
    } else
#endif // USE_PS_Provider

    if (strcmp_P(section, COMMON_pm) == 0)   // [COMMON] processing
    {

        if (strcmp_P(name, LOGGER_ID_pm) == 0) {

#if defined USE_PS_EEPROM
            strcpy(epc_logger_id, value);
#else
            dataLogger.setLoggerId(value, true);
#endif  // USE_PS_EEPROM
        } else if (strcmp_P(name, LOGGING_INTERVAL_MINUTES_pm) == 0) 
        {
            // convert str to num with error checking
            long intervalMin = strtoul(value, &endptr, 10);
            if ((intervalMin > 0) && (errno != ERANGE)) 
            {
                if (intervalMin > loggingInterval_MAX_CDEF_MIN) {
                    SerialStd.print(F("COMMON LOGGING_INTERVAL_MINUTES must be "
                                      "less than : "));
                    SerialStd.print(loggingInterval_MAX_CDEF_MIN);
                    SerialStd.print(F(" Using Max. Reading ignored "));
                    SerialStd.println(intervalMin);
                    intervalMin = loggingInterval_MAX_CDEF_MIN;
                } else {
                    SerialStd.print(
                        F("COMMON LOGGING_INTERVAL_MINUTES set to: "));
                    SerialStd.print(intervalMin);
                    SerialStd.print(F("(min) from default "));
                    SerialStd.println(loggingInterval_CDEF_MIN);
                }
#if defined loggingMultiplier_MAX_CDEF
                dataLogFast.setLoggingInterval(intervalMin);
#endif  // loggingMultiplier_MAX_CDEF

        // loggingInterval_def_min = intervalMin; //Dup for time being
            } else {
                SerialStd.print(F(" Set interval error (range: 1-"));
                SerialStd.print(loggingInterval_MAX_CDEF_MIN);
                SerialStd.print(F(") with read:"));
                SerialStd.println(intervalMin);
                intervalMin = loggingInterval_MAX_CDEF_MIN;
            }
#if defined USE_PS_EEPROM
            epc_logging_interval_min = intervalMin;
#endif  // USE_PS_EEPROM
#if defined loggingMultiplier_MAX_CDEF
        } else if (strcmp_P(name, LOGGING_INTERVAL_MULTIPLIER_pm) == 0) {
            // convert str to num with error checking
            long multNum = strtoul(value, &endptr, 10);
            if ((multNum > 0) && (errno != ERANGE)) {
                if (multNum > loggingInterval_MAX_CDEF_MIN) {
                    PRINTOUT(
                        F("COMMON LOGGING_MULTIPLIER must be less than : "),
                        loggingInterval_MAX_CDEF_MIN,
                        F(" Using Max. Reading ignored "), multNum);
                    multNum = loggingMultiplier_MAX_CDEF;
                } else {
                    PRINTOUT(F("COMMON LOGGING_MULTIPLIER set to: "), multNum,
                             F("(mult) from default "),
                             loggingMultiplier_MAX_CDEF);
                }
                loggingMultiplierTop = multNum;  // In the main program
            } else {
                SerialStd.print(F(" Set interval error (range: 1-"));
                SerialStd.print(loggingInterval_MAX_CDEF_MIN);
                SerialStd.print(F(") with read:"));
                SerialStd.println(multNum);
            }
            #warning multNum not in eeprom or used?
#endif  // loggingMultiplier_MAX_CDEF
        } else if (strcmp_P(name, BATTERY_TYPE_pm) == 0) {
            // convert  str to num with error checking
            long batteryType = strtoul(value, &endptr, 10);
            if ((batteryType < PSLR_NUM) && (batteryType > 0) &&
                (errno != ERANGE)) {
            } else {
                PRINTOUT(F(" Set BATTERY_TYPE error; (range 0-"),PSLR_NUM,F(") read:"),batteryType); 
                batteryType=PSLR_ALL;
            }
#if defined USE_PS_EEPROM
            epc_battery_type = batteryType;
#endif  // USE_PS_EEPROM
        } else if (strcmp_P(name, LIION_TYPE_pm) ==
                   0) {  // Supersed by BATTERY_TYPE if exists
            // convert  str to num with error checking
            long batLiionType = strtoul(value, &endptr, 10);
            if ((batLiionType < PSLR_NUM) && (batLiionType >= 0) &&
                (errno != ERANGE)) {
                PRINTOUT(F("COMMON LiIon Type: "),
                batLiionType,F(" superseded use BATTERY_TYPE"));
            } else {
                PRINTOUT(F(" Set LiIon Type error; (range 0-"),PSLR_NUM,F(") read:"),batLiionType);
                batLiionType=PSLR_ALL;
            }
#if defined USE_PS_EEPROM
            epc_battery_type = batLiionType;
#endif  // USE_PS_EEPROM
            MS_DBG(F("COMMON Battery Type: "), batLiionType);
        } else if (strcmp_P(name, TIME_ZONE_pm) == 0) {
            // convert  str to num with error checking
            long time_zone_local = strtol(value, &endptr, 10);
            if ((time_zone_local < 13) && (time_zone_local > -13) &&
                (errno != ERANGE)) 
            {
            } else {
                PRINTOUT(F("COMMON Set TimeZone error; (range -12 : +12) read:"),time_zone_local);
                time_zone_local = CONFIG_TIME_ZONE_DEF;
            }
#if defined USE_PS_EEPROM 
            epc.app.msc.s.time_zone = time_zone_local;
#endif  // USE_PS_EEPROM
            MS_DBG(F("COMMON Set TimeZone: "), time_zone_local);
        } else if (strcmp_P(name, GEOGRAPHICAL_ID_pm) == 0) {
            SerialStd.print(F("GEOGRAPHICAL_ID:"));
            SerialStd.println(value);
            if (strlen(value) >= (MSC_GEOLOCATION_ID_SZ - 1)) {
                MS_DBG(F("Too long limited to "), MSC_GEOLOCATION_ID_SZ - 1);
                *((char*)((int)value + MSC_GEOLOCATION_ID_SZ)) = 0;
            }
#if defined USE_PS_EEPROM
            strcpy((char*)epc.app.msc.s.geolocation_id, value);
#endif  // USE_PS_EEPROM
        } else {
            SerialStd.print(F("COMMON tbd "));
            SerialStd.print(name);
            SerialStd.print(F(" to "));
            SerialStd.println(value);
        }
    } else if (strcmp_P(section, SENSORS_pm) == 0) {
#if defined INA219M_PHY_ACT
        if (strcmp_P(name, INA219M_MA_MULT_pm) == 0) {
            // For INA219M_MA_MULT expect a string with +ve number and covert to
            // float
            char* endptr;
            float ampMult = (float)strtod(value, &endptr);
            // MS_DBG("Found ", value," conv ", ampMult);
            if ((ampMult > 0) && (errno != ERANGE)) {
                SerialStd.print(F("SENSORS INA219_MA_MULT was '"));
                SerialStd.print(ina219m_phy.getCustomAmpMult());
                #warning move to epcParser
                ina219m_phy.setCustomAmpMult(ampMult);
                SerialStd.print(F("' set to '"));
                SerialStd.print(ina219m_phy.getCustomAmpMult());
                SerialStd.println("'");
            } else {
                SerialStd.print(F("SENSOR INA219_MA_MULT error:"));
                SerialStd.println(value);
            }
        } else if (strcmp_P(name, INA219M_V_THRESHLOW_pm) == 0) {
            // For INA219M_V_THRESHLOW_pm expect a string with +ve number and
            // covert to float
            float voltThreshold = (float)strtod(value, NULL);
            // MS_DBG("Found ", value," conv ", voltThreshold);
            if ((voltThreshold > 0) && (errno != ERANGE)) {
                SerialStd.print(F("SENSORS INA219_V_THRESHOLD was'"));
                SerialStd.print(ina219m_phy.getCustomVoltThreshold());
                #warning move to epcParser
                ina219m_phy.setCustomVoltThreshold(
                    voltThreshold, ina219m_voltLowThresholdAlertFn);
                SerialStd.print(F("' set to '"));
                SerialStd.print(ina219m_phy.getCustomVoltThreshold());
                SerialStd.println("'");
            } else {
                SerialStd.print(F("SENSOR INA219_V_THRESHOLD error:"));
                SerialStd.println(value);
            }
        } else
#endif  // INA219M_PHY_ACT
        {
            SerialStd.print(F("SENSORS tbd "));
            SerialStd.print(name);
            SerialStd.print(F(" to "));
            SerialStd.println(value);
        }
    } else if (strcmp_P(section, NETWORK_pm) == 0) {
        // NETWORK PARTS
#if defined DigiXBeeCellularTransparent_Module
        if (strcmp_P(name, apn_pm) == 0) {
            #if defined USE_PS_EEPROM
            strcpy(epc_apn, value);
            #endif  // USE_PS_EEPROM
        } else
#endif  // DigiXBeeCellularTransparent_Module

#if defined DigiXBeeWifi_Module
        if (strcmp_P(name, WiFiId_pm) == 0) 
        {
            //Set the internet type as WIFI - future may be configurable
            epc.app.msn.s.network_type=MSCN_TYPE_WIFI;
            strcpy(epc_WiFiId , value);
            MS_DBG(F("Use Ini WiFiId"), value);
        } else if (strcmp_P(name, WiFiPwd_pm) == 0) {
            //Expect there to be WiFiId
            strcpy((char*)epc.app.msn.s.WiFiPwd, value);
            MS_DBG(F("Use Ini WiFiPwd"), value);
        } else
#endif  // DigiXBeeWifi_Module

        if (strcmp_P(name, COLLECT_READINGS_pm) == 0) {
            // convert  str to num with error checking
            long collect_readings_local = strtol(value, &endptr, 10);
            if ((collect_readings_local <= 30) &&
                (collect_readings_local >= 0) && (errno != ERANGE)) 
            {
            } else {
                PRINTOUT(F("COMMON Set COLLECT_READINGS error; (range 0 : 30) read:"),collect_readings_local);
                collect_readings_local = MNGI_COLLECT_READINGS_DEF;
            }
            MS_DBG(F("COMMON Set COLLECT_READINGS: "), collect_readings_local);
            epc.app.msn.s.collectReadings_num = (uint8_t)collect_readings_local;

        } else if (strcmp_P(name, SEND_OFFSET_MIN_pm) == 0) {
            // convert  str to num with error checking
            long send_offset_min_local = strtol(value, &endptr, 10);
            if ((send_offset_min_local <= 30) && (send_offset_min_local >= 0) &&
                (errno != ERANGE)) 
            {

            } else {
                PRINTOUT(F("COMMON Set SEND_OFFSET_MIN error; (range 0 : 30) read:"),
                send_offset_min_local);
                send_offset_min_local=MNGI_SEND_OFFSET_MIN_DEF;
            }
            MS_DBG(F("COMMON Set SEND_OFFSET_MIN: "),send_offset_min_local);   
            epc.app.msn.s.sendOffset_min =  (uint8_t)send_offset_min_local;;
        } else

        {
            SerialStd.print(F("NETWORK tbd "));
            SerialStd.print(name);
            SerialStd.print(F(" to "));
            SerialStd.println(value);
        }
#if defined USE_PS_HW_BOOT
    } else if (strcmp_P(section, BOOT_pm) == 0) {
#if 0
        //FUT: needs to go into EEPROM
        const char VER_pm[] EDIY_PROGMEM = "VER";
const char SD_INIT_ID_pm[] EDIY_PROGMEM = "SD_INIT_ID";
#endif
        if (strcmp_P(name, BOARD_NAME_pm) == 0) {
            if (strlen(value) >= (HW_BOOT_BOARD_NAME_SZ - 1)) {
                MS_DBG(F("Too long limited to "), HW_BOOT_BOARD_NAME_SZ - 1);
                *((char*)((int)value + HW_BOOT_BOARD_NAME_SZ)) = 0;
            }
            strcpy((char*)epc.hw_boot.board_name, value);
            SerialStd.print(F("Board Name:"));
            SerialStd.println((char*)epc.hw_boot.board_name);

        } else if (strcmp_P(name, BOARD_SN_pm) == 0) {
            // FUT needs to be checked for sz
            if (strlen(value) >= (HW_BOOT_SERIAL_NUM_SZ - 1)) {
                MS_DBG(F("Too long limited to "), HW_BOOT_SERIAL_NUM_SZ - 1);
                *((char*)((int)value + HW_BOOT_SERIAL_NUM_SZ)) = 0;
            }
            strcpy((char*)epc.hw_boot.serial_num, value);
            SerialStd.print(F("Board SerialNum :"));
            SerialStd.println((char*)epc.hw_boot.serial_num);

        } else if (strcmp_P(name, BOARD_REV_pm) == 0) {
            if (strlen(value) >= (HW_BOOT_REV_SZ - 1)) {
                MS_DBG(F("Too long limited to "), HW_BOOT_REV_SZ - 1);
                *((char*)((int)value + HW_BOOT_REV_SZ)) = 0;
            }
            strcpy((char*)epc.hw_boot.rev, value);
            SerialStd.print(F("Board Rev:"));
            SerialStd.println((char*)epc.hw_boot.rev);
#if 0
        } else if (strcmp_P(name,SD_INIT_ID_pm)== 0) {
            if (strlen( value) >= (HW_BOOT_SD_BOOT_INI_SZ-1) ) {
                MS_DBG(F("Too long limited to "),HW_BOOT_SD_BOOT_INI_SZ-1); 
                *((char*) ((int)value+HW_BOOT_SD_BOOT_INI_SZ) ) =0;
            }
            strcpy((char *)epc.hw_boot.sd_boot_ini, value);
            SerialStd.print(F("SD BOOT ini:"));
            SerialStd.println((char *)epc.hw_boot.sd_boot_ini);
#endif
        } else if (strcmp_P(name, EEPROM_WRITE_pm) == 0) {
            if (strcmp_P(value, YES_pm) == 0) {
                SerialStd.println(F("EEPROM Write started:"));

                // uint8_t crc8=Dallas_crc8((const uint8_t
                // *)((int)&epc.hw_boot+1),((uint8_t)sizeof_hw_boot)-1 );
                // epc.hw_boot.crc16=crc8;
                uint16_t crc16 = calc_crc16(
                    (const uint8_t*)((int)&epc.hw_boot + SIZE_UINT16_CRC16),
                    ((uint16_t)sizeof(epc.hw_boot) - SIZE_UINT16_CRC16));
                epc.hw_boot.crc16 = crc16;
                EEPROM.put(EP_HW_BOOT_ADDR, epc.hw_boot);
                MS_DBG(F("hw_boot wr"), sizeof_hw_boot, F("crc16="),
                       epc.hw_boot.crc16, F("sn="),
                       (char*)epc.hw_boot.serial_num, F("rev="),
                       (char*)epc.hw_boot.rev, F("name="),
                       (char*)epc.hw_boot.board_name
                       // F("ini="),(char *)epc.hw_boot.sd_boot_ini
                );
                SerialStd.println(F("EEPROM Write finished"));
            } else if (strcmp_P(value, DEFAULT_APP_EEPROM_pm) == 0) {
                SerialStd.println(F("ACTION DEFAULT app EEPROM"));
                localAppInit(); 
                localEepromAppWr("Default");                
            } else {
                SerialStd.print(F("EEPROM Write aborted. instruction '"));
                SerialStd.print(value);
                SerialStd.println("'");
            }
            // SerialStd.println(mcuBoardVersion);    
        } else {
            SerialStd.print(F("BOOT tbd "));
            SerialStd.print(name);
            SerialStd.print(F(" to "));
            SerialStd.println(value);
        }
#endif  // USE_PS_HW_BOOT
    } else if (strcmp_P(section, USER_pm) == 0) {
        if (strcmp_P(name, ACTION_pm) == 0) {
#if defined USE_PS_EEPROM
            if (strcmp_P(value, WRITE_pm) == 0) {
                localEepromAppWr("User"); 
            } else
#endif  // USE_PS_EEPROM

                if (strcmp_P(value, COPY_pm) == 0) {
                SerialStd.println(F("ACTION COPY not supported yet:"));

            } else {
                SerialStd.print(F("ACTION Write aborted. instruction '"));
                SerialStd.print(value);
                SerialStd.println("'");
            }
        }

    } else {
        SerialStd.print(F("Not supported ["));
        SerialStd.print(section);
        SerialStd.println(F("] "));
        SerialStd.print(name);
        SerialStd.print(F("="));
        SerialStd.println(value);
    }
#if RAM_REPORT_LEVEL > 1
    if (ram_track) RAM_AVAILABLE;
#endif  // RAM_REPORT_LEVEL
    return 1;
}
#endif  // USE_MS_SD_INI

#if 0 
//void ledflash(uint8_t numFlash = 4, unsigned long onTime_ms = 75,unsigned long offTime_ms = 150)
//Shouldn't be here
void ledflash(uint8_t numFlash, unsigned long onTime_ms,unsigned long offTime_ms)
{
    for (uint8_t i = 0; i < numFlash; i++) {
        setGreenLED( HIGH);
        delay(onTime_ms);
        setGreenLED( LOW);
        delay(offTime_ms);
    }
}
#endif

#if defined USE_PS_EEPROM
/* ==========================================================================
AVR mega1284 has a 4Kbytes eeprom - electricaly eraseable programable read only
memory This is used to store core provisioning setup, so each processor board
can be customized.

Read the eeprom into the local ram buffer. If crc is invalid then initialize to
defaults. The eeprom has a crc16 in two sections. The epc.hw_boot_t and
epc.app...
*/
void localAppInit() 
{
    MS_DBG(F("  ***Eeprom local app storage init***"));
    epc.app.struct_size = sizeof(epc.app.msc.s);
    epc.app.struct_ver  = 1;
    // Set defaults for [common]
    epc.app.msc.sz                     = sizeof(epc.app.msc.s);
    epc.app.msc.s.logging_interval_min = loggingInterval_CDEF_MIN;
    epc.app.msc.s.time_zone            = CONFIG_TIME_ZONE_DEF;
    epc.app.msc.s.battery_type         = PLSR_BAT_TYPE_DEF;
    strcpy_P((char*)epc.app.msc.s.logger_id, (char*)F(LOGGERID_DEF_STR));
    strcpy_P((char*)epc.app.msc.s.geolocation_id,
                (char*)F("Factory default"));

    epc.app.msn.s.network_type= MSCN_TYPE_NONE;
    strcpy_P((char*)epc.app.msn.s.apn,(char*)F(MSCN_APN_DEF_STR));
    strcpy_P((char*)epc.app.msn.s.WiFiId,(char*)F(MSCN_WIFIID_DEF_STR));  
    strcpy_P((char*)epc.app.msn.s.WiFiPwd,(char*)F(MSCN_WIFIPWD_DEF_STR)); 
    epc.app.msn.s.collectReadings_num =MNGI_COLLECT_READINGS_DEF;
    epc.app.msn.s.sendOffset_min = MNGI_SEND_OFFSET_MIN_DEF;


    epc.app.provider.provider_type=  UUIDE_TYPE_MMW;
    strcpy_P((char*)epc.app.provider.s.ed.cloudId, (char*) F(UUIDE_DEF_STR));
    strcpy_P((char*)epc.app.provider.s.ed.registration_token, (char*)F(UUIDE_DEF_STR));
    strcpy_P((char*)epc.app.provider.s.ed.sampling_feature, (char*)F(UUIDE_DEF_STR));
    epc.app.provider.s.ed.timerPostTout_ms = MMW_TIMER_POST_TIMEOUT_MS_DEF;
    epc.app.provider.s.ed.timerPostPace_ms = MMW_TIMER_POST_PACING_MS_DEF; 
    epc.app.provider.s.ed.postMax_num = MMW_TIMER_POST_MAX_MUM_DEF;  
    for (int uuid_lp=0;uuid_lp <UUIDE_SENSOR_CNT_MAX_SZ;uuid_lp++) 
    {
        //strcpy_P(epc.app.provider.s.ed.uuid[uuid_lp],(char*)F(UUIDE_NULL_STR ));
        epc.app.provider.s.ed.uuid[uuid_lp].name[0] = UUIDE_NULL_TERMINATOR;
        epc.app.provider.s.ed.uuid[uuid_lp].value[0] = UUIDE_NULL_TERMINATOR;
    }
} // localAppInit()

void readAvrEeprom() {
    uint16_t crc16;
    // Read EEPROM in sections, and verify CRC

    MS_DBG(F("Eeprom  size="), EEPROM.length(), "boot=", sizeof_hw_boot,
           "app=", sizeof(epc.app));

    // Read Eeprom [Boot] - every board should have it
    EEPROM.get(EP_HW_BOOT_ADDR, epc.hw_boot);
    crc16 = calc_crc16((const uint8_t*)((int)&epc.hw_boot + SIZE_UINT16_CRC16),
                       ((uint16_t)sizeof(epc.hw_boot) - SIZE_UINT16_CRC16));

    MS_DBG(F("Eeprom  boot "), F("crc16calc"), crc16, F("crc16read"),
           epc.hw_boot.crc16);
    if ((crc16 != epc.hw_boot.crc16) && (crc16 != 0)) {
        // Boot is invalid, therefore init. assume whole structure invalid
        MS_DBG(F("  ***Eeprom hw_boot Invalid, loocal ram init"));
#define EEPROM_DEFAULT_STATE 0xFF
        memset(&epc.hw_boot, EEPROM_DEFAULT_STATE, sizeof(epc.hw_boot));
        epc.hw_boot.struct_ver = HW_BOOT_STRUCT_VER;
        strcpy_P((char*)epc.hw_boot.board_name,
                 (const char*)F("enviroDIY mayfly"));
        strcpy_P((char*)epc.hw_boot.serial_num,
                 (const char*)F("M123456789abcdefgh"));
        strcpy_P((char*)epc.hw_boot.rev, (const char*)F("0.5b"));
        // strcpy_P((char *)epc.hw_boot.sd_boot_ini,(const char
        // *)F("ms_cfg.ini"));
        MS_DBG(F("  ***local hw_boot init end***"));
    }

    PRINTOUT(F("From eeprom boot Name["), (char*)epc.hw_boot.board_name,
             F("]  rev["), (char*)epc.hw_boot.rev, F("] sn["),
             (char*)epc.hw_boot.serial_num, F("]"));

    MS_DBG(F("hw_boot ver"), epc.hw_boot.struct_ver);

    EEPROM.get(EP_PERSISTENT_STORE_ADDR, epc.app);
    crc16 = calc_crc16((const uint8_t*)((int)&epc.app + SIZE_UINT16_CRC16),
                       (sizeof(epc.app) - SIZE_UINT16_CRC16));
    MS_DBG(F("App crc16calc"), crc16, F("crc16read"), epc.app.crc16);
    if ((crc16 != epc.app.crc16) && (crc16 != 0)) 
    {
        localAppInit();
    }

    // Read Eeprom Common .app.msc.s that maps from .ini [COMMON]
    PRINTOUT(F("From eeprom common: Logger File Name["),
             (char*)epc.app.msc.s.logger_id, F("]\n\r   logging interval="),
             epc.app.msc.s.logging_interval_min, F("minutes, Tz="),
             epc.app.msc.s.time_zone, F("battery type="),
             epc.app.msc.s.battery_type);
    PRINTOUT(F("   Loc="), (char*)epc.app.msc.s.geolocation_id);

    MS_DBG(F("Common: sz="), epc.app.msc.sz);


    // read EEPROM Network app.msn.s that maps from .ini [NETWORK] 
    PRINTOUT(F("From eeprom Network: Network Type="),epc.app.msn.s.network_type,
            F("\n APN="),(char*)epc.app.msn.s.apn, 
            F("\n WiFiId="),(char*)epc.app.msn.s.WiFiId,
            F(" WiFiPwd="),(char*)epc.app.msn.s.WiFiPwd,
            F("\n\r CollectReadings="), epc.app.msn.s.collectReadings_num,
            F(" SendOffset(min)="), epc.app.msn.s.sendOffset_min
            );


    // read EEPROM provider app.provider.s that maps from .ini [PROVIDER] & UUIDs
    PRINTOUT(F("From eeprom Provider: Provider Type="),epc.app.provider.provider_type,
            F("\n PROVIDER_MMW CloudId="),(char*)epc.app.provider.s.ed.cloudId, 
            F("\n PROVIDER_MMW Reg Token="),(char*)epc.app.provider.s.ed.registration_token,
            F("\n PROVIDER_MMW Sampling Feature="),(char*)epc.app.provider.s.ed.sampling_feature,
            F("\n PROVIDER_MMW timerPostTout(ms)="), epc.app.provider.s.ed.timerPostTout_ms,
            F("\n PROVIDER_MMW timerPostPace(ms)="), epc.app.provider.s.ed.timerPostPace_ms,
            F("\n postMax="), epc.app.provider.s.ed.postMax_num
            );
    PRINTOUT(F("PROVIDER_MMW UUIDs (none unless listed), Max="),UUIDE_SENSOR_CNT_MAX_SZ);
    for (int uuid_lp=0;uuid_lp <UUIDE_SENSOR_CNT_MAX_SZ;uuid_lp++) {
        //if (UUIDE_NULL_TERMINATOR != epc.app.provider.s.ed.uuid[uuid_lp][0]) {
        if (isalnum(epc.app.provider.s.ed.uuid[uuid_lp].value[0]) 
        || isalnum(epc.app.provider.s.ed.uuid[uuid_lp].name[0])) {
            PRINTOUT(uuid_lp,F("]"),(char *)epc.app.provider.s.ed.uuid[uuid_lp].name,
            F("="),(char *)epc.app.provider.s.ed.uuid[uuid_lp].value);
        }

    }   

} // readAvrEeprom
#endif  // USE_PS_EEPROM

// Decode reason for this Reset
#if !defined ARDUINO_ARCH_AVR
#include <sam.h>
#define NUM_RESET_BITS 8
const char* rrReason[NUM_RESET_BITS] = {"POR ", "BOD12 ", "BOD33 ", "NVM ",
                                        "EXT ", "WDT ",   "SYST ",  "Backup "};
String      decodeResetCause(uint8_t resetCause) {
    char    resetReason[60];
    uint8_t rrLen  = 0;
    int8_t  rrLp   = (NUM_RESET_BITS - 1);
    resetReason[0] = 0;
    // resetCause =0xff;
    for (; 0 <= rrLp; --rrLp) {
        if ((0x1 << rrLp) & resetCause)
            strcpy(&resetReason[rrLen], rrReason[rrLp]);
        rrLen = strlen(resetReason);
    }
    // MS_DBG("tot str size",rrLen);
    return (String)resetReason;
}
#endif  // ARDUINO_ARCH_AVR
