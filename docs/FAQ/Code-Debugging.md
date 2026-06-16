# In-Library Debugging<!--! {#page_code_debugging} -->

There are a huge number of **code** debugging printouts available in the library, if you enable them.
This will (sometimes dramatically) increase the number of statements going out to the debugging serial port.
These can help to troubleshoot the various components of the library, including sensor communication, modem communication, and variable array functions.

> [!NOTE]
> The debugging provided in the library is primarily for debugging communication and the internals of the library.
> It will probably *not* be as helpful in troubleshooting a failing sensor.
> All debugging is intended to be used only in **testing** environments.
> Disable debugging for field deployments.

All of the component debugging can be enabled using the [ModSensorDebugConfig.h](https://github.com/EnviroDIY/ModularSensors/blob/master/src/ModSensorDebugConfig.h) file.
Near the top of the file, you can select the serial port you want debugging information to be sent to, if you do not want to use the default "Serial" for your device.
Below that, enable each component of interest by removing the `\\` comment markers in front of each line.

For example, to enable debugging of SDI-12 communication, find the line:

```cpp
// #define MS_SDI12SENSORS_DEBUG
```

and replace it with:

```cpp
#define MS_SDI12SENSORS_DEBUG
```

For some components, including SDI-12 and all modems, there is also a "deep" debugging that can be enabled for even more detailed printouts.

> [!IMPORTANT]
> Because the debugging configuration file is part of the Modular Sensors *library* and not part of your individual program or sketch, the file will be reset any time you update the library.

## Arduino IDE<!--! {#code_debugging_arduino} -->

The debugging configuration file will be found in a subdirectory of your [library folder](https://support.arduino.cc/hc/en-us/articles/4415103213714-Find-sketches-libraries-board-cores-and-other-files-on-your-computer).
Within your library folder, navigate down to `ModularSensors/src/ModSensorDebugConfig.h`.
On a Windows PC, the full path for the file is probably `C:\Users\{your_user_name}\Documents\Arduino\libraries\EnviroDIY_ModularSensors\src\ModSensorDebugConfig.h`.

> [!WARNING]
> The Arduino IDE uses the same library files for every single sketch compiled on your computer.
> This means that you any debugging you enable will be enabled for all programs you compile after it is enabled.
> You should get in the habit of opening the file, modifying, saving, compiling, undoing the modifications, and re-saving the file to avoid having debugging enabled in production.

## PlatformIO<!--! {#code_debugging_pio} -->

If you are using PlatformIO, you can usually find the ModSensorDebugConfig.h file in your project libdeps folder, most often {`project_folder}/.pio/libdeps/{environment_name}/ModularSensors/src/ModSensorDebugConfig.h`.
With PlatformIO and other build systems that allow you to define build flags, you can set also enable debugging through [build flags in your platformio.ini file](https://docs.platformio.org/en/latest/projectconf/sections/env/options/build/build_flags.html).

To change the Serial port for debugging output, use these build flags:

- `-D MS_OUTPUT=SerialX` to define a serial or other stream to use for all printout, debugging or otherwise
- `-D MS_2ND_OUTPUT=SerialX` to define a second serial to output identical debugging output to
  - This is very helpful for boards that use a built-in USB adapter.
    Assigning a secondary output that can be attached to a secondary TTL-to-USB adapter can make debugging much easier.
- `-D MS_SILENT` to suppress *ALL* output.

Other component debugging can be enabled using flags similar to `-D MS_SDI12SENSORS_DEBUG`.
See the debugging config file for the complete list of possible flags.
