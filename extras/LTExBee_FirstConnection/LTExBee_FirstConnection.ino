#define TINY_GSM_MODEM_XBEE
#define TINY_GSM_RX_BUFFER 64
#define TINY_GSM_YIELD_MS 2
#define TINY_GSM_DEBUG Serial


#include <Arduino.h>
#include <StreamDebugger.h>
#include <TinyGsmClient.h>

StreamDebugger debugger(Serial1, Serial);
TinyGsm        gsmModem(debugger);

const char* apn = "hologram";

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
    /** First run the TinyGSM init() function for the XBee. */
    DBG(F("Initializing the XBee..."));
    gsmModem.init();

    /** Then enter command mode to set pin outputs. */
    DBG(F("Putting XBee into command mode..."));
    if (gsmModem.commandMode()) {
        DBG(F("Setting I/O Pins..."));
        /** Enable pin sleep functionality on `DIO9`.
         * NOTE: Only the `DTR_N/SLEEP_RQ/DIO8` pin (9 on the bee socket) can be
         * used for this pin sleep/wake. */
        gsmModem.sendAT(GF("D8"), 1);
        gsmModem.waitResponse();
        /** Enable status indication on `DIO9` - it will be HIGH when the XBee
         * is awake.
         * NOTE: Only the `ON/SLEEP_N/DIO9` pin (13 on the bee socket) can be
         * used for direct status indication. */
        gsmModem.sendAT(GF("D9"), 1);
        gsmModem.waitResponse();
        /** Enable CTS on `DIO7` - it will be `LOW` when it is clear to send
         * data to the XBee.  This can be used as proxy for status indication if
         * that pin is not readable.
         * NOTE: Only the `CTS_N/DIO7` pin (12 on the bee socket) can be used
         * for CTS. */
        gsmModem.sendAT(GF("D7"), 1);
        gsmModem.waitResponse();
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
        gsmModem.waitResponse();
        /** Enable RSSI PWM output on `DIO10` - this should be directly attached
         * to an LED if possible.  A higher PWM duty cycle (and thus brighter
         * LED) indicates better signal quality.
         * NOTE: Only the `DIO10/PWM0` pin (6 on the bee socket) can be used for
         * this function. */
        gsmModem.sendAT(GF("P0"), 1);
        gsmModem.waitResponse();
        /** Enable pin sleep on the XBee. */
        DBG(F("Setting Sleep Options..."));
        gsmModem.sendAT(GF("SM"), 1);
        gsmModem.waitResponse();
        /** Disassociate from the network for the lowest power deep sleep. */
        gsmModem.sendAT(GF("SO"), 0);
        gsmModem.waitResponse();
        DBG(F("Setting Other Options..."));
        /** Disable remote manager, USB Direct, and LTE PSM
         * NOTE:  LTE-M's PSM (Power Save Mode) sounds good, but there's no easy
         * way on the LTE-M Bee to wake the cell chip itself from PSM, so we'll
         * use the Digi pin sleep instead. */
        gsmModem.sendAT(GF("DO"), 0);
        gsmModem.waitResponse();
        /** Ask data to be "packetized" and sent out with every new line (0x0A)
         * character. */
        gsmModem.sendAT(GF("TD0A"));
        gsmModem.waitResponse();
        /* Make sure USB direct is NOT enabled on the XBee3 units. */
        gsmModem.sendAT(GF("P1"), 0);
        gsmModem.waitResponse();
        /** Set the socket timeout to 10s (this is default). */
        gsmModem.sendAT(GF("TM"), 64);
        gsmModem.waitResponse();

        DBG(F("Setting Cellular Carrier Options..."));
        // Carrier Profile - 0 = Automatic selection
        //                 - 1 = No profile/SIM ICCID selected
        //                 - 2 = AT&T
        //                 - 3 = Verizon
        // NOTE:  To select T-Mobile, you must enter bypass mode!
        gsmModem.sendAT(GF("CP"), 2);
        gsmModem.waitResponse();
        // Cellular network technology - 0 = LTE-M with NB-IoT fallback
        //                             - 1 = NB-IoT with LTE-M fallback
        //                             - 2 = LTE-M only
        //                             - 3 = NB-IoT only
        gsmModem.sendAT(GF("N#"), 0);
        gsmModem.waitResponse();

        DBG(F("Setting the APN..."));
        /** Save the network connection parameters. */
        gsmModem.gprsConnect(apn);
        DBG(F("Ensuring XBee is in transparent mode..."));
        /* Make sure we're really in transparent mode. */
        gsmModem.sendAT(GF("AP0"));
        gsmModem.waitResponse();
        /** Write all changes to flash and apply them. */
        DBG(F("Applying changes..."));
        gsmModem.writeChanges();
        /** Finally, exit command mode. */
        gsmModem.exitCommand();
        /** Force restart the modem to make sure all settings take. */
        DBG(F("Restarting XBee..."));
        gsmModem.restart();
    } else {
        // wait a bit
        delay(30000L);
        // try again
        return;
    }

    // Scan for networks - this is probably really slow
    gsmModem.sendAT(GF("AS"));
    gsmModem.waitResponse(180000L, GF("S"), GF("ERROR"));
    while (Serial1.available()) {
        Serial.println(Serial1.readStringUntil('\r'));
    }

    // Wait forever for a connection
    DBG(F("Waiting for network registration"));
    while (!gsmModem.isNetworkConnected()) {
        int csq = gsmModem.getSignalQuality();
        DBG("Signal quality:", csq);
        delay(250);
    }

    // Scan for networks - this is probably really slow
    gsmModem.sendAT(GF("AS"));
    gsmModem.waitResponse(180000L, GF("S"), GF("ERROR"));
    while (Serial1.available()) {
        Serial.println(Serial1.readStringUntil('\r'));
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