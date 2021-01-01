#define TINY_GSM_MODEM_SARAR4
#define TINY_GSM_RX_BUFFER 64
#define TINY_GSM_YIELD_MS 2
#define TINY_GSM_DEBUG Serial


#include <Arduino.h>
#include <StreamDebugger.h>
#include <TinyGsmClient.h>

StreamDebugger debugger(Serial1, Serial);
TinyGsm        gsmModem(debugger);

const char* apn = "m2m";

void setup() {
    // Set the reset pin HIGH to ensure the Bee does not continually reset
    pinMode(20, OUTPUT);
    digitalWrite(20, HIGH);

    // Set the sleep_rq pin LOW to wake the Bee
    pinMode(23, OUTPUT);
    digitalWrite(23, LOW);

    // Set the input pin mode
    pinMode(19, INPUT);

    // Set console baud rate
    Serial.begin(115200);
    delay(10);

    // Set XBee module baud rate
    Serial1.begin(9600);

    // Wait for warm-up, probably overkill
    delay(6000);
}

void loop() {
    bool success = false;
    DBG(F("Putting XBee into command mode..."));
    for (uint8_t i = 0; i < 5; i++) {
        /** First, wait the required guard time before entering command mode. */
        delay(1010);
        /** Now, enter command mode to set all pin I/O functionality. */
        gsmModem.streamWrite(GF("+++"));
        success = gsmModem.waitResponse(2000, GF("OK\r")) == 1;
        if (success) break;
    }
    if (success) {
        DBG(F("Setting I/O Pins..."));
        /** Enable pin sleep functionality on `DIO9`.
         * NOTE: Only the `DTR_N/SLEEP_RQ/DIO8` pin (9 on the bee socket) can be
         * used for this pin sleep/wake. */
        gsmModem.sendAT(GF("D8"), 1);
        success &= gsmModem.waitResponse(GF("OK\r")) == 1;
        /** Enable status indication on `DIO9` - it will be HIGH when the XBee
         * is awake.
         * NOTE: Only the `ON/SLEEP_N/DIO9` pin (13 on the bee socket) can be
         * used for direct status indication. */
        gsmModem.sendAT(GF("D9"), 1);
        success &= gsmModem.waitResponse(GF("OK\r")) == 1;
        /** Enable CTS on `DIO7` - it will be `LOW` when it is clear to send
         * data to the XBee.  This can be used as proxy for status indication if
         * that pin is not readable.
         * NOTE: Only the `CTS_N/DIO7` pin (12 on the bee socket) can be used
         * for CTS. */
        gsmModem.sendAT(GF("D7"), 1);
        success &= gsmModem.waitResponse(GF("OK\r")) == 1;
        /** Enable association indication on `DIO5` - this is should be directly
         * attached to an LED if possible.
         * - Solid light indicates no connection
         * - Single blink indicates connection
         * - double blink indicates connection but failed TCP link on last
         * attempt
         *
         * NOTE: Only the `Associate/DIO5` pin (15 on the bee socket) can be
         * used for this function. */
        gsmModem.sendAT(GF("D5"), 1);
        success &= gsmModem.waitResponse(GF("OK\r")) == 1;
        /** Enable RSSI PWM output on `DIO10` - this should be directly attached
         * to an LED if possible.  A higher PWM duty cycle (and thus brighter
         * LED) indicates better signal quality.
         * NOTE: Only the `DIO10/PWM0` pin (6 on the bee socket) can be used for
         * this function. */
        gsmModem.sendAT(GF("P0"), 1);
        success &= gsmModem.waitResponse(GF("OK\r")) == 1;
        /** Enable pin sleep on the XBee. */
        DBG(F("Setting Sleep Options..."));
        gsmModem.sendAT(GF("SM"), 1);
        success &= gsmModem.waitResponse(GF("OK\r")) == 1;
        DBG(F("Setting Other Options..."));
        /** Disable remote manager, USB Direct, and LTE PSM.
         * NOTE:  LTE-M's PSM (Power Save Mode) sounds good, but there's no easy
         * way on the LTE-M Bee to wake the cell chip itself from PSM, so we'll
         * use the Digi pin sleep instead. */
        gsmModem.sendAT(GF("DO"), 0);
        success &= gsmModem.waitResponse(GF("OK\r")) == 1;
        /* Make sure USB direct is NOT enabled on the XBee3 units. */
        gsmModem.sendAT(GF("P1"), 0);
        success &= gsmModem.waitResponse(GF("OK\r")) == 1;
        // DBG(F("Setting Cellular Carrier Options..."));
        // // Carrier Profile - 1 = No profile/SIM ICCID selected
        // gsmModem.sendAT(GF("CP"),1);
        // success &= gsmModem.waitResponse(GF("OK\r")) == 1;
        // // Cellular network technology - LTE-M/NB IoT
        // gsmModem.sendAT(GF("N#"),0);
        // success &= gsmModem.waitResponse(GF("OK\r")) == 1;
        // Make sure airplane mode is off - bypass and airplane mode are
        // incompatible.
        DBG(F("Making sure airplane mode is off..."));
        gsmModem.sendAT(GF("AM"), 0);
        success &= gsmModem.waitResponse(GF("OK\r")) == 1;
        DBG(F("Turning on Bypass Mode..."));
        /** Enable bypass mode. */
        gsmModem.sendAT(GF("AP5"));
        success &= gsmModem.waitResponse(GF("OK\r")) == 1;
        /** Write changes to flash. */
        gsmModem.sendAT(GF("WR"));
        success &= gsmModem.waitResponse(GF("OK\r")) == 1;
        /** Apply changes. */
        gsmModem.sendAT(GF("AC"));
        success &= gsmModem.waitResponse(GF("OK\r")) == 1;
        // Finally, force a reset to actually enter bypass mode - this
        // effectively exits command mode.
        DBG(F("Resetting the module to reboot in bypass mode..."));
        gsmModem.sendAT(GF("FR"));
        success &= gsmModem.waitResponse(5000L, GF("OK\r")) == 1;
        // Allow 5s for the unit to reset.
        delay(500);
        // Re-initialize the TinyGSM SARA R4 instance.
        DBG(F("Attempting to reconnect to the u-blox SARA R410M module..."));
        success &= gsmModem.init();
        gsmModem.getModemName();
    } else {
        // wait a bit
        delay(30000L);
        // try again
        return;
    }

    DBG(F("Setting Cellular Carrier Options..."));
    // Turn off the cellular radio while making network changes
    gsmModem.sendAT(GF("+CFUN=0"));
    gsmModem.waitResponse();
    // Mobile Network Operator Profile
    // - 0: SW default
    // - 1: SIM ICCID selected
    // - 2: ATT
    // - 3: Verizon
    // - 4: Telstra
    // - 5: T-Mobile US
    // - 6: China Telecom
    // - 8: Sprint
    // - 19: Vodafone
    // - 20: NTT DoCoMo
    // - 21: Telus
    // - 28: SoftBank
    // - 31: Deutsche Telekom
    // - 32: US Cellular
    // - 33: VIVO
    // - 39: SKT
    // - 44: Claro Brasil
    // - 45: TIM Brasil
    // - 46: Orange France
    // - 90: global
    // - 100: Standard Europe
    // - 101: Standard Europe No-ePCO (The factory-programmed configuration of
    // this profile is the same of the standard Europe profile (<MNO>=100), but
    // the ePCO is disabled.)
    // - 102: Standard Japan (global)
    // - 198: AT&T 2-4-12 (The factory programmed configuration of this profile
    // is the same of the AT&T profile (<MNO>=2), but the LTE band 5 is
    // disabled.)
    // - 201: GCF-PTCRB (This profile is meant only for conformance testing.)
    gsmModem.sendAT(GF("+UMNOPROF="), 1);
    gsmModem.waitResponse();
    // Selected network technology - 7: LTE Cat.M1
    //                             - 8: LTE Cat.NB1
    // Fallback network technology - 7: LTE Cat.M1
    //                              - 8: LTE Cat.NB1
    // NOTE:  As of 2020 in the USA, AT&T and Verizon only use LTE-M
    // T-Mobile uses NB-IOT
    gsmModem.sendAT(GF("+URAT="), 7, ',', 8);
    gsmModem.waitResponse();
    // Set the band mask manually if needed
    // bit 0 = band 1; bit 127 = band 128
    // gsmModem.sendAT(GF("+UBANDMASK="), 0, ',', 134217732);
    // gsmModem.waitResponse();
    // Restart the module to apply changes and bring back to full functionality
    gsmModem.restart();

    // Check again for the carrier profile (to ensure it took)
    // If 1/SIM select was used, this will show what the SIM picked
    gsmModem.sendAT(GF("+UMNOPROF?"));
    gsmModem.waitResponse();

    // Scan for networks - this is probably really slow
    DBG(F("Scanning for networks.  This may take up to 3 minutes"));
    gsmModem.sendAT(GF("+COPS=0"));
    gsmModem.waitResponse();
    gsmModem.sendAT(GF("+COPS=?"));
    gsmModem.waitResponse(180000L);

    // Wait forever for a connection
    DBG(F("Waiting for network registration"));
    while (!gsmModem.isNetworkConnected()) {
        int csq = gsmModem.getSignalQuality();
        DBG("Signal quality:", csq);
        delay(250);
    }

    // Print some stuff after connected
    String ccid = gsmModem.getSimCCID();
    DBG("CCID:", ccid);

    String imei = gsmModem.getIMEI();
    DBG("IMEI:", imei);

    String imsi = gsmModem.getIMSI();
    DBG("IMSI:", imsi);

    String cop = gsmModem.getOperator();
    DBG("Operator:", cop);

    IPAddress local = gsmModem.localIP();
    DBG("Local IP:", local);


    // Shut down
    gsmModem.poweroff();
    DBG("Powering down.");

    // And do nothing forever more.
    while (1) {}
}