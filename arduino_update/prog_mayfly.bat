rem prog_mayfly <nam.hex>
rem c:\Users\neilh77\.platformio\packages\tool-avrdude\avrdude  -c stk500 -P COM10 -p m1284p %1
c:\users\neilh77\.platformio\packages\tool-avrdude\avrdude -v -p atmega1284p -C C:\Users\neilh77\.platformio\packages\tool-avrdude\avrdude.conf -c arduino -b 57600 -D -P "COM10" -U flash:w:%1:i
