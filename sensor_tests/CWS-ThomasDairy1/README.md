# Deployment Notes

#### SiteCode: CWS-ThomasDairy1
https://monitormywatershed.org/sites/CWS-ThomasDairy1/

#### Library Dependencies (in PlatformIO.ini)

```.ini
```

#### Logger Settings

```C++
```

#### Device Addresses/Pins:

```C++
```


#### Registration tokens:

```C++
const char *REGISTRATION_TOKEN = "75af6f65-b93d-4517-94e3-abdafaf9e253";   // Device registration token
const char *SAMPLING_FEATURE = "c24919f5-e95a-41ca-9bf8-ce241eeb0669";     // Sampling feature UUID
const char *UUIDs[] =                                                      // UUID array for device sensors
{
    "db7d4a18-5b98-4188-bf7d-8762cb1da2d3",   // Permittivity (Meter_Teros11_Ea)
    "9da8d39f-710d-4678-b1f3-009f667a7073",   // Temperature (Meter_Teros11_Temp)
    "8c26d5d9-da81-4218-a198-a78c815cedce",   // Volumetric water content (Meter_Teros11_VWC)
    "858b111c-014b-40ad-bf24-4aa15c35f1ef",   // Permittivity (Meter_Teros11_Ea)
    "929ca303-01b3-49a8-a96b-17121c8cb8c7",   // Temperature (Meter_Teros11_Temp)
    "f68e2659-f2bf-492e-a4d6-2313e825dc61",   // Volumetric water content (Meter_Teros11_VWC)
    "63401eba-984b-4b34-856d-9c0cbc9d9a48",   // Relative humidity (Bosch_BME280_Humidity)
    "99b87310-03aa-40ca-beab-2696e3db22bb",   // Barometric pressure (Bosch_BME280_Pressure)
    "fe553ef8-75cd-4677-9f01-82d9bbce3762",   // Temperature (Bosch_BME280_Temp)
    "6b3a0e43-3392-4e83-b0aa-ed236b5d9ae9"    // Battery voltage (EnviroDIY_Mayfly_Batt)
};
```
