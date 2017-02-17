// SensorTemplateFile.h

#ifndef _SENSOR_TEMPLATE_FILE_h
#define _SENSOR_TEMPLATE_FILE_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif


class SensorTemplateFile : public Sensor<float/*SENSORS_DATA_TYPE*/>
{
public:
    SensorTemplateFile(void);
    bool update(void);
    String getValueAsString();
    bool sleep(void);
    bool wake(void);
};

#endif

