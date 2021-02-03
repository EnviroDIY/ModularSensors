//Paste your own values below  
//Test06
const char *UUIDs[] =                                                      // UUID array for device sensors
{
    "a4361b13-a4b4-4d53-b1c8-1398d73ceeec",   //0 Gage height (HOBO_MX2001_gageHeight)
    "e5a6b171-ae06-4fc0-bc3f-68820f9e448c",   //1 Gage height (Keller_Acculevel_gageHeight)
    "b40cdca7-22c5-4d6e-91db-4eb77ebf2179",   //2 Temperature (Decagon_CTD-10_Temp)
    "9a848614-1dc2-4372-890b-68d2b48d9407",   //3 Temperature (Keller_Acculevel_Temp)
    "be84dc61-86dd-4ff9-9c44-f1d25bd78f6b",   //4 Battery voltage (EnviroDIY_Mayfly_Batt)
    "b404d204-f127-44d4-869c-50c058412999",   //5 Sequence number (EnviroDIY_Mayfly_SampleNum)
    "83fb7c9a-9af5-4bff-874a-d780045c19f0",   //6 Received signal strength indication (Digi_Cellular_RSSI)
    "93e8bb31-6102-45f0-85c1-c83057999192",   //7 Temperature (Digi_Cellular_Temp)
    "0ac7e1ad-2baf-4a54-b753-1a19fc4c8a17",   //8 Time, elapsed (Digi_Cellular_PriorActiveTime)
    "a6e185cb-32a4-4c05-bc7d-c14e87bc9701"    //9 Battery voltage (All_ExternalVoltage_Battery)
};
const char *registrationToken = "e89538ae-7ff0-4897-a396-9b10c0fadfc3";   // Device registration token
const char *samplingFeature = "31461753-cd5d-4ed0-99f7-17713c5e1e0e";     // Sampling feature UUID

    //Define only one modem
    #define MODEM_WIFIID_CDEF "Arthur2004Sid"
    #define MODEM_WIFIPWD_CDEF "LagunaGroup04Psk"
    //#define MODEM_LTE_APN_CDEF "xxx"