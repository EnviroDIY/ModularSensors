// ==========================================================================
// inihUnhandled 
// For any Unhandled sections this is called
// ==========================================================================
#ifdef USE_SD_MAYFLY_INI
//expect to be in near space
  //#define EDIY_PROGMEM PROGMEM
#define mCONST_UNI(p1) const char p1##_pm[] PROGMEM = #p1
const char BOOT_pm[] EDIY_PROGMEM = "BOOT";
const char VER_pm[] EDIY_PROGMEM = "VER";
const char MAYFLY_SN_pm[] EDIY_PROGMEM = "MAYFLY_SN"; 
const char MAYFLY_REV_pm[] EDIY_PROGMEM = "MAYFLY_REV";
const char MAYFLY_INIT_ID_pm[] EDIY_PROGMEM = "MAYFLY_INIT_ID";

const char COMMON_pm[] EDIY_PROGMEM = "COMMON";
const char LOGGER_ID_pm[] EDIY_PROGMEM = "LOGGER_ID";
//mCONST_UNI(LOGGER_ID);// = "nh07k" ;
const char LOGGING_INTERVAL_MINUTES_pm[] EDIY_PROGMEM = "LOGGING_INTERVAL_MINUTES";
const char LOGGING_INTERVAL_MULTIPLIER_pm[] EDIY_PROGMEM = "LOGGING_INTERVAL_MULTIPLIER";
const char LIION_TYPE_pm[] EDIY_PROGMEM = "LIION_TYPE";
const char TIME_ZONE_pm[] EDIY_PROGMEM = "TIME_ZONE";
//FUT const char GEOGRAPHICAL_ID_pm[] EDIY_PROGMEM = "GEOGRAPHICAL_ID";

const char NETWORK_pm[] EDIY_PROGMEM = "NETWORK";
const char apn_pm[] EDIY_PROGMEM = "apn";
const char WiFiId_pm[] EDIY_PROGMEM = "WiFiId";
const char WiFiPwd_pm[] EDIY_PROGMEM = "WiFiPwd";

// INA219 configurable parms
const char INA219M_MA_MULT_pm[] EDIY_PROGMEM = "INA219M_MA_MULT";
const char INA219M_V_THRESHLOW_pm[] EDIY_PROGMEM ="INA219M_V_THRESHLOW"; 

const char PROVIDER_pm[] EDIY_PROGMEM = "PROVIDER";
const char CLOUD_ID_pm[] EDIY_PROGMEM = "CLOUD_ID";
const char REGISTRATION_TOKEN_pm[] EDIY_PROGMEM = "REGISTRATION_TOKEN";
const char SAMPLING_FEATURE_pm[] EDIY_PROGMEM = "SAMPLING_FEATURE";

const char UUIDs_pm[] EDIY_PROGMEM = "UUIDs";
const char SENSORS_pm[] EDIY_PROGMEM = "SENSORS";
const char index_pm[] EDIY_PROGMEM = "index";
static uint8_t uuid_index =0;

#if defined(ARDUINO_AVR_ENVIRODIY_MAYFLY)
#define RAM_AVAILABLE   ramAvailable();
#define RAM_REPORT_LEVEL 1
void ramAvailable(){
    extern int16_t __heap_start, *__brkval;
    uint16_t top_stack = (int) &top_stack  - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
    SerialStd.print(F(" Ram available:"));
    SerialStd.println(top_stack );// Stack and heap ??    
}
#elif defined(ARDUINO_ARCH_SAMD)
extern "C" char *sbrk(int i);
#define RAM_AVAILABLE   ramAvailable();
#define RAM_REPORT_LEVEL 1 
void ramAvailable () {
  char stack_dummy = 0;
  SerialStd.print(F(" Ram available:"));
  SerialStd.println(&stack_dummy - sbrk(0) );// Stack and heap ??  
}
#endif // ARDUINO_AVR_ENVIRODIY_MAYFLY
void ledflash(uint8_t numFlash = 4, unsigned long onTime_ms = 75,unsigned long offTime_ms = 150);
static int inihUnhandledFn( const char* section, const char* name, const char* value)
{
    #if RAM_REPORT_LEVEL > 1
    bool ram_track = true;
    #endif
    //MS_DBG(F("inih "),section," ",name," ",value);
    if (strcmp_P(section,PROVIDER_pm)== 0)
    {
        if        (strcmp_P(name,REGISTRATION_TOKEN_pm)== 0) {
            //TODO: njh move storage to class EnviroDIYPublisher
            strcpy(ps.provider.s.registration_token, value);
            SerialStd.print(F("PROVIDER Setting registration token: "));
            SerialStd.println(ps.provider.s.registration_token );
            //EnviroDIYPOST.setToken(ps.provider.s.registration_token);
        } else if (strcmp_P(name,CLOUD_ID_pm)== 0) {
            //TODO: njh move storage to class EnviroDIYPublisher - though hardcoded
            strcpy(ps.provider.s.cloudId, value);
            SerialStd.print(F("PROVIDER Setting cloudId: "));
            SerialStd.println(ps.provider.s.cloudId );
        } else if (strcmp_P(name,SAMPLING_FEATURE_pm)== 0) {
            //TODO: njh move storage to class EnviroDIYPublisher
            strcpy(ps.provider.s.sampling_feature, value);
            SerialStd.print(F("PROVIDER Setting SamplingFeature: "));
            SerialStd.println(ps.provider.s.sampling_feature );
            //dataLogger.setSamplingFeatureUUID(ps.provider.s.sampling_feature);
        } else {
            SerialStd.print(F("PROVIDER not supported:"));
            SerialStd.print(name);
            SerialStd.print("=");
            SerialStd.println(value);
        }
    } else if (strcmp_P(section,UUIDs_pm)== 0)
    {
        /* UUIDs are applied to internal sensor Array as follows: 
        1) "UUID_label"="UUID"
        eg ASQ212_PAR="UUID"
           search variableList for UUID_label and if found replace with "UUID"
        2) index="UUID"
           if the word "index" is there with a UUID, then the UUID is applied in sequence. 
           Any UUID_label's found also increment the counted 'index'
        */

        uint8_t uuid_search_i=0;
    
        SerialStd.print(F(""));
        SerialStd.print(uuid_index);
        SerialStd.print(":");
        SerialStd.print(name);
        SerialStd.print(F("={"));
        SerialStd.print(value);        
        do {
            if (strcmp((const char *)variableList[uuid_search_i]->getVarUUID().c_str(),name)==0) 
            {//Found a match
                variableList[uuid_search_i]->setVarUUID_atl((char *)value,true);
                uuid_search_i=variableCount;
            }
            uuid_search_i++;
        } while (uuid_search_i < variableCount );
        
        if (uuid_search_i > variableCount) {
            SerialStd.println(F("} match  & added."));
        } else 
        if (strcmp_P(name,index_pm)== 0) { //Check if index and then simple reference
            if (uuid_index < variableCount) 
            {
                SerialStd.print(F("} replacing {"));
                SerialStd.print(variableList[uuid_index]->getVarUUID() );
                SerialStd.println(F("}"));
                variableList[uuid_index]->setVarUUID_atl((char *)value,true);           
            } else {
                SerialStd.println(F("} out of range. Notused"));
            }
        } else 
        {
            //SerialStd.println();
            SerialStd.println(F(" UUID not supported"));
            //SerialStd.print(name);
            //SerialStd.print("=");
            //SerialStd.println(value);
        } 
        uuid_index++;
    } else if (strcmp_P(section,COMMON_pm)== 0) {// [COMMON] processing
        char *endptr;
        errno=0;
        if (strcmp_P(name,LOGGER_ID_pm)== 0) {
            SerialStd.print(F("COMMON LoggerId Set: "));
            SerialStd.println(value);
            dataLogger.setLoggerId(value,true);
        } else if (strcmp_P(name,LOGGING_INTERVAL_MINUTES_pm)== 0){
            //convert str to num with error checking
            long intervalMin = strtoul(value,&endptr,10);
            if ((intervalMin>0) &&(errno!=ERANGE) ) {
                if (intervalMin > loggingInterval_MAX_CDEF_MIN) {
                    SerialStd.print(F("COMMON LOGGING_INTERVAL_MINUTES must be less than : "));
                    SerialStd.print(loggingInterval_MAX_CDEF_MIN);
                    SerialStd.print(F(" Using Max. Reading ignored "));
                    SerialStd.println(intervalMin);
                    intervalMin= loggingInterval_MAX_CDEF_MIN;
                } else {
                    SerialStd.print(F("COMMON LOGGING_INTERVAL_MINUTES set to: "));
                    SerialStd.print(intervalMin);
                    SerialStd.print(F("(min) from default "));
                    SerialStd.println(loggingInterval_def_min);
                }
                dataLogger.setLoggingInterval(intervalMin);
                #if defined loggingMultiplier_MAX_CDEF
                dataLogFast.setLoggingInterval(intervalMin);
                #endif //loggingMultiplier_MAX_CDEF
                //loggingInterval_def_min = intervalMin; //Dup for time being
            } else {
                SerialStd.print(F(" Set interval error (range: 1-"));
                SerialStd.print(loggingInterval_MAX_CDEF_MIN);
                SerialStd.print(F(") with read:"));                
                SerialStd.println(intervalMin);
            }
#if defined loggingMultiplier_MAX_CDEF
       } else if (strcmp_P(name,LOGGING_INTERVAL_MULTIPLIER_pm)== 0){
            //convert str to num with error checking
            long multNum = strtoul(value,&endptr,10);
            if ((multNum>0) &&(errno!=ERANGE) ) {
                if (multNum > loggingInterval_MAX_CDEF_MIN) {
                    PRINTOUT(F("COMMON LOGGING_MULTIPLIER must be less than : "),
                        loggingInterval_MAX_CDEF_MIN,F(" Using Max. Reading ignored "),
                        multNum);
                    multNum= loggingMultiplier_MAX_CDEF;
                } else {
                    PRINTOUT(F("COMMON LOGGING_MULTIPLIER set to: "),multNum,
                        F("(mult) from default "),loggingMultiplier_MAX_CDEF);
                }
                loggingMultiplierTop= multNum; //In the main program
            } else {
                SerialStd.print(F(" Set interval error (range: 1-"));
                SerialStd.print(loggingInterval_MAX_CDEF_MIN);
                SerialStd.print(F(") with read:"));                
                SerialStd.println(multNum);
            }
#endif //loggingMultiplier_MAX_CDEF            
        } else if (strcmp_P(name,LIION_TYPE_pm)== 0){
            //convert  str to num with error checking
            long batLiionType = strtoul(value,&endptr,10);
            if ((batLiionType < PSLR_NUM) && (batLiionType>0) &&(errno!=ERANGE) ) {
                mcuBoard.setBatteryType((ps_liion_rating_t )batLiionType);
                //mayflyPhy.setBatteryType((ps_liion_rating_t )batLiionType);
                SerialStd.print(F("COMMON LiIon Type: "));
                SerialStd.println(batLiionType);
            } else {
                SerialStd.print(F(" Set LiIon Type error; (range 0-2) read:"));
                SerialStd.println(batLiionType);
            }
        } else if (strcmp_P(name,TIME_ZONE_pm)== 0){
            //convert  str to num with error checking
            long time_zone_local = strtol(value,&endptr,10);    
            if ((time_zone_local < 13) && (time_zone_local> -13) &&(errno!=ERANGE) ) {
                SerialStd.print(F("COMMON Set TimeZone ; "));
                timeZone=time_zone_local;
            } else {
                SerialStd.print(F("COMMON Set TimeZone error; (range -12 : +12) read:"));     
            }
            SerialStd.println(time_zone_local);           
        } else {
            SerialStd.print(F("COMMON tbd "));
            SerialStd.print(name);
            SerialStd.print(F(" to "));  
            SerialStd.println(value);  
        }       
    } else if (strcmp_P(section,SENSORS_pm)== 0) {
#if defined  INA219M_PHY_ACT      
        if (strcmp_P(name,INA219M_MA_MULT_pm)== 0)  {
            //For INA219M_MA_MULT expect a string with +ve number and covert to float
            char *endptr;
            float ampMult = (float) strtod(value,&endptr); 
            //MS_DBG("Found ", value," conv ", ampMult);
            if ((ampMult>0) &&(errno!=ERANGE) ) {
                SerialStd.print(F("SENSORS INA219_MA_MULT was '"));
                SerialStd.print(ina219m_phy.getCustomAmpMult());
                ina219m_phy.setCustomAmpMult(ampMult); 
                SerialStd.print(F("' set to '"));
                SerialStd.print(ina219m_phy.getCustomAmpMult());
                SerialStd.println("'");
            } else {
                SerialStd.print(F("SENSOR INA219_MA_MULT error:"));
                SerialStd.println(value);
            }
        } else if (strcmp_P(name,INA219M_V_THRESHLOW_pm)== 0)  {
            //For INA219M_V_THRESHLOW_pm expect a string with +ve number and covert to float
            float voltThreshold = (float) strtod(value,NULL);
            //MS_DBG("Found ", value," conv ", voltThreshold);
            if ((voltThreshold>0) &&(errno!=ERANGE) ) {
                SerialStd.print(F("SENSORS INA219_V_THRESHOLD was'"));
                SerialStd.print(ina219m_phy.getCustomVoltThreshold());
                ina219m_phy.setCustomVoltThreshold(voltThreshold,ina219m_voltLowThresholdAlertFn); 
                SerialStd.print(F("' set to '"));
                SerialStd.print(ina219m_phy.getCustomVoltThreshold());
                SerialStd.println("'");  
            } else {
                SerialStd.print(F("SENSOR INA219_V_THRESHOLD error:"));
                SerialStd.println(value);
            }
        } else
#endif //INA219M_PHY_ACT 
        {
            SerialStd.print(F("SENSORS tbd "));
            SerialStd.print(name);
            SerialStd.print(F(" to "));  
            SerialStd.println(value);  
        }
    } else if (strcmp_P(section,NETWORK_pm)== 0) {
#if defined DigiXBeeCellularTransparent_Module 
        if (strcmp_P(name,apn_pm)== 0) {
            SerialStd.print(F("NETWORK APN was '"));
            SerialStd.print(modemPhy.getApn());
            modemPhy.setApn(value,true);
            SerialStd.print(F("', now set to '"));
            SerialStd.print(modemPhy.getApn());
            SerialStd.println("'");
           
        } else
#endif //DigiXBeeCellularTransparent_Module  
#if defined  DigiXBeeWifi_Module          
        if (strcmp_P(name,WiFiId_pm)== 0)  {
            SerialStd.print(F("NETWORK WiFiId: was '"));
            SerialStd.print(modemPhy.getWiFiId());
            modemPhy.setWiFiId(value,true);
            SerialStd.print(F("' now '"));
            SerialStd.print(modemPhy.getWiFiId());
            SerialStd.println("'");
        } else if (strcmp_P(name,WiFiPwd_pm)== 0) {
            SerialStd.print(F("NETWORK WiFiPwd: was '"));
            SerialStd.print(modemPhy.getWiFiPwd());
            modemPhy.setWiFiPwd(value,true);
            SerialStd.print(F("' now '"));
            SerialStd.print(modemPhy.getWiFiPwd());
            SerialStd.println("'");
        } else
#endif //DigiXBeeWifi_Module
        {
            SerialStd.print(F("NETWORK tbd "));
            SerialStd.print(name);
            SerialStd.print(F(" to "));  
            SerialStd.println(value);  
        }
    } else if (strcmp_P(section,BOOT_pm)== 0) 
    {
        #if 0
        //FUT: needs to go into EEPROM
        if (strcmp_P(name,VER_pm)== 0) {
            strcpy(ps.provider.s.registration_token, value);
        } else
        const char VER_pm[] EDIY_PROGMEM = "VER";
const char MAYFLY_SN_pm[] EDIY_PROGMEM = "MAYFLY_SN"; 
const char MAYFLY_REV_pm[] EDIY_PROGMEM = "MAYFLY_REV";
const char MAYFLY_INIT_ID_pm[] EDIY_PROGMEM = "MAYFLY_INIT_ID";
        #endif  
        if (strcmp_P(name,MAYFLY_SN_pm)== 0) {
            //FUT: needs to go into EEPROM
            //strcpy(ps.hw_boot.s.Serial_num, value);
            //MFsn_def
            //FUT needs to be checked for sz
            SerialStd.print(F("Mayfly SerialNum :"));
            SerialStd.println(value);
#if 0
//Need to use to update EEPROM. Can cause problems if wrong. 
        } else if (strcmp_P(name,MAYFLY_REV_pm)== 0) {
            //FUT: needs to go into EEPROM
            //strcpy(ps.hw_boot.s.rev, value);
            //FUT needs to be checked for sz
            strcpy(MFVersion, value); //won't work with mcuBoardVersion
            SerialStd.print(F("Mayfly Rev:"));
            SerialStd.println(mcuBoardVersion);
#endif //
        } else
        {
            SerialStd.print(F("BOOT tbd "));
            SerialStd.print(name);
            SerialStd.print(F(" to "));  
            SerialStd.println(value);
        }  
    } else
    {
        SerialStd.print(F("Not supported ["));
        SerialStd.print(section);
        SerialStd.println(F("] "));
        SerialStd.print(name);
        SerialStd.print(F("="));  
        SerialStd.println(value);  
    }
    #if RAM_REPORT_LEVEL > 1
    if (ram_track) RAM_AVAILABLE;
    #endif //RAM_REPORT_LEVEL
    return 1;
}
#endif //USE_SD_MAYFLY_INI

//void ledflash(uint8_t numFlash = 4, unsigned long onTime_ms = 75,unsigned long offTime_ms = 150)
void ledflash(uint8_t numFlash, unsigned long onTime_ms,unsigned long offTime_ms)
{
    for (uint8_t i = 0; i < numFlash; i++) {
        setGreenLED( HIGH);
        delay(onTime_ms);
        setGreenLED( LOW);
        delay(offTime_ms);
    }
}