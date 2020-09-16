To enable 3 more SERCOM/SAMD51 as Serial2->4

Step to
C:\Users\<user>\.platformio\packages\framework-arduino-samd-adafruit\variants\feather_m4
rename variant.h   -> variant.h_orig_<date>
rename variant.cpp -> variant.cpp_orig_<date>

Copy in files variant.cpp & variant.h from 
ModularSensors\arduino_update\arduino-samd-adafruit\variants\feather_m4\


For Arduino Pin extensions 

C:\Users\<user>\.platformio\packages\framework-arduino-samd-adafruit\cores\adafruit



C:\Users\<user>\.platformio\packages\framework-arduino-samd-adafruit\cores\adafruit
rename wiring_digital.c -> wiring_digital.c_org_<date>
rename Wvariant.h       -> Wvariant.h_orig_<date>

Copy in files wiring_digital.c & Wvariant.h from 
ModularSensors\arduino_update\framework-arduino-samd-adafruit\cores\adafruit

repeat for other packages as needed 
<future>
framework-arduino-samd-adafruit\variants\pygamer_m4
framework-arduino-samd-adafruit\variants\pygamer_advance_m4

framework-arduino-samd-seeed\xxx
<end future>