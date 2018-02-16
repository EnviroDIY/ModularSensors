.PHONY: travis-build

travis-build:
ifdef PLATFORMIO_CI_ZERO
	platformio ci --lib="." --project-option="lib_ignore = SoftwareSerial_ExtInts, AltSoftSerial" --board=adafruit_feather_m0 --board=sodaq_autonomo
else
	platformio ci --lib="." --project-option="lib_ignore = RTCZero" --board=mayfly --board=uno --board=leonardo --board=megaatmega2560"
endif
