# In-Library Debugging<!-- {#page_code_debugging} -->

For intense _code_ debugging for any individual component of the library (sensor communication, modem communication, variable array functions, etc), open the source file header (\*.h), for that component.
Find the line `// #define DEBUGGING_SERIAL_OUTPUT xxxxx`, where xxxxx is the name of a serial output (ie, Serial or USBSerial).
Remove the two comment slashes from that line.
Then recompile and upload your code.
This will (sometimes dramatically) increase the number of statements going out to the debugging serial port.
A few sensors also the line `// #define DEEP_DEBUGGING_SERIAL_OUTPUT xxxxx`, un-commenting of which will send even more information to the defined port.
Note that this type of debugging is intended to help find errors in the code of this library, not problems with the results given by any sensors!
