#!/usr/bin/env python
"""
Custom matrix builder for ModularSensors step 3 - Build Matrix.

This script builds the ModularSensors-specific job matrix by:
1. Reading the ModSensorConfig.h file from the src directory
2. Reading build flags from the menu example file
3. Assembling the job matrix with ModularSensors-specific combinations
4. Returning the final filtered matrix

This is designed to be called from the CI pipeline as a custom builder.
See: https://github.com/EnviroDIY/workflows
"""

import copy
import os
import re
import json

# %%
# Import helper functions from the CI pipeline
from build_utils import dict_product, remove_nested_duplicates

# %%
# ModularSensors-specific configuration
ms_config_file = "ModSensorConfig.h"


def build_custom_matrix(config: dict) -> list[dict]:
    """
    Build the ModularSensors-specific job matrix.

    This is the main entry point called by the CI pipeline (3_build_matrix.py).
    """

    # %%
    # Extract config values
    workspace_path = config.get("workspace_path", os.getcwd())
    examples_path = config.get(
        "examples_path", os.path.join(workspace_path, "examples")
    )
    compiler_list = config.get("compiler_list", ["arduino-cli", "pio"])
    build_envs = config.get("build_envs", [])
    build_fqbns = config.get("build_fqbns", [])
    boards = build_envs + build_fqbns

    print("=== ModularSensors Custom Matrix Builder ===")

    # %%
    # Read build flags from the menu-a-la-carte example
    menu_example_name = "menu_a_la_carte"
    menu_file_path = os.path.join(
        examples_path, menu_example_name, menu_example_name + ".ino"
    )
    menu_example_name = os.path.join("examples", "menu_a_la_carte")

    # Pattern for flags in the menu-a-la-cart example
    pattern = re.compile(
        r"(?:#if|#elif) defined[\s\(](?P<flag1>BUILD_\w+)(?:(?:[\s\n\\\)]*?\|\|[\s\n\\]*defined[\s\n\\\(]*?)(?P<flag_last>BUILD_\w+))*",
        re.MULTILINE,
    )

    # Lists for the inline defines to specify which sections of the menu to build
    ignored_flags = ["BUILD_TEST_SKYWIRE", "BUILD_MODBUS_SENSOR"]
    all_modem_flags = [
        "BUILD_MODEM_SIM_COM_SIM7080",
    ]
    all_sensor_flags = [
        "NO_SENSORS",
    ]
    all_publisher_flags = [
        "BUILD_PUB_MONITOR_MY_WATERSHED_PUBLISHER",
    ]
    all_other_flags = []

    serial_flags = [
        "BUILD_TEST_ALTSOFTSERIAL",
        "BUILD_TEST_NEOSWSERIAL",
        "BUILD_TEST_SOFTSERIAL",
    ]
    array_flags = [
        "BUILD_TEST_PRE_NAMED_VARS",
        "BUILD_TEST_CREATE_IN_ARRAY",
        "BUILD_TEST_SEPARATE_UUIDS",
    ]
    loop_flags = [
        "BUILD_TEST_SIMPLE_LOOP",
        "BUILD_TEST_COMPLEX_LOOP",
    ]
    # NOTE: the clock must be set via compiler flag or config
    clock_flags = ["-D MS_USE_RV8803", "-D MS_USE_DS3231", "-D MS_USE_RTC_ZERO"]
    compiler_flags = [[]]

    # Open the file and read it
    try:
        with open(menu_file_path, "r") as textfile:
            filetext = textfile.read()
    except FileNotFoundError as error:
        raise FileNotFoundError(f"Menu file not found at {menu_file_path}") from error

    # Find matches and add them to the lists
    for match in re.finditer(pattern, filetext):
        for match_flag in match.groups():
            if match_flag is None:
                continue
            if (
                "_SENSOR_" in match_flag
                and match_flag not in all_sensor_flags + ignored_flags
            ):
                all_sensor_flags.append(match_flag)
            elif (
                "_MODEM_" in match_flag
                and match_flag not in all_modem_flags + ignored_flags
            ):
                all_modem_flags.append(match_flag)
            elif (
                "_PUB_" in match_flag
                and match_flag not in all_publisher_flags + ignored_flags
            ):
                all_publisher_flags.append(match_flag)
            else:
                if (
                    match_flag
                    not in all_sensor_flags
                    + all_modem_flags
                    + all_publisher_flags
                    + all_other_flags
                    + ignored_flags
                ):
                    all_other_flags.append(match_flag)

    print(f"Found {len(all_sensor_flags)} sensor flags")
    print(f"Found {len(all_modem_flags)} modem flags")
    print(f"Found {len(all_publisher_flags)} publisher flags")
    print(f"Found {len(all_other_flags)} other flags")

    # %%
    def expand_compilers(source_dict: dict) -> list[dict]:
        arduino_cli_dict = copy.deepcopy(source_dict)
        arduino_cli_dict.update({"compiler": ["arduino-cli"], "fqbn": build_fqbns})
        pio_dict = copy.deepcopy(source_dict)
        pio_dict.update({"compiler": ["platformio"], "pio_env": build_envs})

        return list(dict_product(arduino_cli_dict)) + list(dict_product(pio_dict))

    # %%
    # Get non-menu examples
    excluded_folders = [".history", "archive", "logger_test", "tests", "more"]
    non_menu_examples = []
    for root, subdirs, files in os.walk(examples_path):
        for filename in files:
            if (
                filename == os.path.split(root)[-1] + ".ino"
                and os.path.split(menu_example_name)[-1] != os.path.split(root)[-1]
                and not any(
                    e in os.path.normpath(root).split(os.sep) for e in excluded_folders
                )
            ):
                non_menu_examples.append(os.path.relpath(root, workspace_path))

    print(f"Found {len(non_menu_examples)} non-menu examples")

    # %%
    # Assemble the matrix using ModularSensors-specific combinations
    assembled_matrix = []

    other_dict = {
        "sensor": [""],
        "modem": [""],
        "publisher": [""],
        "array": [""],
        "loop": [""],
        "serial": [""],
        "compiler_flags": [[]],
        "job_group": ["Other Examples"],
    }

    # Create a matrix for the non-menu examples
    non_menu_dict = copy.deepcopy(other_dict)
    non_menu_dict.update(
        {
            "example": [
                e
                for e in non_menu_examples
                if not any(f in e.lower() for f in ["data_saving", "mayfly", "drwi"])
            ],
        }
    )
    non_menu_matrix = expand_compilers(non_menu_dict)
    for item in non_menu_matrix:
        item["log_group"] = item["example"].split(os.sep)[-1]
    assembled_matrix += non_menu_matrix
    print(f"Total matrix items with common examples: {len(assembled_matrix)}")

    # %%
    mayfly_only_dict_a = copy.deepcopy(other_dict)
    mayfly_only_dict_a.update(
        {
            "compiler": ["arduino-cli"],
            "example": [e for e in non_menu_examples if "mayfly" in e.lower()],
            "fqbn": ["EnviroDIY:avr:envirodiy_mayfly"],
        }
    )
    mayfly_only_dict_p = copy.deepcopy(other_dict)
    mayfly_only_dict_p.update(
        {
            "compiler": ["platformio"],
            "example": [e for e in non_menu_examples if "mayfly" in e.lower()],
            "pio_env": ["mayfly"],
        }
    )
    mayfly_only_matrix = list(dict_product(mayfly_only_dict_a)) + list(
        dict_product(mayfly_only_dict_p)
    )
    for item in mayfly_only_matrix:
        item["log_group"] = item["example"].split(os.sep)[-1]
    assembled_matrix += mayfly_only_matrix
    print(
        f"Total matrix items after adding Mayfly-specific examples: {len(assembled_matrix)}"
    )

    # %%
    drwi_dict = copy.deepcopy(other_dict)
    drwi_dict_a = copy.deepcopy(other_dict)
    drwi_dict_a.update(
        {
            "compiler": ["arduino-cli"],
            "example": [
                e
                for e in non_menu_examples
                if "drwi" in e.lower() and "mayfly" not in e.lower()
            ],
            "fqbn": [
                "EnviroDIY:avr:envirodiy_mayfly",
                "EnviroDIY:samd:stonefly_m4",
            ],
        }
    )
    drwi_dict_p = copy.deepcopy(other_dict)
    drwi_dict_p.update(
        {
            "compiler": ["platformio"],
            "example": [
                e
                for e in non_menu_examples
                if "drwi" in e.lower() and "mayfly" not in e.lower()
            ],
            "pio_env": ["mayfly", "envirodiy_stonefly_m4"],
        }
    )
    drwi_matrix = list(dict_product(drwi_dict_a)) + list(dict_product(drwi_dict_p))
    for item in drwi_matrix:
        item["log_group"] = item["example"].split(os.sep)[-1]
    assembled_matrix += drwi_matrix
    print(f"Total matrix items after adding DRWI examples: {len(assembled_matrix)}")

    # %%
    simple_expandable_lists = {
        "All Sensors": all_sensor_flags,
        "All Modems": all_modem_flags,
        "All Publishers": all_publisher_flags,
        "Array Types": array_flags,
        "Loop Types": loop_flags,
    }
    for list_name, e_list in simple_expandable_lists.items():
        list_dict = {
            "example": [menu_example_name],
            "sensor": all_sensor_flags[
                0 : len(e_list) if e_list == all_sensor_flags else 1
            ],
            "modem": all_modem_flags[
                0 : len(e_list) if e_list == all_modem_flags else 1
            ],
            "publisher": all_publisher_flags[
                0 : len(e_list) if e_list == all_publisher_flags else 1
            ],
            "array": array_flags[0 : len(e_list) if e_list == array_flags else 1],
            "loop": loop_flags[0 : len(e_list) if e_list == loop_flags else 1],
            "serial": [""],
            "compiler_flags": [[]],
            "job_group": [list_name],
        }
        list_dict_a = copy.deepcopy(list_dict)
        list_dict_a.update({"compiler": ["arduino-cli"], "fqbn": build_fqbns})
        list_dict_p = copy.deepcopy(list_dict)
        list_dict_p.update({"compiler": ["platformio"], "pio_env": build_envs})
        list_matrix = list(dict_product(list_dict_a)) + list(dict_product(list_dict_p))
        for item in list_matrix:
            if list_name == "All Sensors":
                item["log_group"] = item["sensor"].replace("BUILD_SENSOR_", "")
            elif list_name == "All Modems":
                item["log_group"] = item["modem"].replace("BUILD_MODEM_", "")
            elif list_name == "All Publishers":
                item["log_group"] = item["publisher"].replace("BUILD_PUB_", "")
            elif list_name == "Array Types":
                item["log_group"] = item["array"].replace("BUILD_TEST_", "")
            elif list_name == "Loop Types":
                item["log_group"] = item["loop"].replace("BUILD_TEST_", "")
        print(f"Items for {list_name}: {len(list_matrix)}")
        assembled_matrix += list_matrix
    print(
        f"Total matrix items before adding special configurations: {len(assembled_matrix)}"
    )

    # %%
    special_config_dict = {
        "example": [menu_example_name],
        "modem": all_modem_flags[0:1],
        "publisher": all_publisher_flags[0:1],
        "array": array_flags[0:1],
        "loop": loop_flags[0:1],
    }

    # %%
    serial_sensor_flags = [
        flag
        for flag in all_sensor_flags
        if any(f in flag for f in ["_MAX_BOTIX", "YOSEMITECH_Y504", "GEOLUX_HYDRO_CAM"])
    ]
    serial_sensor_dict = copy.deepcopy(special_config_dict)
    serial_sensor_dict.update(
        {
            "sensor": serial_sensor_flags,
            "serial": serial_flags,
            "compiler_flags": [["-D NEOSWSERIAL_EXTERNAL_PCINT"]],
            "job_group": ["Serial Configurations"],
        }
    )
    serial_sensor_dict_a = copy.deepcopy(serial_sensor_dict)
    serial_sensor_dict_a.update(
        {"compiler": ["arduino-cli"], "fqbn": ["EnviroDIY:avr:envirodiy_mayfly"]}
    )
    serial_sensor_dict_p = copy.deepcopy(serial_sensor_dict)
    serial_sensor_dict_p.update(
        {"compiler": ["platformio"], "pio_env": ["serial_tests"]}
    )
    serial_sensor_matrix = list(dict_product(serial_sensor_dict_a)) + list(
        dict_product(serial_sensor_dict_p)
    )
    for item in serial_sensor_matrix:
        item["log_group"] = (
            item["serial"] + "-" + item["sensor"].replace("BUILD_SENSOR_", "")
        )
    assembled_matrix += serial_sensor_matrix
    print(
        f"Total matrix items after adding software serial configurations: {len(assembled_matrix)}"
    )

    # %%
    software_wire_dict = copy.deepcopy(special_config_dict)
    software_wire_dict.update(
        {
            "sensor": ["BUILD_SENSOR_GRO_POINT_GPLP8"],
            "serial": ["BUILD_TEST_SOFTWARE_WIRE"],
            "compiler_flags": [["-D MS_PALEOTERRA_SOFTWAREWIRE"]],
            "job_group": ["Wire Configurations"],
        }
    )
    software_wire_dict_a = copy.deepcopy(software_wire_dict)
    software_wire_dict_a.update(
        {"compiler": ["arduino-cli"], "fqbn": ["EnviroDIY:avr:envirodiy_mayfly"]}
    )
    software_wire_dict_p = copy.deepcopy(software_wire_dict)
    software_wire_dict_p.update(
        {"compiler": ["platformio"], "pio_env": ["software_wire"]}
    )
    software_wire_matrix = list(dict_product(software_wire_dict_a)) + list(
        dict_product(software_wire_dict_p)
    )
    for item in software_wire_matrix:
        item["log_group"] = "PaleoTerra Software Wire"
    assembled_matrix += software_wire_matrix
    print(
        f"Total matrix items after adding PaleoTerra software wire configurations: {len(assembled_matrix)}"
    )

    software_wire_dict_a.update(
        {
            "sensor": ["BUILD_SENSOR_RAIN_COUNTER_I2C"],
            "compiler_flags": [["-D MS_RAIN_SOFTWAREWIRE"]],
        }
    )
    software_wire_dict_p.update(
        {
            "sensor": ["BUILD_SENSOR_RAIN_COUNTER_I2C"],
            "compiler_flags": [["-D MS_RAIN_SOFTWAREWIRE"]],
        }
    )
    software_wire_matrix = list(dict_product(software_wire_dict_a)) + list(
        dict_product(software_wire_dict_p)
    )
    for item in software_wire_matrix:
        item["log_group"] = "I2C Rain Software Wire"
    assembled_matrix += software_wire_matrix
    print(
        f"Total matrix items after adding I2C Rain software wire configurations: {len(assembled_matrix)}"
    )

    # %%
    sdi_sensor_flags = [
        flag
        for flag in all_sensor_flags
        if any(
            f in flag
            for f in [
                "SDI12",
                "DECAGON",
                "METER",
                "CLARI_VUE10",
                "RAINVUE",
                "IN_SITU_RDO",
                "ZEBRA_TECH",
            ]
        )
    ]
    sdi12_dict = copy.deepcopy(special_config_dict)
    sdi12_dict.update(
        {
            "sensor": sdi_sensor_flags,
            "serial": [""],
            "compiler_flags": [["-D MS_SDI12_NON_CONCURRENT"]],
            "job_group": ["SDI-12 Non-Concurrent"],
        }
    )
    sdi12_dict_a = copy.deepcopy(software_wire_dict)
    sdi12_dict_a.update(
        {
            "compiler": ["arduino-cli"],
            "fqbn": ["EnviroDIY:avr:envirodiy_mayfly", "EnviroDIY:samd:stonefly_m4"],
        }
    )
    sdi12_dict_p = copy.deepcopy(software_wire_dict)
    sdi12_dict_p.update(
        {
            "compiler": ["platformio"],
            "pio_env": ["sdi12_non_concurrent", "sdi12_non_concurrent_stonefly"],
        }
    )
    sdi12_matrix = list(dict_product(sdi12_dict_a)) + list(dict_product(sdi12_dict_p))
    for item in sdi12_matrix:
        item["log_group"] = item["sensor"].replace("BUILD_SENSOR_", "")
    assembled_matrix += sdi12_matrix
    print(
        f"Total matrix items after adding SDI12 configurations: {len(assembled_matrix)}"
    )

    analog_sensor_flags = [
        flag
        for flag in all_sensor_flags
        if any(f in flag for f in ["SQ212", "OBS3", "TIADS1X15", "TURNER_CYCLOPS"])
    ]
    ads_dict = copy.deepcopy(special_config_dict)
    ads_dict.update(
        {
            "sensor": analog_sensor_flags,
            "serial": [""],
            "compiler_flags": [["-D MS_USE_ADS1015"]],
            "job_group": ["ADS 1015"],
        }
    )
    ads_dict_a = copy.deepcopy(software_wire_dict)
    ads_dict_a.update(
        {
            "compiler": ["arduino-cli"],
            "fqbn": ["EnviroDIY:avr:envirodiy_mayfly", "EnviroDIY:samd:stonefly_m4"],
        }
    )
    ads_dict_p = copy.deepcopy(software_wire_dict)
    ads_dict_p.update(
        {
            "compiler": ["platformio"],
            "pio_env": ["ads1015", "ads1015_stonefly"],
        }
    )
    ads_matrix = list(dict_product(ads_dict_a)) + list(dict_product(ads_dict_p))
    for item in ads_matrix:
        item["log_group"] = item["sensor"].replace("BUILD_SENSOR_", "")
    assembled_matrix += ads_matrix
    print(
        f"Total matrix items after adding ADS configurations: {len(assembled_matrix)}"
    )

    # %%
    # Sort and deduplicate the matrix
    assembled_matrix = sorted(
        assembled_matrix,
        key=lambda x: (
            x["compiler"],
            x["example"],
            x["fqbn"] if "fqbn" in x else x["pio_env"],
            x["sensor"],
            x["modem"],
            x["publisher"],
            x["array"],
            x["loop"],
            x["serial"],
            x["job_group"],
        ),
    )
    final_matrix = remove_nested_duplicates(assembled_matrix)
    print(f"Final filtered matrix: {len(final_matrix)}")

    # %%
    # convert all of the flag types to inline defines for the final matrix
    for item in final_matrix:
        item["inline_defines"] = []
        for key in [
            "sensor",
            "modem",
            "publisher",
            "array",
            "loop",
            "serial",
        ]:
            if item[key] and len(item[key]) > 0:
                item["inline_defines"].append(item[key])
            item.pop(key, None)
        item["board"] = item.get("fqbn", item.get("pio_env", ""))
        # add clocks for Arduino CLI boards that need them
        if item["compiler"] == "arduino-cli":
            if item["fqbn"] == "adafruit:samd:adafruit_feather_m0":
                item["compiler_flags"].append(f"-D MS_USE_RV8803")
            if item["fqbn"] == "adafruit:samd:adafruit_feather_m4":
                item["compiler_flags"].append(f"-D MS_USE_DS3231")
            if item["fqbn"] == "adafruit:samd:adafruit_grandcentral_m4":
                item["compiler_flags"].append(f"-D MS_USE_RV8803")
            if item["fqbn"] == "arduino:avr:mega":
                item["compiler_flags"].append(f"-D MS_USE_RV8803")
            if item["fqbn"] == "arduino:samd:mzero_bl":
                item["compiler_flags"].append(f"-D MS_USE_RTC_ZERO")

    # %%
    return final_matrix


# %%
# cSpell:ignore fqbn fqbns PCINT Wextra adafruit_grandcentral_m4 mzero_bl
