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

    # Pattern for flags in the menu-a-la-cart example
    pattern = re.compile(
        r"(?:#if|#elif) defined[\s\(](?P<flag1>BUILD_\w+)((?:[\s\n\\\)]*?\|\|[\s\n\\]*defined[\s\n\\\(]*?)(?P<flag_last>BUILD_\w+))*",
        re.MULTILINE,
    )

    # Lists for the flags
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

    # Get non-menu examples
    excluded_folders = [".history", "archive", "logger_test", "tests", "more"]
    non_menu_examples = []
    for root, subdirs, files in os.walk(examples_path):
        for filename in files:
            if (
                filename == os.path.split(root)[-1] + ".ino"
                and menu_example_name != os.path.split(root)[-1]
                and not any(
                    e in os.path.normpath(root).split(os.sep) for e in excluded_folders
                )
            ):
                non_menu_examples.append(os.path.relpath(root, workspace_path))

    print(f"Found {len(non_menu_examples)} non-menu examples")

    # %%
    # Assemble the matrix using ModularSensors-specific combinations
    assembled_matrix = []

    # Create a matrix for the non-menu examples
    non_menu_matrix = list(
        dict_product(
            {
                "compiler": compiler_list,
                "example": [
                    e
                    for e in non_menu_examples
                    if not any(
                        f in e.lower() for f in ["data_saving", "mayfly", "drwi"]
                    )
                ],
                "board": boards,
                "sensor": [""],
                "modem": [""],
                "publisher": [""],
                "array": [""],
                "loop": [""],
                "serial": [""],
                "compiler_flags": [[]],
                "job_group": ["Other Examples"],
            }
        )
    )
    assembled_matrix += non_menu_matrix
    print(f"Total matrix items with common examples: {len(assembled_matrix)}")

    # %%
    mayfly_only_matrix = list(
        dict_product(
            {
                "compiler": compiler_list,
                "example": [e for e in non_menu_examples if "mayfly" in e.lower()],
                "board": ["mayfly"],
                "sensor": [""],
                "modem": [""],
                "publisher": [""],
                "array": [""],
                "loop": [""],
                "serial": [""],
                "compiler_flags": [[]],
                "job_group": ["Other Examples"],
            }
        )
    )
    assembled_matrix += mayfly_only_matrix
    print(
        f"Total matrix items after adding Mayfly-specific examples: {len(assembled_matrix)}"
    )

    # %%
    drwi_matrix = list(
        dict_product(
            {
                "compiler": compiler_list,
                "example": [
                    e
                    for e in non_menu_examples
                    if "drwi" in e.lower() and "mayfly" not in e.lower()
                ],
                "board": ["mayfly", "stonefly"],
                "sensor": [""],
                "modem": [""],
                "publisher": [""],
                "array": [""],
                "loop": [""],
                "serial": [""],
                "compiler_flags": [[]],
                "job_group": ["Other Examples"],
            }
        )
    )
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
        list_matrix = list(
            dict_product(
                {
                    "compiler": compiler_list,
                    "example": [menu_example_name],
                    "board": boards,
                    "sensor": all_sensor_flags[
                        0 : len(e_list) if e_list == all_sensor_flags else 1
                    ],
                    "modem": all_modem_flags[
                        0 : len(e_list) if e_list == all_modem_flags else 1
                    ],
                    "publisher": all_publisher_flags[
                        0 : len(e_list) if e_list == all_publisher_flags else 1
                    ],
                    "array": array_flags[
                        0 : len(e_list) if e_list == array_flags else 1
                    ],
                    "loop": loop_flags[0 : len(e_list) if e_list == loop_flags else 1],
                    "serial": [""],
                    "compiler_flags": [[]],
                    "job_group": [list_name],
                }
            )
        )
        print(f"Items for {list_name}: {len(list_matrix)}")
        assembled_matrix += list_matrix
    print(
        f"Total matrix items before adding special configurations: {len(assembled_matrix)}"
    )

    # %%
    serial_sensor_flags = [
        flag
        for flag in all_sensor_flags
        if any(f in flag for f in ["_MAX_BOTIX", "YOSEMITECH_Y504"])
    ]
    serial_sensor_matrix = list(
        dict_product(
            {
                "compiler": compiler_list,
                "example": [menu_example_name],
                "board": ["serial_tests"],
                "sensor": serial_sensor_flags,
                "modem": all_modem_flags[0:1],
                "publisher": all_publisher_flags[0:1],
                "array": array_flags[0:1],
                "loop": loop_flags[0:1],
                "serial": serial_flags,
                "compiler_flags": [["NEOSWSERIAL_EXTERNAL_PCINT"]],
                "job_group": ["Serial Configurations"],
            }
        )
    )
    assembled_matrix += serial_sensor_matrix
    print(
        f"Total matrix items after adding software serial configurations: {len(assembled_matrix)}"
    )

    # %%
    software_wire_matrix = list(
        dict_product(
            {
                "compiler": compiler_list,
                "example": [menu_example_name],
                "board": ["software_wire"],
                "sensor": ["BUILD_SENSOR_GRO_POINT_GPLP8"],
                "modem": all_modem_flags[0:1],
                "publisher": all_publisher_flags[0:1],
                "array": array_flags[0:1],
                "loop": loop_flags[0:1],
                "serial": ["BUILD_TEST_SOFTWARE_WIRE"],
                "compiler_flags": [["MS_PALEOTERRA_SOFTWAREWIRE"]],
                "job_group": ["Wire Configurations"],
            }
        )
    )
    assembled_matrix += software_wire_matrix
    print(
        f"Total matrix items after adding PaleoTerra software wire configurations: {len(assembled_matrix)}"
    )

    software_wire_matrix = list(
        dict_product(
            {
                "compiler": compiler_list,
                "example": [menu_example_name],
                "board": ["software_wire"],
                "sensor": ["BUILD_SENSOR_RAIN_COUNTER_I2C"],
                "modem": all_modem_flags[0:1],
                "publisher": all_publisher_flags[0:1],
                "array": array_flags[0:1],
                "loop": loop_flags[0:1],
                "serial": ["BUILD_TEST_SOFTWARE_WIRE"],
                "compiler_flags": [["MS_RAIN_SOFTWAREWIRE"]],
                "job_group": ["Wire Configurations"],
            }
        )
    )
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
    sdi12_matrix = list(
        dict_product(
            {
                "compiler": compiler_list,
                "example": [menu_example_name],
                "board": ["sdi12_non_concurrent", "sdi12_non_concurrent_stonefly"],
                "sensor": sdi_sensor_flags,
                "modem": all_modem_flags[0:1],
                "publisher": all_publisher_flags[0:1],
                "array": array_flags[0:1],
                "loop": loop_flags[0:1],
                "serial": serial_flags[0:1],
                "compiler_flags": [["MS_SDI12_NON_CONCURRENT"]],
                "job_group": ["SDI-12 Configurations"],
            }
        )
    )
    assembled_matrix += sdi12_matrix
    print(
        f"Total matrix items after adding SDI12 configurations: {len(assembled_matrix)}"
    )

    analog_sensor_flags = [
        flag
        for flag in all_sensor_flags
        if any(f in flag for f in ["SQ212", "OBS3", "TIADS1X15", "TURNER_CYCLOPS"])
    ]
    ads_matrix = list(
        dict_product(
            {
                "compiler": compiler_list,
                "example": [menu_example_name],
                "board": ["ads1015", "ads1015_stonefly"],
                "sensor": analog_sensor_flags,
                "modem": all_modem_flags[0:1],
                "publisher": all_publisher_flags[0:1],
                "array": array_flags[0:1],
                "loop": loop_flags[0:1],
                "serial": serial_flags[0:1],
                "compiler_flags": [["MS_USE_ADS1015"]],
                "job_group": ["ADS Configurations"],
            }
        )
    )
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
            x["board"],
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
    return final_matrix


# %%
# cSpell:ignore fqbns PCINT Wextra
