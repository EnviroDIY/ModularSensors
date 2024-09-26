/** =========================================================================
 * @example{lineno} Stream_Debug.ino
 * @brief Testing sketch to run StreamDebugger to copy text from one serial
 * output to another.
 *
 * @m_examplenavigation{page_extra_helper_sketches,}
 * ======================================================================= */

#include <Arduino.h>
#include <StreamDebugger.h>
StreamDebugger StreamDbg(Serial1, Serial);

void setup() {
    Serial.begin(115200);
    delay(50);

    Serial1.begin(9600);
    delay(50);

    StreamDbg.print("AT");
}

void loop() {
    // Start direct-access from Serial to Serial1
    StreamDbg.directAccess();
}
