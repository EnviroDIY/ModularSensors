
#ifndef SRC_UI_HELPER_H_
#define SRC_UI_HELPER_H_
#include "Arduino.h"
#include"Free_Fonts.h"
#include"TFT_eSPI.h"


class uiHelper {
public:
void begin();
void fillscreen(const char *msg);
void update3(String status, float param1=0.0,float param2=0.0, float param3=0.0);

TFT_eSPI tft;
private :


};


#endif // SRC_UI_HELPER_H_
