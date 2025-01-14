# In-Library Debugging<!-- {#page_code_debugging} -->

There are a huge number of code debugging printouts available in the library, if you enable them.

## PlatformIO<!-- {#code_debugging_pio} -->

When using PlatformIO or other build systems that allow you to define build flags, you can set these general debugging options:

- ```-D MS_SERIAL_OUTPUT=SerialX``` to define a serial or other stream to use for all printout, debugging or otherwise
- ```-D MS_2ND_OUTPUT=SerialX``` to define a second serial to output identical debugging output to
  - This is very helpful for boards that use a built-in USB adapter.  Assigning a secondary output that can be attaced to a secondary TTL-to-USB adapter can make debugging much easier.
- ```-D MS_SILENT``` to surpress *ALL* output.

There is also a debugging - and sometimes deep debugging - define for each component.
The debugging flags are generally named as MS_xxx_DEBUG`, where xxxxx is the name of the header file to debug - in all caps with spaces removed.

## Arduino IDE<!-- {#code_debugging_pio} -->

For intense _code_ debugging for any individual component of the library (sensor communication, modem communication, variable array functions, etc), open the source file header (\*.h), for that component.
Find the line `// #define MS_xxx_DEBUG`, where xxxxx is the name of the header file to debug - in all caps with spaces removed.
Remove the two comment slashes from that line.
Then recompile and upload your code.
This will (sometimes dramatically) increase the number of statements going out to the debugging serial port.
A few sensors also the line `// #define MS_xxx_DEBUG_DEEP`, un-commenting of which will send even more information to the defined port.
Note that this type of debugging is intended to help find errors in the code of this library, not problems with the results given by any sensors!
