.PHONY: travis-build

travis-build:
	git rm library.json
	pip install -U platformio
	pio upgrade
	pio lib -g install 2079@1.3.2  # DS3231
	pio lib -g install 311@1.0.0  # EnableInterrupt
	pio lib -g install 322@1.0.7  # SdFat
	pio lib -g install 344@1.2.0  # soligen2010 fork of Adafruit ADS1X15
	pio lib -g install 31@1.0.2  # Adafruit Unified Sensor
	pio lib -g install 773@1.0.1  # Adafruit AM2315
	pio lib -g install 166@1.0.7  # Adafruit BME280 Library
	pio lib -g install 19@1.3.0  # Adafruit DHT sensor library
	pio lib -g install 406@1.0.0  # Adafruit MPL115A2
	pio lib -g install 1@2.3.4  # OneWire
	pio lib -g install 54@3.8.0  # Dallas Temperature
	pio lib -g install 1485@1.3.4 # SDI-12
	pio lib -g install 5439@0.1.1 # KellerModbus
	pio lib -g install 5431@0.1.2 # MS5803
	pio lib -g install 873@1.5.2  # RTCZero
	pio lib -g install 1824@0.6.4  # SensorModbusMaster
	pio lib -g install https://github.com/EnviroDIY/TinyGSM.git#3b971ba75990e82536a250f72e637700ff771934
	pio lib -g install 2078@0.2.1  # YosemitechModbus
	pio lib -g install https://github.com/PaulStoffregen/AltSoftSerial.git  # #73, but need the git until Paul S. has a new release
	pio lib -g install https://github.com/EnviroDIY/SoftwaterSerial_ExternalInts.git
	pio update
ifdef PLATFORMIO_CI_ZERO
	platformio ci --lib="." --project-option="lib_ignore = SoftwareSerial_ExtInts, AltSoftSerial, SoftI2CMaster"  --project-option="lib_ldf_mode = deep" --board=adafruit_feather_m0
else
	platformio ci --lib="." --project-option="lib_ignore = RTCZero"  --project-option="lib_ldf_mode = deep" --board=mayfly
endif
