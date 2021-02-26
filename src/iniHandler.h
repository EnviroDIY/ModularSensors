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
const int  TIMER_POST_TOUT_MS_MAX=30000 ;
const int  TIMER_POST_TOUT_MS_MIN=100 ;

const char TIMER_POST_PACE_MS_pm[] EDIY_PROGMEM = "TIMER_POST_PACE_MS";
const int  TIMER_POST_PACE_MS_MAX=5000 ;
const int  TIMER_POST_PACE_MS_MIN=0 ;

const char POST_MAX_NUM_pm[] EDIY_PROGMEM       = "POST_MAX_NUM";
const int  POST_RANGE_MAX_NUM=500;
const int  POST_RANGE_MIN_NUM=0;

const char PROVIDER_TS_pm[] EDIY_PROGMEM           = "PROVIDER_TS";
//KEY STRINGS 
const char TS_MQTT_KEY_pm[] EDIY_PROGMEM      = "TS_MQTT_KEY";
const char TS_CHANNEL_ID_pm[] EDIY_PROGMEM    = "TS_CHANNEL_ID";
const char TS_CHANNELWR_KEY_pm[] EDIY_PROGMEM   = "TS_CHANNELWR_KEY";

const char PROVIDER_UBIDOTS_pm[] EDIY_PROGMEM       = "PROVIDER_UBIDOTS";
//KEY_STRINGS
const char UB_AUTH_TOKEN_pm[] EDIY_PROGMEM = "UB_AUTH_TOKEN";
const char UB_DEVICE_ID_pm[] EDIY_PROGMEM   = "UB_DEVICE_ID";

const char SENSORS_pm[] EDIY_PROGMEM = "SENSORS";
const char index_pm[] EDIY_PROGMEM   = "index";

const char USER_pm[] EDIY_PROGMEM   = "USER";
const char ACTION_pm[] EDIY_PROGMEM = "ACTION";
const char WRITE_pm[] EDIY_PROGMEM  = "WRITE";
const char DEFAULT_APP_EEPROM_pm[] EDIY_PROGMEM  = "DEFAULT_APP_EEPROM";
const char COPY_pm[] EDIY_PROGMEM   = "COPY";

//Ensure the following are initialized before new search
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
void localAppStorageInit();

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

/* Parse the pesistent UUIDs
 * ~ instate them in classes where needed
 *  ~ Searh for name:value pairs that match a UUD that have not already been found
*/
static void populateUuidMatchEpc(ini_name_value_t *uuidTable) 
{
    int8_t uuid_vl_idx = 0;
    int epc_idx=0;
    ini_name_value_t *dest2pi;;
    int idx_break=0;

    //Check for any unassigned NAME:VALUE
    do {
        //Point to active record
        dest2pi = (ini_name_value_t *)((int)uuidTable+sizeof(ini_name_value_t)*epc_idx);
        if (isalnum(dest2pi->value[0]) 
        && isalnum(dest2pi->name[0])) 
        {
            //Found alpha values in table so valid, search for a match with internal variables
            MS_DEEP_DBG(F("pume search"),dest2pi->name,epc_idx, F("?"),uuid_vl_idx,
            (const char*)variableList[uuid_vl_idx]->getVarUUID().c_str()  );
            if (strcmp((const char*)variableList[uuid_vl_idx]
                        ->getVarUUID().c_str(),
                        dest2pi->name) == 0) 
            {  // Found a match
                variableList[uuid_vl_idx]->setVarUUID_atl((char*)dest2pi->value, false);
                PRINTOUT(F("PROVIDER_xx"),dest2pi->name,dest2pi->value,F("->"),
                 variableList[uuid_vl_idx]->getVarCode());
                epc_idx++;
                uuid_vl_idx = -1; ///Reset to start at begining of variable_list
            }
            #ifdef MS_TU_CTD_DEBUG
            delay(100); //Don;t overpower debug output.
            #endif
        } else {
            MS_DBG(F("search !isalnum epc["),epc_idx, F("] "),
            isalnum(dest2pi->value[0]),
            isalnum(dest2pi->name[0]) ) ;
        }
        uuid_vl_idx++;
        if (uuid_vl_idx>=variableCount) { 
            epc_idx++;
            uuid_vl_idx = 0;
        }
        if (++idx_break > (PROVID_UUID_SENSOR_CNTMAX_SZ*PROVID_UUID_SENSOR_CNTMAX_SZ)) {
            PRINTOUT(F("Search error! break out of loop"));
            break;
        }
    }while (epc_idx < PROVID_UUID_SENSOR_CNTMAX_SZ );
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
    

    #if defined USE_PUB_MMW
    //[Provider_MMW]
    //CLOUD_ID=data.enviroDIY.com - hard coded
    PRINTOUT(F("PROVIDER_MMW CloudId"),             epc.app.provider.s.ed.cloudId);
    PRINTOUT(F("PROVIDER_MMW Registration Token"),  epc.app.provider.s.ed.registration_token);
    PRINTOUT(F("PROVIDER_MMW Samping Feature"),     epc.app.provider.s.ed.sampling_feature);
    PRINTOUT(F("PROVIDER_MMW timerPost Tout(ms)"),  epc.app.provider.s.ed.timerPostTout_ms);
    PRINTOUT(F("PROVIDER_MMW timerPost Pacing(ms)"),epc.app.provider.s.ed.timerPostPace_ms);
    //POST_MAX_NUM=0; Fut Not used
    populateUuidMatchEpc(epc.app.provider.s.ed.uuid);
    #endif //USE_PUB_MMW

    #if defined USE_PUB_TSMQTT 
    #warning "Need Eeprom:TSMQTT"   
    PRINTOUT(F("PROVIDER_TS EPROM fut\n\r CloudId"),epc.app.provider.s.ts.cloudId);
    PRINTOUT(F("PROVIDER_TS MQTT Key"),             epc.app.provider.s.ts.thingSpeakMQTTKey );
    PRINTOUT(F("PROVIDER_TS Channel ID"),           epc.app.provider.s.ts.thingSpeakChannelID);
    PRINTOUT(F("PROVIDER_TS Channel Wr Key"),       epc.app.provider.s.ts.thingSpeakChannelKey);
    PRINTOUT(F("PROVIDER_TS timerPost Tout(ms)"),   epc.app.provider.s.ed.timerPostTout_ms);
    PRINTOUT(F("PROVIDER_TS timerPost Pacing(ms)"), epc.app.provider.s.ed.timerPostPace_ms);
    //POST_MAX_NUM=0; Fut Not used    
    #endif // USE_PUB_TSMQTT 

    #if defined USE_PUB_UBIDOTS
    PRINTOUT(F("PROVIDER_UB"),F("EPROM fut\n\r CloudId"), epc.app.provider.s.ub.cloudId);
    PRINTOUT(F("PROVIDER_UB"),F("Auth Token"),          epc.app.provider.s.ub.authentificationToken);
    PRINTOUT(F("PROVIDER_UB"),F("DeviceId"),            epc.app.provider.s.ub.deviceID);
    PRINTOUT(F("PROVIDER_UB"),F("timerPost Tout(ms)"),  epc.app.provider.s.ub.timerPostTout_ms);
    PRINTOUT(F("PROVIDER_UB"),F("timerPost Pacing(ms)"),epc.app.provider.s.ub.timerPostPace_ms);
    //POST_MAX_NUM=0; Fut Not used
    populateUuidMatchEpc(epc.app.provider.s.ub.uuid);
    #endif // USE_PUB_UBIDOTS

}

/* A short string size checker*/
#define eCpy(parm1,parm2) chkLen(parm1,value,parm2)
static bool chkLen(char *destStr, const char *value,int size) {
    int value_len = strlen(value);
    bool ret_val;
    if (value_len > size-1) {
        PRINTOUT(F("ERROR ini param too long, got"),value_len, F("expect"),size,F("for"),value );
        ret_val= false;
    } else {
        strcpy(destStr,value );
    }
    return ret_val;
}

static void populateUuidMatchIni(const char* name, const char* value,
                           ini_name_value_t *uuidTable) 
{

    /* UUIDs are applied to internal sensor Array as follows:
    1) "UUID_label"="UUID"
    eg ASQ212_PAR="UUID"
    search variableList for UUID_label and if found replace with "UUID"
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

            //#define uuidde_name(uuid_idx)  (char*)epc.app.provider.s.ed.uuid[uuid_idx].name
            //char *dest1p = uuidub_name(uuid_ram_idx);
            ini_name_value_t *dest2pi = (ini_name_value_t *)((int)uuidTable+sizeof(ini_name_value_t)*uuid_ram_idx);
            //char *dest2p = (char *)(dest2pi->name); //+(uuid_ram_idx*PROVID_UUID_SENSOR_NAME_SZ)
            strcpy(dest2pi->name,name);

            //#define uuidde_value(uuid_idx) (char*)epc.app.provider.s.ed.uuid[uuid_idx].value
            //dest1p = uuidub_value(uuid_ram_idx);
            //dest2p = (char *)(dest2pi->value);//(uuid_ram_idx*PROVID_UUID_SENSOR_VALUE_SZ)

            strcpy(dest2pi->value,value);

            uuid_search_i = variableCount;
            uuid_ram_idx++;
        }
        uuid_search_i++;
    } while (uuid_search_i < variableCount);

    if (uuid_search_i > variableCount) {
        SerialStd.println(F("} match  & added."));
    } else {
        SerialStd.println(F("} not supported"));
    }
    uuid_index++;

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
    if (strcmp_P(section, PROVIDER_MMW_pm) == 0) 
    {
    #if defined USE_PUB_MMW
        //Process [PROVIDER_MMW] only defined for EnviroDIY 
        if (strcmp_P(name, CLOUD_ID_pm) == 0) {
            strcpy(epc.app.provider.s.ed.cloudId, value);
            MS_DBG(F("PROVIDER_MMW Setting cloudId: "),
            epc.app.provider.s.ed.cloudId);
        } else if (strcmp_P(name, REGISTRATION_TOKEN_pm) == 0) {
            strcpy(epc.app.provider.s.ed.registration_token, value);
            MS_DBG(F("PROVIDER_MMW Setting registration token: "),
            epc.app.provider.s.ed.registration_token);
        } else if (strcmp_P(name, SAMPLING_FEATURE_pm) == 0) {
            strcpy(epc.app.provider.s.ed.sampling_feature, value);
            MS_DBG(F("PROVIDER_MMW Setting SamplingFeature: "),
            epc.app.provider.s.ed.sampling_feature);
        }else if (strcmp_P(name, TIMER_POST_TOUT_MS_pm) == 0) {
            // convert  str to num with error checking
            long timerPostTimeout_local = strtol(value, &endptr, 10);
            if ((timerPostTimeout_local <= TIMER_POST_TOUT_MS_MAX) &&
                (timerPostTimeout_local >= TIMER_POST_TOUT_MS_MIN) && (errno != ERANGE)) 
            {
                //Null, default below
            } else {
                PRINTOUT(F("PROVIDER_MMW Set TIMER_POST_TOUT_MS error; (range ["),
                TIMER_POST_TOUT_MS_MIN, TIMER_POST_TOUT_MS_MAX,
                                  F("] read:"),timerPostTimeout_local);
                timerPostTimeout_local =MMW_TIMER_POST_TIMEOUT_MS_DEF;
            }
            epc.app.provider.s.ed.timerPostTout_ms = timerPostTimeout_local;
            MS_DBG(F("PROVIDER_MMW Set TIMER_POST_TOUT_MS : "),timerPostTimeout_local);

        } else  if (strcmp_P(name, TIMER_POST_PACE_MS_pm) == 0) {
            // convert  str to num with error checking
            long timerPostPacing_local = strtol(value, &endptr, 10);
            if ((timerPostPacing_local <= TIMER_POST_PACE_MS_MAX) &&
                (timerPostPacing_local >= TIMER_POST_PACE_MS_MIN) && (errno != ERANGE)) 
            {
                //Null, default below
            } else {
                PRINTOUT(F("PROVIDER_MMW Set TIMER_POST_PACE_MS error; (range "),
                                TIMER_POST_PACE_MS_MIN,TIMER_POST_PACE_MS_MAX,
                                F("] read:"),timerPostPacing_local);
                timerPostPacing_local= MMW_TIMER_POST_PACING_MS_DEF;
            }
            epc.app.provider.s.ed.timerPostPace_ms = timerPostPacing_local;
            MS_DBG(F("PROVIDER_MMW Set TIMER_POST_PACE_MS: "),timerPostPacing_local);

        } else if (strcmp_P(name, POST_MAX_NUM_pm) == 0) {
            // convert  str to num with error checking
            long postMax_num_local = strtol(value, &endptr, 10);
            if ((postMax_num_local <= POST_RANGE_MAX_NUM) && (postMax_num_local >= POST_RANGE_MIN_NUM) &&
                (errno != ERANGE)) 
            {
                //Null, default below
            } else {
                PRINTOUT(
                    F("PROVIDER_MMW Set POST_MAX_NUM error; range["), 
                    POST_RANGE_MIN_NUM,POST_RANGE_MAX_NUM,F("] read:"),postMax_num_local);
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
            */
            populateUuidMatchIni( name,  value, ps_ram.app.provider.s.ed.uuid);
         }
    #endif //USE_PUB_MMW
    } else if (strcmp_P(section, PROVIDER_TS_pm) == 0) 
    {
    #if defined USE_PUB_TSMQTT 
        //Process [PROVIDER_TS] only defined for thingSpeak 
        if (strcmp_P(name, CLOUD_ID_pm) == 0) {
            eCpy(epc.app.provider.s.ts.cloudId, PROVID_CLOUD_ID_SZ);
            MS_DBG(F("PROVIDER_TS Setting cloudId: "),
            epc.app.provider.s.ts.cloudId);
        } else if (strcmp_P(name, TS_MQTT_KEY_pm) == 0) {
            eCpy(epc.app.provider.s.ts.thingSpeakMQTTKey, PROVID_TSMQTTKEY_SZ);
            MS_DBG(F("PROVIDER_TS Setting TS_MQTT_KEY: "),
            epc.app.provider.s.ts.thingSpeakMQTTKey);
        } else if (strcmp_P(name, TS_CHANNEL_ID_pm) == 0) {
            eCpy(epc.app.provider.s.ts.thingSpeakChannelID, PROVID_TSCHANNELID_SZ);
            MS_DBG(F("PROVIDER_TS Setting TS_CHANNEL_ID: "),
            epc.app.provider.s.ts.thingSpeakChannelID);
        } else if (strcmp_P(name, TS_CHANNELWR_KEY_pm) == 0) {
            eCpy(epc.app.provider.s.ts.thingSpeakChannelKey, PROVID_TSCHANNELKEY_SZ);
            MS_DBG(F("PROVIDER_TS Setting TS_CHANNELWR_KEY: "),
            epc.app.provider.s.ts.thingSpeakChannelKey);
        } else if (strcmp_P(name, TIMER_POST_TOUT_MS_pm) == 0) {
            // convert  str to num with error checking
            long timerPostTimeout_local = strtol(value, &endptr, 10);
            if ((timerPostTimeout_local <= TIMER_POST_TOUT_MS_MAX) &&
                (timerPostTimeout_local >= TIMER_POST_TOUT_MS_MIN) && (errno != ERANGE)) 
            {
                //Null, default below
            } else {
                PRINTOUT(F("PROVIDER_TS Set TIMER_POST_TOUT_MS error; range["),
                                  TIMER_POST_TOUT_MS_MIN,TIMER_POST_TOUT_MS_MAX, 
                                  F("] read:"),timerPostTimeout_local);
                timerPostTimeout_local =MMW_TIMER_POST_TIMEOUT_MS_DEF;
            }
            epc.app.provider.s.ts.timerPostTout_ms = timerPostTimeout_local;
            MS_DBG(F("PROVIDER_TS Set TIMER_POST_TOUT_MS : "),timerPostTimeout_local);

        } else  if (strcmp_P(name, TIMER_POST_PACE_MS_pm) == 0) {
            // convert  str to num with error checking
            long timerPostPacing_local = strtol(value, &endptr, 10);
            if ((timerPostPacing_local <= TIMER_POST_PACE_MS_MAX) &&
                (timerPostPacing_local >= TIMER_POST_PACE_MS_MIN) && (errno != ERANGE)) 
            {
                //Null, default below
            } else {
                PRINTOUT(F("PROVIDE_TS Set TIMER_POST_PACE_MS error; (range "),
                                  TIMER_POST_PACE_MS_MIN,TIMER_POST_PACE_MS_MAX,
                                  F(" read:"),timerPostPacing_local);
                timerPostPacing_local= MMW_TIMER_POST_PACING_MS_DEF;
            }
            epc.app.provider.s.ts.timerPostPace_ms = timerPostPacing_local;
            MS_DBG(F("PROVIDE_TS Set TIMER_POST_PACE_MS: "),timerPostPacing_local);

        } else if (strcmp_P(name, POST_MAX_NUM_pm) == 0) {
            // convert  str to num with error checking
            long postMax_num_local = strtol(value, &endptr, 10);
            if ((postMax_num_local <= POST_RANGE_MAX_NUM) && (postMax_num_local >= TIMER_POST_PACE_MS_MIN) &&
                (errno != ERANGE)) 
            {
                //Null, default below
            } else {
                PRINTOUT(
                    F("PROVIDER_TS Set POST_MAX_NUM error; ["),
                    TIMER_POST_PACE_MS_MIN,POST_RANGE_MAX_NUM,F("] read:"),postMax_num_local);
                    postMax_num_local=MMW_TIMER_POST_MAX_MUM_DEF;
            }
            //postMax_num = (uint8_t)postMax_num_local;
            epc.app.provider.s.ts.postMax_num = postMax_num_local;
            MS_DBG(F("PROVIDER_TS Set POST_MAX_NUM: "),postMax_num_local);

        }
    #endif // USE_PUB_TSMQTT 
    } else if (strcmp_P(section, PROVIDER_UBIDOTS_pm) == 0) 
    {
    #if defined USE_PUB_UBIDOTS
        //Process [PROVIDER_UBIDOTS] only defined for ubidots.com 
        if (strcmp_P(name, CLOUD_ID_pm) == 0) {
            strcpy(epc.app.provider.s.ub.cloudId, value);
            MS_DBG(F("PROVIDER_UBIDOTS Setting cloudId: "),
            epc.app.provider.s.ub.cloudId);
        } else if (strcmp_P(name, UB_AUTH_TOKEN_pm) == 0) {
            strcpy(epc.app.provider.s.ub.authentificationToken, value);
            MS_DBG(F("PROVIDER_UBIDOTS Setting registration token: "),
            epc.app.provider.s.ub.authentificationToken);
            uuid_ram_idx=0;
            uuid_index=0;        
        } else if (strcmp_P(name, UB_DEVICE_ID_pm) == 0) {
            strcpy(epc.app.provider.s.ub.deviceID, value);
            MS_DBG(F("PROVIDER_UBIDOTS Setting SamplingFeature: "),
            epc.app.provider.s.ub.deviceID);
        }else if (strcmp_P(name, TIMER_POST_TOUT_MS_pm) == 0) {
            // convert  str to num with error checking
            long timerPostTimeout_local = strtol(value, &endptr, 10);
            if ((timerPostTimeout_local <= TIMER_POST_TOUT_MS_MAX) &&
                (timerPostTimeout_local >= TIMER_POST_TOUT_MS_MIN) && (errno != ERANGE)) 
            {
                //Null, default below
            } else {
                //PRINTOUT(PROVIDER_UBIDOTS_pm,F("Set TIMER_POST_TOUT_MS error; range["),
                PRINTOUT(F("PROVIDER_UBIDOTS"),F("Set TIMER_POST_TOUT_MS error; range["),
                TIMER_POST_TOUT_MS_MIN,TIMER_POST_TOUT_MS_MAX,
                                  F("] read:"),timerPostTimeout_local);
                timerPostTimeout_local =MMW_TIMER_POST_TIMEOUT_MS_DEF;
            }
            epc.app.provider.s.ub.timerPostTout_ms = timerPostTimeout_local;
            MS_DBG(F("PROVIDER_UBIDOTS Set TIMER_POST_TOUT_MS : "),timerPostTimeout_local);

        } else  if (strcmp_P(name, TIMER_POST_PACE_MS_pm) == 0) {
            // convert  str to num with error checking
            long timerPostPacing_local = strtol(value, &endptr, 10);
            if ((timerPostPacing_local <= TIMER_POST_PACE_MS_MAX) &&
                (timerPostPacing_local >= TIMER_POST_PACE_MS_MIN) && (errno != ERANGE)) 
            {
                //Null, default below
            } else {
                PRINTOUT(F("PROVIDER_UBIDOTS"),F("Set TIMER_POST_PACE_MS error; (range "),
                                  TIMER_POST_PACE_MS_MIN,TIMER_POST_PACE_MS_MAX,F("] read:"),timerPostPacing_local);
                timerPostPacing_local= MMW_TIMER_POST_PACING_MS_DEF;
            }
            epc.app.provider.s.ub.timerPostPace_ms = timerPostPacing_local;
            MS_DBG(F("PROVIDR_UBIDOTS Set TIMER_POST_PACE_MS: "),timerPostPacing_local);

        } else if (strcmp_P(name, POST_MAX_NUM_pm) == 0) {
            // convert  str to num with error checking
            long postMax_num_local = strtol(value, &endptr, 10);
            if ((postMax_num_local <= POST_RANGE_MAX_NUM) && (postMax_num_local >= POST_RANGE_MIN_NUM) &&
                (errno != ERANGE)) 
            {
                //Null, default below
            } else {
                PRINTOUT(
                    F("PROVIDER_UBIDOTS Set POST_MAX_NUM error; (range"),
                    POST_RANGE_MIN_NUM,POST_RANGE_MAX_NUM,F("] read:"),postMax_num_local);
                    postMax_num_local=MMW_TIMER_POST_MAX_MUM_DEF;
            }
            //postMax_num = (uint8_t)postMax_num_local;
            epc.app.provider.s.ub.postMax_num = postMax_num_local;
            MS_DBG(F("PROVIDER_UBIDOTS Set POST_MAX_NUM: "),postMax_num_local);
        } else {

            /* UBIDOTS UUIDs label are uniquuel used to identify the ubidots "variable" are applied to internal sensor Array as follows:
            1) From ms_cfg.ini: "UUID_label"="ubiddotsVariable"
            eg Air_Humidity_UUID=humd
            search variableList for UUID_label and if found replace with "ubidotsVariable"
            */
            populateUuidMatchIni( name,  value, epc.app.provider.s.ub.uuid);

        }
    #endif // USE_PUB_UBIDOTS

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
                localAppStorageInit(); 
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

For other processors its expected they will have a local fixed QSPI/disk
and this will be a persisten storage

Read the eeprom into the local ram buffer. If crc is invalid then initialize to
defaults. The eeprom has a crc16 in two sections. The epc.hw_boot_t and
epc.app...
*/
void localAppStorageInit() 
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


    epc.app.provider.provider_type=  PROVID_TYPE_MMW;
    strcpy_P((char*)epc.app.provider.s.ed.cloudId, (char*) F(PROVID_DEF_STR));
    strcpy_P((char*)epc.app.provider.s.ed.registration_token, (char*)F(PROVID_DEF_STR));
    strcpy_P((char*)epc.app.provider.s.ed.sampling_feature, (char*)F(PROVID_DEF_STR));
    epc.app.provider.s.ed.timerPostTout_ms = MMW_TIMER_POST_TIMEOUT_MS_DEF;
    epc.app.provider.s.ed.timerPostPace_ms = MMW_TIMER_POST_PACING_MS_DEF; 
    epc.app.provider.s.ed.postMax_num = MMW_TIMER_POST_MAX_MUM_DEF;  
    for (int uuid_lp=0;uuid_lp <PROVID_UUID_SENSOR_CNTMAX_SZ;uuid_lp++) 
    {
        epc.app.provider.s.ed.uuid[uuid_lp].name[0] = PROVID_NULL_TERMINATOR;
        epc.app.provider.s.ed.uuid[uuid_lp].value[0] = PROVID_NULL_TERMINATOR;
    }

    //ThingSpeak init
    strcpy_P((char*)epc.app.provider.s.ts.cloudId, (char*) F(PROVID_DEF_STR));
    strcpy_P((char*)epc.app.provider.s.ts.thingSpeakMQTTKey, (char*)F(PROVID_DEF_STR));
    strcpy_P((char*)epc.app.provider.s.ts.thingSpeakChannelID, (char*)F(PROVID_DEF_STR));
    strcpy_P((char*)epc.app.provider.s.ts.thingSpeakChannelKey, (char*)F(PROVID_DEF_STR));
    epc.app.provider.s.ts.timerPostTout_ms = MMW_TIMER_POST_TIMEOUT_MS_DEF;
    epc.app.provider.s.ts.timerPostPace_ms = MMW_TIMER_POST_PACING_MS_DEF; 
    epc.app.provider.s.ts.postMax_num = MMW_TIMER_POST_MAX_MUM_DEF;  

    // UBIDOTS init
    strcpy_P((char*)epc.app.provider.s.ub.cloudId, (char*) F(PROVID_DEF_STR));
    strcpy_P((char*)epc.app.provider.s.ub.authentificationToken, (char*)F(PROVID_DEF_STR));
    strcpy_P((char*)epc.app.provider.s.ub.deviceID, (char*)F(PROVID_DEF_STR));
    epc.app.provider.s.ub.timerPostTout_ms = MMW_TIMER_POST_TIMEOUT_MS_DEF;
    epc.app.provider.s.ub.timerPostPace_ms = MMW_TIMER_POST_PACING_MS_DEF; 
    epc.app.provider.s.ub.postMax_num = MMW_TIMER_POST_MAX_MUM_DEF;  
    for (int uuid_lp=0;uuid_lp <PROVID_UUID_SENSOR_CNTMAX_SZ;uuid_lp++) 
    {
        epc.app.provider.s.ub.uuid[uuid_lp].name[0] = PROVID_NULL_TERMINATOR;
        epc.app.provider.s.ub.uuid[uuid_lp].value[0] = PROVID_NULL_TERMINATOR;
    }
} // localAppStorageInit()

void readAvrEeprom() {
    uint16_t crc16;
    // Read EEPROM in sections, and verify CRC

    MS_DBG(F("Eeprom size="), EEPROM.length(), "boot=", sizeof_hw_boot,
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
        localAppStorageInit();
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

    // List values for PROVIDER_XX 
#if defined USE_PUB_MMW
    // Don't print if not used, but still in eeprom
    PRINTOUT(F("From eeprom Provider: Provider Type="),epc.app.provider.provider_type,
            F(" PROVIDER_MMW CloudId="),(char*)epc.app.provider.s.ed.cloudId, 
            F("\n PROVIDER_MMW Reg Token="),(char*)epc.app.provider.s.ed.registration_token,
            F("\n PROVIDER_MMW Sampling Feature="),(char*)epc.app.provider.s.ed.sampling_feature,
            F("\n PROVIDER_MMW timerPostTout(ms)="), epc.app.provider.s.ed.timerPostTout_ms,
            F("\n PROVIDER_MMW timerPostPace(ms)="), epc.app.provider.s.ed.timerPostPace_ms,
            F("\n PROVIDER_MMW postMax="), epc.app.provider.s.ed.postMax_num
            );
    PRINTOUT(F("  PROVIDER_MMW UUIDs (none unless listed), Max="),PROVID_UUID_SENSOR_CNTMAX_SZ);
    for (int uuid_lp=0;uuid_lp <PROVID_UUID_SENSOR_CNTMAX_SZ;uuid_lp++) {
        //if (PROVID_NULL_TERMINATOR != epc.app.provider.s.ed.uuid[uuid_lp][0]) {
        if (isalnum(epc.app.provider.s.ed.uuid[uuid_lp].value[0]) 
        || isalnum(epc.app.provider.s.ed.uuid[uuid_lp].name[0])) {
            PRINTOUT(uuid_lp,F("]"),(char *)epc.app.provider.s.ed.uuid[uuid_lp].name,
            F("="),(char *)epc.app.provider.s.ed.uuid[uuid_lp].value);
        }
    }   
#endif // USE_PUB_MMW
#if defined USE_PUB_TSMQTT
    PRINTOUT(
            F(" PROVIDER_TS CloudId="),(char*)epc.app.provider.s.ts.cloudId, 
            F("\n PROVIDER_TS TS_MQTT_KEY="),(char*)epc.app.provider.s.ts.thingSpeakMQTTKey,
            F("\n PROVIDER_TS TS_CHANNEL_ID="),(char*)epc.app.provider.s.ts.thingSpeakChannelID,
            F("\n PROVIDER_TS TS_CHANNEL_KEY="),(char*)epc.app.provider.s.ts.thingSpeakChannelID,
            F("\n PROVIDER_TS timerPostTout(ms)="), epc.app.provider.s.ts.timerPostTout_ms,
            F("\n PROVIDER_TS timerPostPace(ms)="), epc.app.provider.s.ts.timerPostPace_ms,
            F("\n PROVIDER_TS postMax="), epc.app.provider.s.ts.postMax_num
            );
#endif //USE_PUB_TSMQTT

#if defined USE_PUB_UBIDOTS
    PRINTOUT(
            F(" PROVIDER_UBIDOTS CloudId="),(char*)epc.app.provider.s.ub.cloudId, 
            F("\n PROVIDER_UBIDOTS Auth Token="),(char*)epc.app.provider.s.ub.authentificationToken,
            F("\n PROVIDER_UBIDOTS Device id="),(char*)epc.app.provider.s.ub.deviceID,
            F("\n PROVIDER_UBIDOTS timerPostTout(ms)="), epc.app.provider.s.ub.timerPostTout_ms,
            F("\n PROVIDER_UBIDOTS timerPostPace(ms)="), epc.app.provider.s.ub.timerPostPace_ms,
            F("\n PROVIDER_UBIDOTS postMax="), epc.app.provider.s.ub.postMax_num
            );
    PRINTOUT(F("  PROVIDER_UBIDOTS Variable mapping  (none unless listed), Max="),PROVID_UUID_SENSOR_CNTMAX_SZ);
    //                PRINTOUT(PROVIDER_UBIDOTS_pm,F("Set TIMER_POST_TOUT_MS error; range["),
    //const char *PROVIDER_UBIDOTS_const =F("PROVIDER_UBIDOTS");
    //PRINTOUT(PROVIDER_UBIDOTS_const,F("Variable mapping  (none unless listed), Max="),PROVID_UUID_SENSOR_CNTMAX_SZ);
    for (int uuid_lp=0;uuid_lp <PROVID_UUID_SENSOR_CNTMAX_SZ;uuid_lp++) {
        //if (PROVID_NULL_TERMINATOR != epc.app.provider.s.ed.uuid[uuid_lp][0]) {
        if (isalnum(epc.app.provider.s.ub.uuid[uuid_lp].value[0]) 
        || isalnum(epc.app.provider.s.ub.uuid[uuid_lp].name[0])) {
            PRINTOUT(uuid_lp,F("]"),(char *)epc.app.provider.s.ub.uuid[uuid_lp].name,
            F("="),(char *)epc.app.provider.s.ub.uuid[uuid_lp].value);
        }
    }   
#endif // USE_PUB_UBIDOTS

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
