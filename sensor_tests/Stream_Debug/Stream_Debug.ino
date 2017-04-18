#include <Arduino.h>
#include <StreamDebugger.h>
StreamDebugger StreamDbg(Serial1, Serial);

void setup(){
  Serial.begin(9600);
  delay(50);

  Serial1.begin(9600);
  delay(10);

  StreamDbg.print("Hello");

}

void loop() {
    // Start direct-access from Serial to Serial1
    StreamDbg.directAccess();
}
