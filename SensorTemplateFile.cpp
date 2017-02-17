// 
// 
// 

#include "SensorTemplateFile.h"

SensorTemplateFile::SensorTemplateFile(void)
    : Sensor<float/*SENSORS_DATA_TYPE*/>("VAR_CODE_HERE")
{
    // Do nothing. Not because we can't, but we don't need to.
}

bool SensorTemplateFile::update(void)
{
    // Get your sensor value however you need to
    // Store your value in sensorValue so we can grab it when we post data
    // Return true if the sensor reading was successful
    return true;
}


String SensorTemplateFile::getValueAsString()
{
    return String(int(sensorValue));
}


bool SensorTemplateFile::sleep(void)
{
    // Put your sensor to sleep, if applicable
    // Return true if successful
    return true;
}


bool SensorTemplateFile::wake(void)
{
    // Wake your sensor up, if applicable
    // Return true if successful
    return true;
}
