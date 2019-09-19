#include <Wire.h>
#include <Adafruit_INA219.h>

Adafruit_INA219 ina219;


void setup(void) 
{
  Serial.begin(115200);
  while (!Serial) {
      // will pause Zero, Leonardo, etc until serial console opens
      delay(1);
  }
 
  Serial.println("getcurrent v0.01 Using Adafruit INA219 for measuring V and A");
  
  // Initialize the INA219.
  // By default the initialization will use the largest range (32V, 2A).  However
  // you can call a setCalibration function to change this range (see comments).
  ina219.begin();
  // To use a slightly lower 32V, 1A range (higher precision on amps):
  //ina219.setCalibration_32V_1A();
  // Or to use a lower 16V, 400mA range (higher precision on volts and amps):
  ina219.setCalibration_16V_400mA();

}
int elapsed_time_sec=0;
#define SAMPLE_DELAY_SEC 2
void loop(void) 
{
  //float shuntvoltage = 0;
  float busvoltage = 0;
  float current_mA = 0;
  //float loadvoltage = 0;
  //float power_mW = 0;
  char time_now[40];
  elapsed_time_sec += SAMPLE_DELAY_SEC;
  sprintf(time_now,"%5d",elapsed_time_sec);
  //shuntvoltage = ina219.getShuntVoltage_mV();
  ina219.powerSave(false);
  busvoltage = ina219.getBusVoltage_V();
  current_mA = ina219.getCurrent_mA();
  //power_mW = ina219.getPower_mW();
  //loadvoltage = busvoltage + (shuntvoltage / 1000);
  ina219.powerSave(true);

  Serial.print(time_now); Serial.print(":  ");
  Serial.print(busvoltage); Serial.print("V  ");
//  Serial.print("Shunt Voltage: "); Serial.print(shuntvoltage); Serial.println(" mV");
// Serial.print("Load Voltage:  "); Serial.print(loadvoltage); Serial.println(" V");
  Serial.print(current_mA); Serial.println("mA");
  //Serial.print("Power:         "); Serial.print(power_mW); Serial.println(" mW");
//  Serial.println("");

  delay(SAMPLE_DELAY_SEC*1000);
}
