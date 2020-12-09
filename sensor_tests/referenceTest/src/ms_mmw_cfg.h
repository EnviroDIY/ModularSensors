const char* UUIDs[] =  // UUID array for device sensors
    {
        "0f9c6292-3646-4ab6-8aa3-ca542d5eee49",  // Battery voltage
                                                 // (EnviroDIY_Mayfly_Batt)
        "4b36e862-8dea-4f8a-a0d1-29ae20a92812",  // Temperature
                                                 // (EnviroDIY_Mayfly_Temp)
        "b6500c40-6339-47c9-ba65-5f717550432f",  // Percent full scale
                                                 // (Digi_WiFi_SignalPercent)
        "ef0c5561-42c8-49b2-8bc1-67cfbe2ffdd7",  // Battery voltage
                                                 // (All_ExternalVoltage_Battery)
        "ea56e49f-ae3c-49b0-b993-736c92c034ff",  // Battery voltage
                                                 // (All_ExternalVoltage_Battery)
        "c552fac5-c45c-416c-a634-5f22a20672de",  // Gage height
                                                 // (All_Calc_gageHeight)
        "a5d01a6e-5328-4ec8-9ab7-1110720b304e",  // Sequence number
                                                 // (EnviroDIY_Mayfly_SampleNum)
        "08a02c91-5336-4b98-999e-9867cb1c80ae",  // Received signal strength
                                                 // indication
                                                 // (Digi_Cellular_RSSI)
        "988edc32-629f-4fc6-8ab2-59ef2fa81c9c",  // Percent full scale
                                                 // (Digi_Cellular_SignalPercent)
        "a3b360bb-683f-4fb3-b720-67f0d585b9a0"  // Temperature
                                                // (Digi_Cellular_Temp)
};
const char* registrationToken =
    "38486242-cd9f-42f5-ad17-79b480cf2d28";  // Device registration token
const char* samplingFeature =
    "cb344d37-b557-400d-b999-f9b125cade29";  // Sampling feature UUID

    //Define only one modem
    #define MODEM_WIFIID_CDEF "Arthur2004Sid"
    #define MODEM_WIFIPWD_CDEF "LagunaGroup04Psk"
    //#define MODEM_LTE_APN_CDEF "xxx"