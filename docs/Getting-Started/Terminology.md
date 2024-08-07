# Library Terminology<!-- {#page_library_terminology} -->

## Terms<!-- {#library_terminology_terms} -->

Within this library, a Sensor, a Variable, and a Logger mean very specific things:

[//]: # ( @tableofcontents )

[//]: # ( @m_footernavigation )

[//]: # ( Start GitHub Only )

- [Library Terminology](#library-terminology)
  - [Terms](#terms)
    - [Sensor](#sensor)
    - [Variable](#variable)
    - [Logger](#logger)
    - [Modem](#modem)
    - [DataPublisher](#datapublisher)
  - [Library Structure](#library-structure)

[//]: # ( End GitHub Only )

### Sensor<!-- {#library_terminology_sensor} -->

A Sensor is some sort of device that is capable of taking one or more measurements using some sort of method.
Most often we can think of these as probes or other instruments that can give back information about the world around them.
Sensors can usually be given power or have that power cut.
They may be awoken or activated and then returned to a sleeping/low power use state.
The may need to be asked to begin a single reading or they may continuously return data.
They _**must**_ be capable of returning the value of their readings to a logger of some type.

The detailed Sensor class documentation is here:  <https://envirodiy.github.io/ModularSensors/class_sensor.html>

### Variable<!-- {#library_terminology_variable} -->

A Variable is a result value taken by a Sensor _or_ calculated from the results of one or more sensors.
It is characterized by a name (what it is a measurement of), a unit of measurement, and a resolution.
The [names](http://vocabulary.odm2.org/variablename/) and [units](http://vocabulary.odm2.org/units/) of measurements for all variables come from the controlled vocabularies developed for the ODM2 data system.
(<http://vocabulary.odm2.org/>)  The resolution is determined by the method used to take the measurement by the sensor.
A variable may also be assigned a universally unique identifier (UUID) and a unique variable code.
Many sensors are capable of measuring multiple variables at a single time.
For example, a Meter Hydros 21 is a _sensor_.
It is able to measure 3 _variables_: specific conductance, temperature, and water depth.
The variable named "specificConductance" has _units_ of microsiemens per centimeter (µS/cm) and a _resolution_ of 1 µS/cm.
Each measured variable is explicitly tied to the "parent" sensor that "notifies" the variable when a new value has been measured.
Each calculated variable has a parent function returning a float which is the value for that variable.

The Variable class documentation is here:  <https://envirodiy.github.io/ModularSensors/class_variable.html>

Variables are grouped together into VariableArrays.
The VariableArray class documentation is here:  <https://envirodiy.github.io/ModularSensors/class_variable_array.html>

### Logger<!-- {#library_terminology_logger} -->

A logger is a circuit board with a processor or microcontroller unit (MCU) that can control all functions of the modem and sensors that are attached to it and save the values of all variables measured by those sensors to an attached SD card.
In this library, all loggers are Arduino-style small processor circuit boards.

The Logger class documentation is here:  <https://envirodiy.github.io/ModularSensors/class_logger.html>

### Modem<!-- {#library_terminology_modem} -->

[![Old School Modem](https://upload.wikimedia.org/wikipedia/commons/e/e5/Analogue_modem_-_acoustic_coupler.jpg)](https://en.wikipedia.org/wiki/Modem)

A modem is a [system on a chip](https://en.wikipedia.org/wiki/System_on_a_chip) or [system on a module](https://en.wikipedia.org/wiki/System_on_module) a that can communicate with the logger's MCU and with the world wide web.
This doesn't mean something that makes [beeps and bloops](https://en.wikipedia.org/wiki/Modem#/media/File:Analogue_modem_-_acoustic_coupler.jpg) on the phone line.
The modem SoC or SOM handles all the intricacies and [lower layers](https://www.softwaretestinghelp.com/osi-model-layers/) of internet communication allowing the logger to operate at the top application layer.
A cellular SoC may actually have a much more powerful processor than the logger MCU.
The two communcicate via serial lines.
Within this library, the modem is represented as a loggerModem object.
All loggerModem functions are heavily dependent on the [TinyGSM](https://github.com/EnviroDIY/TinyGSM) library.

The loggerModem class documentation is available here:  <https://envirodiy.github.io/ModularSensors/classlogger_modem.html>

### DataPublisher<!-- {#library_terminology_publiser} -->

Unlike the other components, a dataPublisher object doesn't represent any physical device.
It's an object only in the sense of object oriented programming - not something you could hold.
Within the functioning of the library, the dataPublisher "watches" the logger for new data and correctly formats and sends that data to some online web service.

The dataPublisher class documentation is available here:  <https://envirodiy.github.io/ModularSensors/classdata_publisher.html>

## Library Structure<!-- {#library_terminology_structure} -->

This library is built to fully take advantage of Objecting Oriented Programing (OOP) approaches.
This means there are a number of base abstract classes with virtual functions and then many more daughter classes which inherit the functions of their parents and implement others themselves.

The main classes are implemented in the src directory of the library.
They exactly match the terms defined above.

Each sensor is implemented as a subclass of the "Sensor" class contained in "SensorBase.h".

Each variable is separately implemented as a subclass of the "Variable" class contained in "VariableBase.h".
The variables are tied to the sensor using an "[Observer](https://en.wikipedia.org/wiki/Observer_pattern)" software pattern.

The "VariableArray" class contained in "VariableArray.h" defines the logic for iterating through many variable objects.
The VariableArray class takes advantage of various time stamps within the Sensor class to optimize the timing of communications with many sensors.

The "Logger" class defines functions for sleeping the processor and writing to an SD card.

The dataPublisher again act as observers of the logger.
