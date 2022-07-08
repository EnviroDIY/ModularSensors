#!/usr/bin/env python
#%%
# import json
# import yaml
import glob
from pathlib import Path
import json
import shutil
import sys
import re
import subprocess
import tempfile
from platformio.project.config import ProjectConfig
import re
import os
import copy
import numpy as np
import pandas as pd
from itertools import zip_longest

#%%
# Some working directories
# The workspace directory
if "GITHUB_WORKSPACE" in os.environ.keys():
    workspace_dir = os.environ.get("GITHUB_WORKSPACE")
else:
    fileDir = os.path.dirname(os.path.realpath("__file__"))
    workspace_dir = os.path.join(fileDir, "../")
    workspace_dir = os.path.abspath(os.path.realpath(workspace_dir))
# The examples directory
examples_dir = "./examples/"
examples_path = os.path.join(workspace_dir, examples_dir)
examples_path = os.path.abspath(os.path.realpath(examples_path))
# The continuous integration directory
ci_dir = "./continuous_integration/"
ci_path = os.path.join(workspace_dir, ci_dir)
ci_path = os.path.abspath(os.path.realpath(ci_path))

# A directory of files to save and upload as artifacts to use in future jobs
artifact_dir = os.path.join(
    os.path.join(workspace_dir, "continuous_integration_artifacts")
)
if not os.path.exists(artifact_dir):
    os.makedirs(artifact_dir)

#%%
# The locations of some important files

# The massive "menu" example
menu_example_name = "menu_a_la_carte"
menu_file_path = os.path.join(
    os.path.join(examples_path, menu_example_name), menu_example_name + ".ino"
)

# Find all of the non-menu examples
non_menu_examples = [
    f
    for f in os.listdir(examples_path)
    if os.path.isdir(os.path.join(examples_path, f))
    and f not in [".history", "logger_test", menu_example_name]
]

# Arduino CLI configurations
if "GITHUB_WORKSPACE" in os.environ.keys():
    arduino_cli_config = os.path.join(ci_dir, "arduino_cli.yaml")
else:
    arduino_cli_config = os.path.join(ci_dir, "arduino_cli_local.yaml")

# PlatformIO configurations
pio_config_file = os.path.join(ci_path, "platformio.ini")
pio_extra_config_file = os.path.join(ci_path, "platformio_extra_flags.ini")
pio_config = ProjectConfig(pio_config_file)
pio_extra_config = ProjectConfig(pio_extra_config_file)

# Translation between board names on PlatformIO and the Arduino CLI
pio_to_acli = {
    "mayfly": {"fqbn": "EnviroDIY:avr:envirodiy_mayfly"},
    "megaatmega2560": {"fqbn": "arduino:avr:mega"},
    "zeroUSB": {"fqbn": "arduino:samd:mzero_bl"},
    "adafruit_feather_m0": {"fqbn": "adafruit:samd:adafruit_feather_m0"},
    "sodaq_autonomo": {"fqbn": "SODAQ:samd:sodaq_autonomo"},
}


def create_arduino_cli_command(pio_env_name, code_file):
    arduino_command_arguments = [
        "arduino-cli",
        "compile",
        # "--verbose",
        "--warnings",
        "more",
        "--config-file",
        arduino_cli_config,
        "--format",
        "text",
        "--fqbn",
        pio_to_acli[pio_config.get("env:{}".format(pio_env_name), "board")]["fqbn"],
        os.path.join(examples_path, code_file),
    ]
    return " ".join(arduino_command_arguments)


def create_platformio_command(pio_env_file, code_file):
    pio_command_args = [
        "pio",
        "ci",
        # "--verbose",
        "--project-conf",
        pio_env_file,
        os.path.join(examples_path, code_file),
    ]
    return " ".join(pio_command_args)


def snake_to_camel(snake_str):
    crop_strings = ["BUILD_MODEM_", "BUILD_SENSOR_", "BUILD_PUB_", "_PUBLISHER"]
    for crop_string in crop_strings:
        snake_str = snake_str.replace(crop_string, "")
    components = snake_str.strip().split("_")
    # We capitalize the first letter of each component except the first one
    # with the 'title' method and join them together.
    camel_str = "".join(x.title() for x in components)
    if camel_str.startswith("_"):
        return camel_str[1:]
    else:
        return camel_str


def get_example_folder(subfolder_name):
    return os.path.join(examples_path, subfolder_name)


def get_example_filepath(subfolder_name):
    ex_folder = get_example_folder(subfolder_name)
    ex_file = os.path.join(ex_folder, subfolder_name + ".ino")
    return ex_file


#%%
# set up outputs
arduino_job_matrix = []
pio_job_matrix = []

#%%
# Create job info for the basic examples
# for PlatformIO, use one job per example
for example in non_menu_examples:
    pio_command = create_platformio_command(pio_config_file, example)
    job_name = "{} on all boards".format(snake_to_camel(example))
    job_id = "{}_pio".format(example)
    pio_job_matrix.append(
        {"job_name": job_name, "job_id": job_id, "command": pio_command}
    )

# for the Ardunio CLI, use one job per board
for pio_env in pio_config.envs():
    job_name = "all examples on {}".format(snake_to_camel(pio_env))
    job_id = "{}_arduino".format(pio_env)
    arduino_commands = []
    for example in non_menu_examples:
        arduino_command = create_arduino_cli_command(pio_env, example)
        arduino_commands.append(arduino_command)
    arduino_job_matrix.append(
        {
            "job_name": job_name,
            "job_id": job_id,
            "command": " && ".join(arduino_commands),
        }
    )


#%%
# Helper functions for preparing the menu-a-la-carte example for building
def modify_example_filename(example_subfolder, in_file_defines):

    if len(in_file_defines) == 0:
        return get_example_folder(example_subfolder), get_example_filepath(
            example_subfolder
        )

    suffix = "_".join([snake_to_camel(flag) for flag in in_file_defines])
    preped_folder_name = "{}_{}".format(example_subfolder, suffix).replace(
        "_a_la_carte", ""
    )
    prepped_ex_folder = os.path.join(artifact_dir, preped_folder_name)
    prepped_ex_file = os.path.join(prepped_ex_folder, preped_folder_name + ".ino")

    return prepped_ex_folder, prepped_ex_file


def prepare_example(example_subfolder, in_file_defines):
    if len(in_file_defines) == 0:
        return get_example_folder(example_subfolder), get_example_filepath(
            example_subfolder
        )

    ex_file = open(get_example_filepath(example_subfolder), "r")
    as_written = ex_file.read()
    ex_file.close()

    prepped_ex_folder, prepped_ex_file = modify_example_filename(
        example_subfolder, in_file_defines
    )
    if not os.path.exists(prepped_ex_folder):
        os.makedirs(prepped_ex_folder)

    prepared_example = open(
        prepped_ex_file,
        "w+",
    )
    for flag in in_file_defines:
        prepared_example.write("#ifndef {}\n".format(flag.split("=")[0]))
        prepared_example.write(
            "#define {} {}\n".format(
                flag.split("=")[0],
                flag.split("=")[1] if "=" in flag else "",
            )
        )
        prepared_example.write("#endif\n")

    prepared_example.write(as_written)
    prepared_example.close()

    return prepped_ex_folder, prepped_ex_file


def extend_pio_config(added_envs):

    if len(added_envs) == 0:
        return pio_config_file

    new_config = copy.deepcopy(pio_config)
    new_config_path = os.path.join(
        artifact_dir, "test_ci_" + "_".join(added_envs) + ".ini"
    )

    for added_env in added_envs:
        env_key = "env:{}".format(added_env)
        new_config._parser.add_section(env_key)
        for option in pio_extra_config.options(env_key):
            new_config.set(env_key, option, pio_extra_config.get(env_key, option))

    new_config.save(new_config_path)
    return new_config_path


#%% read build flags out of the menu-a-la-cart example
# Pattern for flags in the menu-a-la-cart example
pattern = re.compile(
    "^(?:#if|#elif) defined (?P<flag1>BUILD_\w+)((?:[\\s\\n\\\\]*?\|\|[\\s\\n\\\\]*defined[\\s\\n\\\\]*?)(?P<flag_last>BUILD_\w+))*",
    re.MULTILINE,
)

# lists for the flags
all_modem_flags = [
    "BUILD_MODEM_SIM_COM_SIM7080",
]
all_sensor_flags = [
    "NO_SENSORS",
]
all_publisher_flags = [
    "BUILD_PUB_ENVIRO_DIY_PUBLISHER",
]


# Open the file and read it
textfile = open(menu_file_path, "r")
filetext = textfile.read()
textfile.close()

# find matches and add them to the lists
for match in re.finditer(pattern, filetext):
    # print(match.group("flag1"))
    if (
        "SENSOR" in match.group("flag1")
        and match.group("flag1") not in all_sensor_flags
    ):
        all_sensor_flags.append(match.group("flag1"))
    if "MODEM" in match.group("flag1") and match.group("flag1") not in all_modem_flags:
        all_modem_flags.append(match.group("flag1"))
    if (
        "PUB" in match.group("flag1")
        and match.group("flag1") not in all_publisher_flags
    ):
        all_publisher_flags.append(match.group("flag1"))

#%% Create a matrix of build flags for that example
menu_flag_matrix = []
for modem_flag in all_modem_flags:
    menu_flag_matrix.append([modem_flag, all_sensor_flags[0], all_publisher_flags[0]])
for sensor_flag in all_sensor_flags[1:]:
    menu_flag_matrix.append([all_modem_flags[0], sensor_flag, all_publisher_flags[0]])
for publisher_flag in all_publisher_flags[1:]:
    menu_flag_matrix.append([all_modem_flags[0], all_sensor_flags[0], publisher_flag])

#%%
# Prepare all of the examples by adding flags to the code
for flag_set in menu_flag_matrix:
    prepped_ex_folder, prepped_ex_file = prepare_example(menu_example_name, flag_set)

for pio_env in pio_config.envs():
    arduino_commands = []
    job_name = "all modems"
    job_id = "Modems Arduino CLI"
    for modem_flag in all_modem_flags:
        used_modem = snake_to_camel(modem_flag)
        prepped_ex_folder, _ = modify_example_filename(
            menu_example_name, [modem_flag, all_sensor_flags[0], all_publisher_flags[0]]
        )
        arduino_commands.append("echo ::group::{}".format(used_modem))
        arduino_command = create_arduino_cli_command(pio_env, prepped_ex_folder)
        arduino_commands.append(arduino_command)
        arduino_commands.append("echo ::endgroup::")
    arduino_job_matrix.append(
        {
            "job_name": job_name,
            "job_id": job_id,
            "command": " && ".join(arduino_commands),
        }
    )

    arduino_commands = []
    job_name = "all sensor"
    job_id = "Modems Arduino CLI"
    for sensor_flag in all_sensor_flags[1:]:
        used_sensor = snake_to_camel(sensor_flag)
        prepped_ex_folder, _ = modify_example_filename(
            menu_example_name, [all_modem_flags[0], sensor_flag, all_publisher_flags[0]]
        )
        arduino_commands.append("echo ::group::{}".format(used_sensor))
        arduino_command = create_arduino_cli_command(pio_env, prepped_ex_folder)
        arduino_commands.append(arduino_command)
        arduino_commands.append("echo ::endgroup::")
    arduino_job_matrix.append(
        {
            "job_name": job_name,
            "job_id": job_id,
            "command": " && ".join(arduino_commands),
        }
    )

    arduino_commands = []
    job_name = "all publishers"
    job_id = "Modems Arduino CLI"
    for publisher_flag in all_publisher_flags[1:]:
        used_pub = snake_to_camel(publisher_flag)
        prepped_ex_folder, _ = modify_example_filename(
            menu_example_name, [all_modem_flags[0], all_sensor_flags[0], publisher_flag]
        )
        arduino_commands.append("echo ::group::{}".format(used_pub))
        arduino_command = create_arduino_cli_command(pio_env, prepped_ex_folder)
        arduino_commands.append(arduino_command)
        arduino_commands.append("echo ::endgroup::")
    arduino_job_matrix.append(
        {"job_name": job_name, "job_id": job_id, "command": "\n".join(arduino_commands)}
    )

for flag_set in menu_flag_matrix:
    used_modem = snake_to_camel(flag_set[0])
    used_sensor = snake_to_camel(flag_set[1])
    used_pub = snake_to_camel(flag_set[2])

    job_name = "{}, {}, and {}".format(used_sensor, used_modem, used_pub)
    job_id = "{}-{}-{} PlatformIO".format(used_sensor, used_modem, used_pub)
    prepped_ex_folder, _ = modify_example_filename(menu_example_name, flag_set)

    pio_build_config = pio_config_file
    if (
        used_sensor.startswith("Decagon")
        or used_sensor.startswith("Meter")
        or used_sensor
        in [
            "CampbellClariVue10",
            "CampbellRainvue10",
            "InSituRdo",
            "InSituTrollSdi12A",
            "ZebraTechDOpto",
        ]
    ):
        pio_build_config = extend_pio_config(["sdi12_non_concurrent"])
    if used_sensor in ["PaleoTerraRedox", "RainCounterI2C"]:
        pio_build_config = extend_pio_config(["software_wire"])
    if used_sensor in ["ApogeeSq212", "CampbellObs3", "Tiads1X15", "TurnerCyclops"]:
        pio_build_config = extend_pio_config(["ads1015"])
    if used_sensor in ["MaxBotixSonar", "YosemitechY504"]:
        pio_build_config = extend_pio_config(
            ["AltSoftSerial", "NeoSWSerial", "software_serial"]
        )

    pio_command = create_platformio_command(pio_build_config, prepped_ex_folder)
    pio_job_matrix.append(
        {"job_name": job_name, "job_id": job_id, "command": pio_command}
    )

#%%
# Write out output
print("::set-output name=arduino_job_matrix::{}".format(json.dumps(arduino_job_matrix)))
if "GITHUB_WORKSPACE" not in os.environ.keys():
    json_out = open(os.path.join(artifact_dir, "arduino_job_matrix.json"), "w+")
    json.dump(arduino_job_matrix, json_out, indent=2)
    json_out.close()
print("::set-output name=pio_job_matrix::{}".format(json.dumps(pio_job_matrix)))
if "GITHUB_WORKSPACE" not in os.environ.keys():
    json_out = open(os.path.join(artifact_dir, "pio_job_matrix.json"), "w+")
    json.dump(pio_job_matrix, json_out, indent=2)
    json_out.close()


#%%
if "GITHUB_WORKSPACE" not in os.environ.keys():
    try:
        shutil.rmtree(artifact_dir)
    except:
        pass
