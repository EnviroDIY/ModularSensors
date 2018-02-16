.PHONY: travis-build

travis-build:
ifdef PLATFORMIO_CI_ZERO
	platformio ci --lib="." --project-option="lib_ignore = SoftwareSerial_ExtInts, AltSoftSerial"  --project-option="lib_ldf_mode = deep" --board=adafruit_feather_m0 --board=zero
else
	platformio ci --lib="." --project-option="lib_ignore = RTCZero"  --project-option="lib_ldf_mode = deep" --board=mayfly --board=uno --board=leonardo --board=megaatmega2560
endif
