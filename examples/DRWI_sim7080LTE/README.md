
Example sketch for using the EnviroDIY sim7080 LTE cellular module with an EnviroDIY Mayfly Data Logger. 


The exact hardware configuration used in this example:
 * Mayfly v1.0 board
 * EnviroDIY sim7080 LTE module (with Hologram SIM card)
 * Hydros21 CTD sensor
 * Campbell Scientific OBS3+ Turbidity sensor

An EnviroDIY LTE sim7080 module can be used with the older Mayfly v0.5b boards if you change line 101 (for modemVccPin) from 18 to -1.
This is because the Mayfly v1.0 board has a separate 3.3v regulator to power the Bee socket and is controlled by turning pin 18 on or off.
Mayfly v0.5b has the Bee socket constantly powered, therefore using "-1" is the proper setting for that line of code.

