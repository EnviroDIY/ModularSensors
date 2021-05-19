#!/bin/sh

# exit when any command fails
set -e

# keep track of the last executed command
# trap 'last_command=$current_command; current_command=$BASH_COMMAND' DEBUG
# echo an error message before exiting
# trap 'echo "\"${last_command}\" command filed with exit code $?."' EXIT

mkdir temp
mkdir temp/menu_a_la_carte

for modemFlag in \
    MS_BUILD_TEST_XBEE_CELLULAR \
    MS_BUILD_TEST_XBEE_LTE_B \
    MS_BUILD_TEST_XBEE_3G_B \
    MS_BUILD_TEST_XBEE_WIFI \
    MS_BUILD_TEST_ESP8266 \
    MS_BUILD_TEST_BG96 \
    MS_BUILD_TEST_MONARCH \
    MS_BUILD_TEST_SIM800 \
    MS_BUILD_TEST_SIM7000 \
    MS_BUILD_TEST_SIM7080 \
    MS_BUILD_TEST_S2GB \
    MS_BUILD_TEST_UBEE_R410M \
    MS_BUILD_TEST_UBEE_U201

do
    echo "\e[36m----------------------------------------------------------------------------\e[0m"
    echo "\e[36m----------------------------------------------------------------------------\e[0m"
    echo "Modifying source for \e[32m$modemFlag\e[0m"
    echo "\e[36m----------------------------------------------------------------------------\e[0m"
    echo "\e[36m----------------------------------------------------------------------------\e[0m"
    sed "1s/^/#define $modemFlag\n#define MS_BUILD_TEST_PRE_NAMED_VARS\n/" examples/menu_a_la_carte/menu_a_la_carte.ino > temp/menu_a_la_carte/menu_a_la_carte.ino

    echo "First few lines of source"
    head temp/menu_a_la_carte/menu_a_la_carte.ino

    # Test on several platforms using Platform IO
    platformio run --project-conf="continuous_integration/platformio.ini"

    # Only test on the Mayfly with the Arduino CLI
    arduino-cli --config-file continuous_integration/arduino_cli.yaml compile --clean --build-property "build.extra_flags=-DNEOSWSERIAL_EXTERNAL_PCINT" --fqbn EnviroDIY:avr:envirodiy_mayfly temp/menu_a_la_carte/
done

for sensorFlag in \
    MS_BUILD_TEST_AM2315 \
    MS_BUILD_TEST_DHT \
    MS_BUILD_TEST_SQ212 \
    MS_BUILD_TEST_ATLASCO2 \
    MS_BUILD_TEST_ATLASDO \
    MS_BUILD_TEST_ATLASORP \
    MS_BUILD_TEST_ATLASPH \
    MS_BUILD_TEST_ATLASRTD \
    MS_BUILD_TEST_ATLASEC \
    MS_BUILD_TEST_BME280 \
    MS_BUILD_TEST_OBS3 \
    MS_BUILD_TEST_ES2 \
    MS_BUILD_TEST_VOLTAGE \
    MS_BUILD_TEST_MPL115A2 \
    MS_BUILD_TEST_INSITURDO \
    MS_BUILD_TEST_ACCULEVEL \
    MS_BUILD_TEST_NANOLEVEL \
    MS_BUILD_TEST_MAXBOTIX \
    MS_BUILD_TEST_DS18 \
    MS_BUILD_TEST_MS5803 \
    MS_BUILD_TEST_5TM \
    MS_BUILD_TEST_CTD \
    MS_BUILD_TEST_HYDROS21 \
    MS_BUILD_TEST_TEROS11 \
    MS_BUILD_TEST_PALEOTERRA \
    MS_BUILD_TEST_RAINI2C \
    MS_BUILD_TEST_TALLY \
    MS_BUILD_TEST_INA219 \
    MS_BUILD_TEST_CYCLOPS \
    MS_BUILD_TEST_ANALOGEC \
    MS_BUILD_TEST_Y504 \
    MS_BUILD_TEST_Y510 \
    MS_BUILD_TEST_Y511 \
    MS_BUILD_TEST_Y514 \
    MS_BUILD_TEST_Y520 \
    MS_BUILD_TEST_Y532 \
    MS_BUILD_TEST_Y533 \
    MS_BUILD_TEST_Y550 \
    MS_BUILD_TEST_Y4000 \
    MS_BUILD_TEST_DOPTO \

do
    echo "\e[36m----------------------------------------------------------------------------\e[0m"
    echo "\e[36m----------------------------------------------------------------------------\e[0m"
    echo "Modifying source for \e[32m$sensorFlag\e[0m"
    echo "\e[36m----------------------------------------------------------------------------\e[0m"
    echo "\e[36m----------------------------------------------------------------------------\e[0m"
    sed "1s/^/#define MS_BUILD_TEST_XBEE_CELLULAR\n#define MS_BUILD_TEST_PRE_NAMED_VARS\n#define $sensorFlag\n/" examples/menu_a_la_carte/menu_a_la_carte.ino > temp/menu_a_la_carte/menu_a_la_carte.ino

    echo  "First few lines of source"
    head temp/menu_a_la_carte/menu_a_la_carte.ino

    # Test on several platforms using Platform IO
    platformio run --project-conf="continuous_integration/platformio.ini"

    # Only test on the Mayfly with the Arduino CLI
    arduino-cli --config-file continuous_integration/arduino_cli.yaml compile --clean --build-property "build.extra_flags=-DNEOSWSERIAL_EXTERNAL_PCINT" --fqbn EnviroDIY:avr:envirodiy_mayfly temp/menu_a_la_carte/
done

for publisherFlag in \
    MS_BUILD_TEST_MMW \
    MS_BUILD_TEST_DREAMHOST \
    MS_BUILD_TEST_THINGSPEAK \

do
    echo "\e[36m----------------------------------------------------------------------------\e[0m"
    echo "\e[36m----------------------------------------------------------------------------\e[0m"
    echo "Modifying source for \e[32m$publisherFlag\e[0m"
    echo "\e[36m----------------------------------------------------------------------------\e[0m"
    echo "\e[36m----------------------------------------------------------------------------\e[0m"
    sed "1s/^/#define MS_BUILD_TEST_XBEE_CELLULAR\n#define MS_BUILD_TEST_PRE_NAMED_VARS\n#define $publisherFlag\n/" examples/menu_a_la_carte/menu_a_la_carte.ino > temp/menu_a_la_carte/menu_a_la_carte.ino

    echo "First few lines of source"
    head temp/menu_a_la_carte/menu_a_la_carte.ino

    # Test on several platforms using Platform IO
    platformio run --project-conf="continuous_integration/platformio.ini"

    # Only test on the Mayfly with the Arduino CLI
    arduino-cli --config-file continuous_integration/arduino_cli.yaml compile --clean --build-property "build.extra_flags=-DNEOSWSERIAL_EXTERNAL_PCINT" --fqbn EnviroDIY:avr:envirodiy_mayfly temp/menu_a_la_carte/
done