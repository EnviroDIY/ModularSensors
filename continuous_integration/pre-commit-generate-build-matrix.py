#%%
# import json
# import yaml
from pathlib import Path
from ruamel.yaml import YAML
import sys
import re
import subprocess

yaml = YAML()  # default, if not specfied, is 'rt' (round-trip)
yaml.indent(mapping=2, sequence=4, offset=2)
yaml.preserve_quotes = True
build_file = "C:\\Users\\sdamiano\\Documents\\GitHub\\EnviroDIY\\ModularSensors\\.github\\workflows\\build_menu.yaml"
menu_file = "C:\\Users\sdamiano\\Documents\\GitHub\\EnviroDIY\\ModularSensors\\examples\\menu_a_la_carte\\menu_a_la_carte.ino"

#%% read the current build github action and matrix
with open(build_file, "r") as in_file:
    menu_builder = yaml.load(in_file)

# set flags
all_modem_flags = [
    "SIM_COM_SIM7080",
]
all_sensor_flags = [
    "NO_SENSORS",
]
all_publisher_flags = [
    "ENVIRO_DIY_PUBLISHER",
]

#%% Read flags out of the menu example
pattern = re.compile(
    "^(?:#if|#elif) defined (?P<flag1>BUILD_\w+)((?:[\\s\\n\\\\]*?\|\|[\\s\\n\\\\]*defined[\\s\\n\\\\]*?)(?P<flag_last>BUILD_\w+))*",
    re.MULTILINE,
)
textfile = open(menu_file, "r")
filetext = textfile.read()
textfile.close()

for match in re.finditer(pattern, filetext):
    print(match.group("flag1"))
    if (
        "SENSOR" in match.group("flag1")
        and match.group("flag1") not in all_sensor_flags
    ):
        all_sensor_flags.append(match.group("flag1").replace("BUILD_SENSOR_", ""))
    if "MODEM" in match.group("flag1") and match.group("flag1") not in all_modem_flags:
        all_modem_flags.append(match.group("flag1").replace("BUILD_MODEM_", ""))
    if (
        "PUB" in match.group("flag1")
        and match.group("flag1") not in all_publisher_flags
    ):
        all_publisher_flags.append(match.group("flag1").replace("BUILD_PUB_", "").replace("_PUBLISHER", ""))

#%% Create the matrix
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
            "sensorFlag": flag,
            "modemFlag": all_modem_flags[0],
            "publisherFlag": all_publisher_flags[0],
        }
    )
for flag in all_publisher_flags[1:]:
    matrix_includes.append(
        {
            "publisherFlag": flag,
            "sensorFlag": all_sensor_flags[0],
            "modemFlag": all_modem_flags[0],
        }
    )

# print(yaml.dump(matrix_includes, sort_keys=False))
menu_builder["jobs"]["build"]["strategy"]["matrix"]["include"] = matrix_includes

#%% Write out the new build action yaml
with open(build_file, "w",) as out_file:
    yaml.dump(menu_builder, out_file)

# %% Commit changes to the action
subprocess.run("git add .github\\workflows\\build_menu.yaml")
