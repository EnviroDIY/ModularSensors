/**
 * @file uiHelper.cpp
 * @copyright 2020 Neil Hancock
 * Used for Wio Terminal testing,  mostly mashup 
 * @author neil hancock <neilh20@wllw.net>
 *
 * @brief UI Help
 */

// Included Dependencies
#include "uiHelper.h"


//uiHelper::uiHelper() {}
//uiHelper::~uiHelper() {}

//uiHelper:: {}

void uiHelper::begin() {
    tft.begin();
    tft.setRotation(3);
    tft.fillScreen(TFT_BLACK);
    tft.setFreeFont(FMB12); 
}


void uiHelper::fillscreen(const char *msg) {
    tft.fillScreen(TFT_BLACK);
    tft.setCursor((320 - tft.textWidth(msg))/2, 120);
    tft.print(msg); 
}

void uiHelper::update3(String status, float param1,float param2, float param3) {
// -----------------LCD---------------------
    tft.setFreeFont(FF17);
    tft.setTextColor(tft.color565(224,225,232));
    tft.drawString(status,20,10);
 
    tft.fillRoundRect(10, 45, 300, 55, 5, tft.color565(40,40,86));
    tft.fillRoundRect(10, 105, 300, 55, 5, tft.color565(40,40,86));
    tft.fillRoundRect(10, 165, 300, 55, 5, tft.color565(40,40,86));
 
    tft.setFreeFont(FM9);
    tft.drawString("temperature:", 75, 50);
    tft.drawString("pressure:",75, 110);
    tft.drawString("humidity:",75, 170);
 
    tft.setFreeFont(FMB12);
    tft.setTextColor(TFT_RED);
    tft.drawFloat(param1,2 , 140, 75);
    tft.setTextColor(tft.color565(224,225,232));
    tft.drawFloat(param2,2 , 140, 135);
    tft.setTextColor(TFT_GREEN);
    tft.drawFloat(param3,2 , 140, 195);

    tft.drawString("C", 210, 75);
    tft.drawString("KPa",210, 135);
    tft.drawString("%",210, 195);
}






