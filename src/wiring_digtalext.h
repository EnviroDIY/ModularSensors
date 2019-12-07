/*wiring_digtalext.h
 This diverts the wiring_digital port access to allow for a serial expansion
*/
#define once

void digitalWrExt( uint32_t ulPin, uint32_t ulVal );
void pinModExt( uint32_t ulPin, uint32_t ulMode );
int digitalReadExt( uint32_t ulPin );
#if !defined WIRING_DIGITALEXT_ACT

#define digitalWrite(ulPin,ulVal)  \
    MS_DEEP_DBG("digitalWriteExtD ",ulPin,"=",ulVal);\
    digitalWrExt(ulPin,ulVal)

#define pinModeExt(ulPin,ulVal)    pinModExt(ulPin,ulVal)
#define digitalReaddExt(ulPin) digitalRdExt(ulPin)

#endif //WIRING_DIGITALEXT_ACT