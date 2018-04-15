/******************************************************************************
MS5803_I2C.cpp
Library for MS5803 pressure sensor.
Bobby Schulz @ Northern Widget LLC
6/26/2014
https://github.com/sparkfun/MS5803-14BA_Breakout

The MS5803 is a media isolated temperature and pressure sensor made by 
Measurment Specialties which can be used to measure either water pressure
and depth, or baramatric (atmospheric) pressure, and altitude along with that

"Instruments register only through things they're designed to register. 
Space still contains infinite unknowns."
-Mr. Spock

Distributed as-is; no warranty is given.
******************************************************************************/

#include <Wire.h> // Wire library is used for I2C
#include "MS5803_Mayfly.h"

MS5803::MS5803()
// Base library type I2C
{
	Wire.begin(); // Arduino Wire library initializer
	
}

void MS5803::reset(void)
// Reset device I2C
{
   sendCommand(CMD_RESET);
   sensorWait(3);
}

uint8_t MS5803::begin(ms5803_addr address, int MaxPressure)
// Initialize library for subsequent pressure measurements
{  
	_address = address; //set interface used for communication
	//Set model number based on maximum pressure range 
	if(MaxPressure == 1)	Model = 1;	//BA01
	if(MaxPressure == 2)	Model = 2;	//BA02
	if(MaxPressure == 5)	Model = 3; 	//BA05
	if(MaxPressure == 7)	Model = 4;	//BA07
	if(MaxPressure == 14)	Model = 5;	//BA14
	if(MaxPressure == 30)	Model = 6;	//BA30

	uint8_t i;
	for(i = 0; i <= 7; i++){
		sendCommand(CMD_PROM + (i * 2));
		Wire.requestFrom( _address, 2);
		uint8_t highByte = Wire.read(); 
		uint8_t lowByte = Wire.read();
		coefficient[i] = (highByte << 8)|lowByte;
	// Uncomment below for debugging output.
	//	Serial.print("C");
	//	Serial.print(i);
	//	Serial.print("= ");
	//	Serial.println(coefficient[i]);
	}

	switch(Model){
	case 1:
		{
		int ConvTemp1[] = {16, 7, 15, 8, 10000, 1, 31, 3, 0, 7, 0, 0, 3, 0, 0, 0};
		memcpy(ConvCoef, ConvTemp1, 16);
		}
		break;

	case 2:
		{
		int ConvTemp2[] = {17, 6, 16, 7, 10000, 1, 31, 61, 4, 2, 0, 20, 12, 0, 0, 0};
		memcpy(ConvCoef, ConvTemp2, 16);
		}
		break;

	case 3:
		{
		int ConvTemp3[] = {18, 5, 17, 7, 10000, 3, 33, 3, 3, 7, 3, 0, 3, 0, 0, 0};
		memcpy(ConvCoef, ConvTemp3, 16);
		}
		break;

	case 4:
		{
		int ConvTemp4[] = {18, 5, 17, 6, 2500, 3, 33, 3, 3, 7, 3, 0, 3, 0, 0, 0};
		memcpy(ConvCoef, ConvTemp4, 16);
		}
		break;

    case 5:
    	{
    	int ConvTemp5[] = {16, 7, 15, 8, 1000, 3, 33, 3, 1, 5, 3, 7, 4, 7, 37, 1};
    	memcpy(ConvCoef, ConvTemp5, 16);
    	}
    	break;

    case 6:
    	{
		int ConvTemp6[] = {16, 7, 15, 8, 10, 10, 3, 33, 3, 1, 5, 7, 7, 4, 7, 37, 1};
		memcpy(ConvCoef, ConvTemp6, 16);
		}
		break;
	}

	return 0;
}
	
float MS5803::getTemperature(temperature_units units, precision _precision)
// Return a temperature reading in either F or C.
{
	getMeasurements(_precision);
	float temperature_reported;
	// If Fahrenheit is selected return the temperature converted to F
	if(units == FAHRENHEIT){
		temperature_reported = _temperature_actual / 100.0f;
		temperature_reported = (((temperature_reported) * 9) / 5) + 32;
		return temperature_reported;
		}
		
	// If Celsius is selected return the temperature converted to C	
	else {
		temperature_reported = _temperature_actual / 100.0f;
		return temperature_reported;
	}
}

float MS5803::getPressure(precision _precision)
// Return a pressure reading units Pa.
{
	getMeasurements(_precision);
	float pressure_reported;
	pressure_reported = _pressure_actual;
	// pressure_reported = pressure_reported / 10;
	pressure_reported = pressure_reported / (float(ConvCoef[4])/100.0); //05BA model!
	return pressure_reported;
}

void MS5803::getMeasurements(precision _precision)

{
	//Retrieve ADC result
	int32_t temperature_raw = getADCconversion(TEMPERATURE, _precision);
	int32_t pressure_raw = getADCconversion(PRESSURE, _precision);
	
	
	//Create Variables for calculations
	int32_t temp_calc;
	int32_t pressure_calc;
	
	int32_t dT;
		
	//Now that we have a raw temperature, let's compute our actual.
	dT = temperature_raw - ((int32_t)coefficient[5] << 8);
	temp_calc = (((int64_t)dT * coefficient[6]) >> 23) + 2000;
	
	// TODO TESTING  _temperature_actual = temp_calc;
	
	//Now we have our first order Temperature, let's calculate the second order.
	int64_t T2, OFF2, SENS2, OFF, SENS; //working variables

	if (temp_calc < 2000) 
	// If temp_calc is below 20.0C
		//LOW TEMP
	{	
		T2 = ConvCoef[5]* (((int64_t)dT * dT) >> ConvCoef[6]);
		OFF2 = ConvCoef[7] * ((temp_calc - 2000) * (temp_calc - 2000)) / (pow(2,ConvCoef[8]));
		SENS2 = ConvCoef[9] * ((temp_calc - 2000) * (temp_calc - 2000)) / (pow(2,ConvCoef[10]));
		
		if(temp_calc < -1500)
		// If temp_calc is below -15.0C 
			//VERY LOW TEMP
		{
			OFF2 = OFF2 + ConvCoef[11] * ((temp_calc + 1500) * (temp_calc + 1500));
			SENS2 = SENS2 + ConvCoef[12] * ((temp_calc + 1500) * (temp_calc + 1500));
		}
    } 
	else
	// If temp_calc is above 20.0C
		//HIGH TEMP
	{ 
		T2 = ConvCoef[13] * ((uint64_t)dT * dT)/pow(2,ConvCoef[14]);
		OFF2 = ConvCoef[15]*((temp_calc - 2000) * (temp_calc - 2000)) / 16;
		SENS2 = 0;

		if(temp_calc > 4500 && Model == 1) SENS2 = SENS2 - ((temp_calc + 1500) * (temp_calc + 1500))/8; //NOTE: this condition is only used for the 01BA model! 
	}
	
	// Now bring it all together to apply offsets 
	

	OFF = ((int64_t)coefficient[2] << ConvCoef[0]) + (((coefficient[4] * (int64_t)dT)) >> ConvCoef[1]);	//05BA model!
	SENS = ((int64_t)coefficient[1] << ConvCoef[2]) + (((coefficient[3] * (int64_t)dT)) >> ConvCoef[3]); //05BA model!
	// OFF = ((int64_t)coefficient[2] << 16) + (((coefficient[4] * (int64_t)dT)) >> 7);
	// SENS = ((int64_t)coefficient[1] << 15) + (((coefficient[3] * (int64_t)dT)) >> 8);
	
	temp_calc = temp_calc - T2;
	OFF = OFF - OFF2;
	SENS = SENS - SENS2;

	// Now lets calculate the pressure
	

	pressure_calc = (((SENS * pressure_raw) / 2097152 ) - OFF) / 32768;
	
	_temperature_actual = temp_calc ;
	_pressure_actual = pressure_calc ; // 10;// pressure_calc;
	

}

uint32_t MS5803::getADCconversion(measurement _measurement, precision _precision)
// Retrieve ADC measurement from the device.  
// Select measurement type and precision
{	
	uint32_t result;
	uint8_t highByte, midByte, lowByte;
	
	sendCommand(CMD_ADC_CONV + _measurement + _precision);
	// Wait for conversion to complete
	sensorWait(1); //general delay
	switch( _precision )
	{ 
		case ADC_256 : sensorWait(1); break; 
		case ADC_512 : sensorWait(3); break; 
		case ADC_1024: sensorWait(4); break; 
		case ADC_2048: sensorWait(6); break; 
		case ADC_4096: sensorWait(10); break; 
	}	
	
	sendCommand(CMD_ADC_READ);
	Wire.requestFrom(_address, 3);
	
	while(Wire.available())    
	{ 
		highByte = Wire.read();
		midByte = Wire.read();
		lowByte = Wire.read();	
	}
	
	result = ((uint32_t)highByte << 16) + ((uint32_t)midByte << 8) + lowByte;

	return result;

}

void MS5803::sendCommand(uint8_t command)
{	
	Wire.beginTransmission( _address);
	Wire.write(command);
	Wire.endTransmission();
	
}

void MS5803::sensorWait(uint8_t time)
// Delay function.  This can be modified to work outside of Arduino based MCU's
{
	delay(time);
};


