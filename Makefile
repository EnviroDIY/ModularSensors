.PHONY: travis-build

travis-build:
	git rm library.json
	pip install -U platformio
	pio upgrade
	pio lib -g install 2079  # DS3231
	pio lib -g install 311  # EnableInterrupt
	pio lib -g install 322  # SdFat
	pio lib -g install 344  # soligen2010 fork of Adafruit ADS1X15
	pio lib -g install 31  # Adafruit Unified Sensor
	pio lib -g install 19  # Adafruit DHT sensor library
	pio lib -g install 773  # Adafruit AM2315
	pio lib -g install 166  # Adafruit BME280 Library
	pio lib -g install 406  # Adafruit MPL115A2
	pio lib -g install 1  # OneWire
	pio lib -g install 54  # Dallas Temperature
	pio lib -g install https://github.com/EnviroDIY/Arduino-SDI-12.git#ExtInts
	pio lib -g install https://github.com/EnviroDIY/TinyGSM.git
	pio lib -g install 1824  # SensorModbusMaster
	pio lib -g install 2078  # YosemitechModbus
	pio lib -g install 5431 # MS5803
	pio lib -g install 5439 # KellerModbus
	pio lib -g install 873  # RTCZero
	pio lib -g install https://github.com/PaulStoffregen/AltSoftSerial.git  # #73, but need the git until Paul S. has a new release
	pio lib -g install https://github.com/EnviroDIY/SoftwaterSerial_ExternalInts.git
	pio update
ifdef PLATFORMIO_CI_ZERO
	platformio ci --lib="." --project-option="lib_ignore = SoftwareSerial_ExtInts, AltSoftSerial, SoftI2CMaster"  --project-option="lib_ldf_mode = deep" --board=adafruit_feather_m0
else
	platformio ci --lib="." --project-option="lib_ignore = RTCZero"  --project-option="lib_ldf_mode = deep" --board=mayfly
endif
