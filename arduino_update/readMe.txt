To enable 3 more SERCOM/SAMD51 as Serial2->4

Step to
C:\Users\<user>\.platformio\packages\framework-arduinosam\variants\feather_m4
rename variant.h   -> variant.h_orig
rename variant.cpp -> variant.cpp_orig

Copy in files variant.cpp & variant.h from 
ModularSensors\arduino_update\variants\feather_m4\

For Arduino Pin extensions 


C:\Users\<user>\.platformio\packages\framework-arduinosam\cores\adafruit
rename wiring_digital.c -> wiring_digital.c_org
rename Wvariant.h       -> Wvariant.h_orig

Copy in files wiring_digital.c & Wvariant.h from 
ModularSensors\arduino_update\cores\adafruit
