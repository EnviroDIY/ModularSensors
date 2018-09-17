.PHONY: travis-build

travis-build:
ifdef PLATFORMIO_CI_ZERO
	platformio ci -v --lib="." --project-option="lib_ignore = SoftwareSerial_ExtInts, AltSoftSerial, SoftI2CMaster"  --project-option="lib_ldf_mode = deep" --board=adafruit_feather_m0
else
	platformio ci -v --lib="." --project-option="lib_ignore = RTCZero"  --project-option="lib_ldf_mode = deep" --board=mayfly
endif
