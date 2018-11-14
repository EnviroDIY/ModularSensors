
#ifndef mayfly_routing_h
#define mayfly_routing_h

// Local routing defitions here
// FUT: Most of this information should go in EEPROM
// Boards functions are modified based on name

// Mayfly serial 160141 Rev 0.4 - always on voltage moniotring
// Function testing
#define BOARD01 1
//Mayfly sn 180368 Rev 0.5b - RS485 switched
#define BOARD02 2
//Mayfly sn 180256 Rev 0.5b test
#define BOARD03 3

#define BOARD_NAME BOARD02


#if   BOARD_NAME == BOARD01
//**************************************************************************
//#define SENSOR_RS485_PHY TRUE
const char *MFVersion = "v0.4";
const char *MFsn ="160141";
// How frequently (in minutes) to log data
const uint8_t loggingInterval = 5;
const char *apn = "xxxxx";  // The APN for the gprs connection, unnecessary for WiFi
const char *wifiId = "ArthurStrGuest";  // The WiFi access point, unnecessary for gprs
const char *wifiPwd = "";  // The password for connecting to WiFi, unnecessary for gprs

//#define SENSOR_CONFIG_GENERAL 1
//#define SENSOR_CONFIG_KELLER_ACCULEVEL 1
//#define SENSOR_CONFIG_KELLER_NANOLEVEL 1

#define registrationToken_UUID   "38486242-cd9f-42f5-ad17-79b480cf2d28"
#define samplingFeature_UUID     "cb344d37-b557-400d-b999-f9b125cade29"

#define MaximDS3231_Temp_UUID    "4b36e862-8dea-4f8a-a0d1-29ae20a92812"
#define ProcessorStats_Batt_UUID "0f9c6292-3646-4ab6-8aa3-ca542d5eee49"
#define ProcessorStats_SampleNum_UUID  "c552fac5-c45c-416c-a634-5f22a20672de" 
#define Volt0_UUID "ef0c5561-42c8-49b2-8bc1-67cfbe2ffdd7"
#define Volt1_UUID "ea56e49f-ae3c-49b0-b993-736c92c034ff"

#elif BOARD_NAME == BOARD02
//**************************************************************************
#define SENSOR_RS485_PHY 1
const char *MFVersion = "v0.5b";
const char *MFsn ="180368";
// How frequently (in minutes) to log data
const uint8_t loggingInterval = 2;
const char *apn = "xxxxx";  // The APN for the gprs connection, unnecessary for WiFi
const char *wifiId = "AzondeNetSsid";  // The WiFi access point, unnecessary for gprs
const char *wifiPwd = NULL;  // The password for connecting to WiFi, unnecessary for gprs
//#define SENSOR_CONFIG_GENERAL 1
//#define SENSOR_CONFIG_KELLER_ACCULEVEL 1
#define SENSOR_CONFIG_KELLER_NANOLEVEL 1
#define registrationToken_UUID "7b19191b-b3d6-416d-9e07-4fbe549f2493"
#define samplingFeature_UUID   "b918f150-4fff-424d-813a-a69a808a93b6"

#define KellerNanolevel_Height_UUID "f40a9dff-2c54-43ae-a016-1c5095c188eb"
#define KellerNanolevel_Temp_UUID   "7bc30855-ae08-4865-ab89-a8e17df93bfc"
#define ProcessorStats_Batt_UUID    "2c58e64d-6b66-4d9e-b893-bfdb10b65426"
#define MaximDS3231_Temp_UUID       "3907922a-56fe-46f3-a56e-9de6b77d3679"
//#define Modem_RSSI_UUID ""
// Try without as something crashing Mayfly
//#define Modem_SignalPercent_UUID    "0cf94fc8-a5d2-4fbe-82f2-2a81650575a8"
#define ProcessorStats_SampleNum_UUID  "0cf94fc8-a5d2-4fbe-82f2-2a81650575a8"
 
// From LiIon 100K+100K
#define Volt0_UUID "9e47e1aa-2ef6-4283-9a25-537cfd78b17b"
// From Solar - 100K+100K
#define Volt1_UUID "ebddd4d7-562d-481b-8356-d1f464fc5685"

#elif BOARD_NAME == BOARD03
//**************************************************************************
//Keller Nanolevel with XBP-u.fl 
#define SENSOR_RS485_PHY 1
const char *MFVersion = "v0.5b";
const char *MFsn ="180256";
// How frequently (in minutes) to log data
const uint8_t loggingInterval = 5;
const char *apn = "xxxxx";  // The APN for the gprs connection, unnecessary for WiFi
const char *wifiId = "AzondeNetSsid";  // The WiFi access point, unnecessary for gprs
const char *wifiPwd = NULL;//"";  // The password for connecting to WiFi, unnecessary for gprs
//#define SENSOR_CONFIG_GENERAL 1
//#define SENSOR_CONFIG_KELLER_ACCULEVEL 1
#define SENSOR_CONFIG_KELLER_NANOLEVEL 1
#define registrationToken_UUID "d96bf9fb-faca-4cc3-bcb9-3d23255a1f3c"
#define samplingFeature_UUID   "79f702a9-368f-4940-9669-8978ffa3254b"

#define KellerNanolevel_Height_UUID "67c22f5d-e5d8-4a26-a82b-7f59132e5c81"
#define KellerNanolevel_Temp_UUID   "b03d4384-4623-4df5-b705-f50710d5e4e9"
#define ProcessorStats_Batt_UUID    "8c796edd-7863-4fe7-9e54-0cbe0d694d59"
#define MaximDS3231_Temp_UUID       "e6159fe0-e30d-4a9d-bebc-1dc5c2435a22"

//#define Modem_RSSI_UUID ""
// Try without as something crashing Mayfly
//#define Modem_SignalPercent_UUID    ""
// The following mapped to Mayfly_FreeRAM
#define ProcessorStats_SampleNum_UUID "5fbb799d-630d-486f-a0ff-015f0195d393"
// From LiIon 100K+100K
#define Volt0_UUID "d3b78c2e-312b-4e2a-b804-8230c963f912"
// From Solar - 100K+100K
#define Volt1_UUID "c7da692b-6661-4545-bd3d-04938faa285b"
#endif //BOARD_NAME

#endif //mayfly_routing_h