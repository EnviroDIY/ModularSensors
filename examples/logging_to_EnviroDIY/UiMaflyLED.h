/*
 * UI on Two Leds
 * Off longer than 5minutes - no power
 * Power up - Green flash - 1 sec On/Off
 * (Time good, sucessfull sync - all part of power up)
 * Time Sync fail - 3 Red flashes 1sec On/OFF 
 * Sampling Start - Green/0.5sec on/ 1sec off, preempts  any comms attempt
 * Sensor Fail -  5 Red/Green Flashes 
 * Communications Attempted -Grn 2sec On/ 1sec off  
 * Comms Fail -   5 Red flashes- On/0.5 Off/0.5
 * SD card Fail - 5 RedOn 1sec Off1sec RedOn1sec GrnOn1sec Off1sec
 * WakeUp ack - Grn 1sec flash, Min 2green flashes after button released
 * 
 * For power savings after 30minutes, no LEDs flash in closed box.
 * Sensor button short push initiates leds (less than 2secs)
 * Sensor button long push initiates sampling. (greater than 2secs with Grn leds flash)
 * Sensor button push after initial start up - initaites reading.
*/

#ifndef UILED_H
#define UILED_H

//#include "WProgram.h" 
enum uiLedState {
 uls_unInit,
 uls_pwrup,
 uls_sleep,
 uls_sampling,
 uls_sensorFail,
 uls_sdCardFail,
 uls_commsStart,
 uls_commsFail,

} ;
class UiLED{
  public:
    LED(uint8_t ledRedPinDef,uint8_t ledGrnPinDef); //Change
	void set(uint8_t reqState);
	uint8_t getState();
    void on();
	void off();
	void    switchOn();
	uint8_t switchOff();
	bool timer(); //Called over 0.5Sec?
	//void toggle();
	//void blink(unsigned int time, byte times=1);

	//void fadeIn(unsigned int time);
	//void fadeOut(unsigned int time);
  private:
	bool redLedStatus;
	bool grenLedStatus;
	uint8_t uiState;
	uint8_t timerCounter;
	uint8_t redPin;
	uint8_t grnPin;
};

//extern LED DEBUG_LED;

#endif //UILED_H

/*
|| @changelog

*/