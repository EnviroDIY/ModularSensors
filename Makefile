.PHONY: travis-build

travis-build:
	# Get the current branch
	export BRANCH=$(if [ "$TRAVIS_PULL_REQUEST" == "false" ]; then echo $TRAVIS_BRANCH; else echo $TRAVIS_PULL_REQUEST_BRANCH; fi)
    echo "TRAVIS_BRANCH=$TRAVIS_BRANCH, PR=$PR, BRANCH=$BRANCH"
	# Remove src files to emulate a user library installation
	git rm library.json
	git rm library.properties
	git rm -r src
	# Install PlatformIO (this should be cached!)
	pip install -U platformio
	pio upgrade
	# Install this library
	pio lib -g install https://github.com/EnviroDIY/ModularSensors.git#$BRANCH
	# Install serial libraries needed for most examples
	pio lib -g install https://github.com/PaulStoffregen/AltSoftSerial.git  # #73, but need the git until Paul S. has a new release
	pio lib -g install https://github.com/EnviroDIY/SoftwaterSerial_ExternalInts.git
	pio update
ifdef PLATFORMIO_CI_ZERO
	platformio ci --project-option="lib_ignore = SoftwareSerial_ExtInts, AltSoftSerial, SoftI2CMaster"  --project-option="lib_ldf_mode = deep" --board=adafruit_feather_m0
else
	platformio ci --project-option="lib_ignore = RTCZero"  --project-option="lib_ldf_mode = deep" --board=mayfly
endif
