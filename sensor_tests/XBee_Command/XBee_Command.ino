#include <Arduino.h>

void setup(){
  Serial.begin(9600);
  delay(50);

  Serial1.begin(9600);
  delay(10);
  Serial.println("----");

  // First write the output value, and only then set the output mode.
  digitalWrite(23, LOW);
  pinMode(23, OUTPUT);
  pinMode(19, INPUT);
/*
  Serial.println("Single Write");
  delay(1000);  // cannot send anything for 1 second before entering command mode
  Serial1.write('+');  // enter command mode
  Serial1.write('+');  // enter command mode
  Serial1.write('+');  // enter command mode
  Serial1.flush(); delay(10);
  Serial.println("+++");
  Serial.println("Available: " + String(Serial1.available(),2));
  if (Serial1.available() > 0) Serial.println(Serial1.readString());  else Serial.println("X-p");
  Serial1.write('A');
  Serial1.write('T');
  Serial1.write('A');
  Serial1.write('I');
  Serial1.write('\r');
  Serial1.flush(); delay(10);
  Serial.println(F("AT AI"));
  Serial.println("Available: " + String(Serial1.available(),2));
  if (Serial1.available() > 0) Serial.println(Serial1.readString());  else Serial.println("X-p");
  Serial1.write('A');    // Exit command mode
  Serial1.write('T');
  Serial1.write('C');
  Serial1.write('N');
  Serial1.write('\r');
  Serial1.flush(); delay(10);
  Serial.println(F("AT CN"));
  Serial.println("Available: " + String(Serial1.available(),2));
  if (Serial1.available() > 0) Serial.println(Serial1.readString());  else Serial.println("X-p");
  Serial.println("----");
  delay(15000);


  Serial.println("Multi Write");
  delay(1000);  // cannot send anything for 1 second before entering command mode
  Serial1.write("+++");  // enter command mode
  Serial1.flush(); delay(10);
  Serial.println("+++");
  Serial.println("Available: " + String(Serial1.available(),2));
  if (Serial1.available() > 0) Serial.println(Serial1.readString());  else Serial.println("X-p");
  Serial1.write("ATAI\r");  // Assn index
  Serial1.flush(); delay(10);
  Serial.println(F("AT AI"));
  Serial.println("Available: " + String(Serial1.available(),2));
  if (Serial1.available() > 0) Serial.println(Serial1.readString());  else Serial.println("X-p");
  Serial1.write("ATCN\r");  // Exit command mode
  Serial1.flush(); delay(10);
  Serial.println(F("AT CN"));
  Serial.println("Available: " + String(Serial1.available(),2));
  if (Serial1.available() > 0) Serial.println(Serial1.readString());  else Serial.println("X-p");
  Serial.println("----");
  delay(15000);

  Serial.println("Print");
  delay(1000);  // cannot send anything for 1 second before entering command mode
  Serial1.print("+++");  // enter command mode
  Serial1.flush(); delay(10);
  Serial.println("+++");
  Serial.println("Available: " + String(Serial1.available(),2));
  if (Serial1.available() > 0) Serial.println(Serial1.readString());  else Serial.println("X-p");
  Serial1.print("ATAI\r");  // Assn index
  Serial1.flush(); delay(10);
  Serial.println(F("AT AI"));
  Serial.println("Available: " + String(Serial1.available(),2));
  if (Serial1.available() > 0) Serial.println(Serial1.readString());  else Serial.println("X-p");
  Serial1.print("ATCN\r");  // Exit command mode
  Serial1.flush(); delay(10);
  Serial.println(F("AT CN"));
  Serial.println("Available: " + String(Serial1.available(),2));
  if (Serial1.available() > 0) Serial.println(Serial1.readString());  else Serial.println("X-p");
  Serial.println("----");
  delay(15000);

  Serial.println("Print Flash");
  delay(1000);  // cannot send anything for 1 second before entering command mode
  Serial1.print((F("+++")));  // enter command mode
  Serial1.flush(); delay(10);
  Serial.println("+++");
  Serial.println("Available: " + String(Serial1.available(),2));
  if (Serial1.available() > 0) Serial.println(Serial1.readString());  else Serial.println("X-p");
  Serial1.print((F("ATAI\r")));  // Assn Index
  Serial1.flush(); delay(10);
  Serial.println(F("AT AI"));
  Serial.println("Available: " + String(Serial1.available(),2));
  if (Serial1.available() > 0) Serial.println(Serial1.readString());  else Serial.println("X-p");
  Serial1.print((F("ATCN\r")));  // Exit command mode
  Serial1.flush(); delay(10);
  Serial.println(F("AT CN"));
  Serial.println("Available: " + String(Serial1.available(),2));
  if (Serial1.available() > 0) Serial.println(Serial1.readString());  else Serial.println("X-p");
  Serial.println("----");
  delay(15000);

  Serial.println("Print String");
  delay(1000);  // cannot send anything for 1 second before entering command mode
  Serial1.print(String("+++"));  // enter command mode
  Serial1.flush(); delay(10);
  Serial.println("+++");
  Serial.println("Available: " + String(Serial1.available(),2));
  if (Serial1.available() > 0) Serial.println(Serial1.readString());  else Serial.println("X-p");
  Serial1.print(String("ATAI\r"));  // assn index
  Serial1.flush(); delay(10);
  Serial.println(F("AT AI"));
  Serial.println("Available: " + String(Serial1.available(),2));
  if (Serial1.available() > 0) Serial.println(Serial1.readString());  else Serial.println("X-p");
  Serial1.print(String("ATCN\r"));  // Exit command mode
  Serial1.flush(); delay(10);
  Serial.println(F("AT CN"));
  Serial.println("Available: " + String(Serial1.available(),2));
  if (Serial1.available() > 0) Serial.println(Serial1.readString());  else Serial.println("X-p");
  Serial.println("----");
  delay(15000);

  Serial.println("Print Flash String");
  delay(1000);  // cannot send anything for 1 second before entering command mode
  Serial1.print(String(F("+++")));  // enter command mode
  Serial1.flush(); delay(10);
  Serial.println("+++");
  Serial.println("Available: " + String(Serial1.available(),2));
  Serial.println(Serial1.readString());
  // if (Serial1.available() > 0) Serial.println(Serial1.readString());  else Serial.println("X-p");
  Serial1.print(String(F("ATAI\r")));  // Assn index
  Serial1.flush(); delay(10);
  Serial.println(F("AT AI"));
  Serial.println("Available: " + String(Serial1.available(),2));
  Serial.println(Serial1.readString());
  // if (Serial1.available() > 0) Serial.println(Serial1.readString());  else Serial.println("X-p");
  Serial1.print(String(F("ATCN\r")));  // Exit command mode
  Serial1.flush(); delay(10);
  Serial.println(F("AT CN"));
  Serial.println("Available: " + String(Serial1.available(),2));
  Serial.println(Serial1.readString());
  // if (Serial1.available() > 0) Serial.println(Serial1.readString());  else Serial.println("X-p");
  Serial.println("----");
  delay(15000);
  */

}

void loop() {
  digitalWrite(23, HIGH);
  // while(digitalRead(19)==LOW){}
  delay(1100);  // cannot send anything for 1 second before entering command mode
  Serial1.print(String(F("+++")));  // enter command mode
  Serial1.flush();
  delay(1100);  // cannot send anything for 1 second after entering command mode
  Serial.println("+++");
  Serial.println("Available: " + String(Serial1.available(),2));
  // Serial.println(Serial1.readString());
  if (Serial1.available() > 0) Serial.println(Serial1.readString());  else Serial.println("X-p");
  Serial1.print(String(F("ATAI\r")));  // Assn index
  Serial1.flush(); delay(3);
  Serial.println(F("AT AI"));
  Serial.println("Available: " + String(Serial1.available(),2));
  // Serial.println(Serial1.readString());
  if (Serial1.available() > 0) Serial.println(Serial1.readString());  else Serial.println("X-p");
  Serial1.print(String(F("ATCN\r")));  // Exit command mode
  Serial1.flush(); delay(3);
  Serial.println(F("AT CN"));
  Serial.println("Available: " + String(Serial1.available(),2));
  // Serial.println(Serial1.readString());
  if (Serial1.available() > 0) Serial.println(Serial1.readString());  else Serial.println("X-p");
  Serial.println(Serial1.readString());
  Serial.println("----");
  digitalWrite(23, LOW);
  delay(10000);

  // digitalWrite(23, HIGH);
  // while(digitalRead(19)==LOW){}
  delay(1100);  // cannot send anything for 1 second before entering command mode
  Serial1.print(String(F("+++")));  // enter command mode
  Serial.println(String(F("+++")));  // enter command mode
  delay(1100);  // cannot send anything for 1 second after entering command mode
  Serial.println(Serial1.readString());
  // Serial1.print(String(F("ATRE\r")));  // set back to factory defaults
  // Serial.println(String(F("AT RE")));  // set back to factory defaults
  // Serial.println(Serial1.readString());
  // Serial1.print(String(F("ATAP0\r")));  // Put in transparent mode
  // Serial.println(String(F("AT AP 0")));  // Put in transparent mode
  // Serial.println(Serial1.readString());
  Serial1.print(String(F("ATAP1\r")));  // Put in API mode
  Serial.println(String(F("AT AP 1")));  // Put in API mode
  Serial.println(Serial1.readString());
  Serial1.print(String(F("ATIP1\r")));  // Put in TCP mode
  Serial.println(String(F("AT IP 1")));  // Put in TCP mode
  Serial.println(Serial1.readString());
  Serial1.print(String(F("ATSM1\r")));  // set sleep mode to pin sleep
  Serial.println(String(F("AT SM 1")));  // set sleep mode to pin sleep
  Serial.println(Serial1.readString());
  Serial1.print(String(F("ATSO200\r")));  // set sleep option to disconnected deep sleep
  Serial.println(String(F("AT SO 200")));  // set sleep option to disconnected deep sleep
  // 0x200 = b1000000000 -> Sleep Options Bit field.
  // Bit 6 - Stay associated with AP during sleep.
  // Bit 9 - Disassociate from AP for Deep Sleep.
  // All other bits ignored.
  Serial.println(Serial1.readString());
  Serial1.print(String(F("ATPD5DBF\r")));  // set the CTS and DTR pins to pull DOWN
  Serial.println(String(F("AT PD 5DBF")));  // set the CTS and DTR pins to pull DOWN
  // 0x5DBF = b101110110111111 -> Pin pulll up/pull down options bit field
  Serial.println(Serial1.readString());
  Serial1.print(String(F("ATGTFA\r")));  // shorten the guard time to 250ms
  Serial.println(String(F("AT GT FA")));  // shorten the guard time to 250ms
  // 0xFA = 250
  Serial.println(Serial1.readString());
  Serial1.print(String(F("ATWR\r")));  // Write changes to flash
  Serial.println(String(F("AT WR")));  // Write changes to flash
  Serial.println(Serial1.readString());
  Serial1.print(String(F("ATAC\r")));  // Apply changes
  Serial.println(String(F("AT AC")));  // Apply changes
  Serial.println(Serial1.readString());
  Serial1.print(String(F("ATCN\r")));  // Exit command mode
  Serial.println(String(F("AT CN")));  // Exit command mode
  Serial.println(Serial1.readString());
  Serial.println("----");
  digitalWrite(23, LOW);
  delay(10000);
}
