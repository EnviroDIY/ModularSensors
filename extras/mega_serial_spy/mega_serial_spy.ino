/** =========================================================================
 * @file mega_serial_spy.ino
 * @brief Testing sketch to run on an Arduino Mega to print all output from
 * connected serial ports to the terminal.
 * ======================================================================= */

#include <Arduino.h>

void changeBauds(void) {
    Serial.read();
    for (uint8_t i = 1; i < 4; i++) {
        Serial.print("Select a baud rate for Serial");
        Serial.print(i);
        Serial.println(" to monitor at:");
        Serial.println("");
        Serial.println("[1] - 9600");
        Serial.println("[2] - 57600");
        Serial.println("[3] - 115200");
        Serial.println("[4] - 74880");
        Serial.println("");
        Serial.println(
            "Enter you selection in the Serial Monitor and press <enter>");
        Serial.println("");

        String user_input = "";
        int    selection  = 0;

        // Wait for user feedback, then parse feedback one byte at a time
        while ((Serial.peek() != 255) && !selection) {
            char incoming = Serial.read();
            if (isDigit(incoming)) {
                // Append the current digit to the string placeholder
                user_input += static_cast<char>(incoming);
            }
            // Parse the string on new-line
            if (incoming == '\n') { selection = user_input.toInt(); }
            delay(2);
        }

        uint32_t baud = 9600;

        if (selection) {
            switch (selection) {
                case 1:
                default: baud = 9600; break;
                case 2: baud = 57600; break;
                case 3: baud = 115200; break;
                case 4: baud = 74880; break;
            }
        }

        Serial.print("Starting Serial");
        Serial.print(i);
        Serial.print(" at ");
        Serial.print(baud);
        Serial.println(" baud.");

        switch (i) {
            case 1: Serial1.begin(baud); break;
            case 2: Serial2.begin(baud); break;
            case 3: Serial3.begin(baud); break;
        }
    }
}


void setup() {
    Serial.begin(115200);

    // Wait for the Serial Monitor to open
    while (!Serial) {
        // Delay required to avoid RTOS task switching problems
        delay(1);
    }

    delay(500);  // Short delay for cosmetic reasons
    Serial.println("");
    Serial.println("Serial port Spy\r\n");
    Serial.println("-----------------------------------------------------------"
                   "--------------------");
    changeBauds();
}

void loop() {
    // From HW UART1 to USB
    while (Serial1.available()) Serial.write(Serial1.read());
    // From HW UART2 to USB
    while (Serial2.available()) Serial.write(Serial2.read());
    // From HW UART4 to USB
    while (Serial3.available()) Serial.write(Serial3.read());

    if (Serial.available()) changeBauds();
}
