# Geographical Customizations seperate file, to log data to EnviroDIY/Monitor My Watershed

This sketch uses 
a) ms_cfg.ini where  all the MMW UUID keys are added. This can be customized per Mayfly node.  
b) sensors to provide a simple test bed for verifying access to log to https://monitormywatershed.org/ .
c) mscfg.h - that has all the constants for .ino file in one place.

This code can be built against current (and future releases), as is, and the same microSD card used for configuring a test MMW acces.

Other sensors capability will be added in time, and the sensors configuration will be from ms_cfg.ini/SD card.