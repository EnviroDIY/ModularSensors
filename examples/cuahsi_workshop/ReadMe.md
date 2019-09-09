
### Example for the CUAHSI Workshop

### To Do:

1.  Give your logger a name in line 49 by replacing the x's with your own logger id:

```cpp
const char *LoggerID = "XXXXX";
```

2.  Fill in the correct WiFi Id and Password in lines 119 and 120:

```cpp
const char *wifiId = "xxxxx";  // The WiFi access point, unnecessary for gprs
const char *wifiPwd = "xxxxx";  // The password for connecting to WiFi, unnecessary for gprs
```


3.  Verify your sensor attachment pins:
    - CTD is connected to the D6-7 plug (line 159, SDI-12 data on pin 7)
    - Maxim DS18 is connected to the D4-5 plug (line 212, OneWire bus on pin 4)
    - Ultrasonic is connected to the D10-11 plug (line 79, software data Rx on pin 11)

4.  If using ultrasonic, and interested in water depth instead of distance to the water, create a calculated variable for water depth by uncommenting lines 211, 236-259 and 281-282 and commenting out line 275.

5.  Fill out all of the variable UUID's
    - lines 269 - 280, 211, and 253 as applicable

6.  Fill out your registration token and sampling feature UUID in lines 305-306:

```cpp
const char *registrationToken = "12345678-abcd-1234-ef00-1234567890ab";   // Device registration token
const char *samplingFeature = "12345678-abcd-1234-ef00-1234567890ab";     // Sampling feature UUID
```

7.  Compile and upload!
