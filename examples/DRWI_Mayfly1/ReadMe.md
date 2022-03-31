# DRWI Sites with a Mayfly 1.x and EnviroDIY LTE Bees <!-- {#example_drwi_mayfly1} -->
Example sketch for using the EnviroDIY SIM7080G LTE cellular module with an EnviroDIY Mayfly Data Logger.

The exact hardware configuration used in this example:
 * Mayfly v1.1 board
 * EnviroDIY SIM7080 LTE module (with Hologram SIM card)
 * Hydros21 CTD sensor

An EnviroDIY LTE SIM7080 module can be used with the older Mayfly v0.5b boards if you change line 101 (for modemVccPin) from 18 to -1.
This is because the Mayfly v1.x board has a separate 3.3v regulator to power the Bee socket and is controlled by turning pin 18 on or off.
Mayfly v0.5b has the Bee socket constantly powered, therefore using "-1" is the proper setting for that line of code.

The EnviroDIY LTE SIM7080 module includes 2 antennas in the package.  The small thin one is the cellular antenna, and should be connected to the socket labeled "CELL".  The thicker block is the GPS antenna, and should be connected to the "GPS" socket, but only if you intend to use the GPS functionality of the module.  ModularSensors does not currently suport GPS functionality, but other libraries such as TinyGPS can work with the SIM7080 module.

The included cell antenna works best in high-signal-strength areas.  For most remote areas and logger deployments, we suggest a larger LTE antenna, like the W3907B0100
from PulseLarsen (Digikey 1837-1003-ND or Mouser 673-W3907B0100)

_______

[//]: # ( @tableofcontents )

[//]: # ( @m_footernavigation )

[//]: # ( Start GitHub Only )
- [DRWI Sites with a Mayfly 1.x and EnviroDIY LTE Bees](#drwi-sites-with-a-mayfly-1x-and-envirodiy-lte-bees)
- [Unique Features of the DRWI Mayfly 1.x LTE Example](#unique-features-of-the-drwi-mayfly-1x-lte-example)

[//]: # ( End GitHub Only )

_______

# Unique Features of the DRWI Mayfly 1.x LTE Example <!-- {#example_drwi_mayfly1_unique} -->
- Specifically for sites within the Delaware River Watershed Initiative.
- Uses a EnviroDIY LTE Bee based on the SIMCom SIM7080G


[//]: # ( @section example_drwi_mayfly1_pio_config PlatformIO Configuration )

[//]: # ( @include{lineno} DRWI_Mayfly1/platformio.ini )

[//]: # ( @section example_drwi_mayfly1_code The Complete Code )

[//]: # ( @include{lineno} DRWI_Mayfly1/DRWI_Mayfly1.ino )
