#%%
# import json
import yaml

all_modem_flags = [
    "MS_BUILD_TEST_XBEE_CELLULAR",
    "MS_BUILD_TEST_XBEE_LTE_B",
    "MS_BUILD_TEST_XBEE_3G_B",
    "MS_BUILD_TEST_XBEE_WIFI",
    "MS_BUILD_TEST_ESP8266",
    "MS_BUILD_TEST_BG96",
    "MS_BUILD_TEST_MONARCH",
    "MS_BUILD_TEST_SIM800",
    "MS_BUILD_TEST_SIM7000",
    "MS_BUILD_TEST_SIM7080",
    "MS_BUILD_TEST_S2GB",
    "MS_BUILD_TEST_UBEE_R410M",
    "MS_BUILD_TEST_UBEE_U201",
]
all_sensor_flags = [
    "NO_SENSORS",
    "MS_BUILD_TEST_AM2315",
    "MS_BUILD_TEST_DHT",
    "MS_BUILD_TEST_SQ212",
    "MS_BUILD_TEST_ATLASCO2",
    "MS_BUILD_TEST_ATLASDO",
    "MS_BUILD_TEST_ATLASORP",
    "MS_BUILD_TEST_ATLASPH",
    "MS_BUILD_TEST_ATLASRTD",
    "MS_BUILD_TEST_ATLASEC",
    "MS_BUILD_TEST_BME280",
    "MS_BUILD_TEST_OBS3",
    "MS_BUILD_TEST_ES2",
    "MS_BUILD_TEST_VOLTAGE",
    "MS_BUILD_TEST_MPL115A2",
    "MS_BUILD_TEST_INSITURDO",
    "MS_BUILD_TEST_ACCULEVEL",
    "MS_BUILD_TEST_NANOLEVEL",
    "MS_BUILD_TEST_MAXBOTIX",
    "MS_BUILD_TEST_DS18",
    "MS_BUILD_TEST_MS5803",
    "MS_BUILD_TEST_5TM",
    "MS_BUILD_TEST_CTD",
    "MS_BUILD_TEST_HYDROS21",
    "MS_BUILD_TEST_TEROS11",
    "MS_BUILD_TEST_PALEOTERRA",
    "MS_BUILD_TEST_RAINI2C",
    "MS_BUILD_TEST_TALLY",
    "MS_BUILD_TEST_INA219",
    "MS_BUILD_TEST_CYCLOPS",
    "MS_BUILD_TEST_ANALOGEC",
    "MS_BUILD_TEST_Y504",
    "MS_BUILD_TEST_Y510",
    "MS_BUILD_TEST_Y511",
    "MS_BUILD_TEST_Y514",
    "MS_BUILD_TEST_Y520",
    "MS_BUILD_TEST_Y532",
    "MS_BUILD_TEST_Y533",
    "MS_BUILD_TEST_Y550",
    "MS_BUILD_TEST_Y4000",
    "MS_BUILD_TEST_DOPTO",
]
all_publisher_flags = [
    "MS_BUILD_TEST_MMW",
    "MS_BUILD_TEST_DREAMHOST",
    "MS_BUILD_TEST_THINGSPEAK",
]

matrix_includes = []
for flag in all_modem_flags:
    matrix_includes.append(
        {
            "modemFlag": flag,
            "sensorFlag": all_sensor_flags[0],
            "publisherFlag": all_publisher_flags[0],
        }
    )
for flag in all_sensor_flags[1:]:
    matrix_includes.append(
        {
            "modemFlag": all_modem_flags[0],
            "sensorFlag": flag,
            "publisherFlag": all_publisher_flags[0],
        }
    )
for flag in all_publisher_flags[1:]:
    matrix_includes.append(
        {
            "modemFlag": all_modem_flags[0],
            "sensorFlag": all_sensor_flags[0],
            "publisherFlag": flag,
        }
    )

print(yaml.dump(matrix_includes, sort_keys=False))

# %%
