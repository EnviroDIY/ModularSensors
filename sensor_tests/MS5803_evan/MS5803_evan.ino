#include <Wire.h>
#include <SparkFun_MS5803_I2C.h>
MS5803 sensor(ADDRESS_HIGH);
int delaytime = 1000;
float temperature_c, temperature_f;
double pressure_abs, pressure_relative, altitude_delta, pressure_baseline;
double base_altitude = 0.0;
void setup() {
Serial.begin(9600);
  sensor.reset();
  sensor.begin();
  pressure_baseline = sensor.getPressure(ADC_4096);
}
void loop() {
pressure_abs = sensor.getPressure(ADC_4096);
pressure_relative = sealevel(pressure_abs, base_altitude);
Serial.print("pressure abs (mbar)= ");
Serial.println(pressure_abs);
Serial.print("Pressure relative (mbar)= ");
Serial.println(pressure_relative);
Serial.print("Altitude change (m)= ");
Serial.println(altitude_delta);
  delay(delaytime);
}
 double sealevel(double P, double A)
{
  return(P/pow(1-(A/44330.0),5.255));
}
double altitude(double P, double P0)
{
  return(44330.0*(1-pow(P/P0,1/5.255)));
}
