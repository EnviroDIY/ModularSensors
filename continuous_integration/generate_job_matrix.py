#!/usr/bin/env python
# %%
from copy import deepcopy
from itertools import chain, product
import os
import re
from typing import List
import json
import shutil
import requests

from platformio.project.config import ProjectConfig

# %%
# set verbose
use_verbose = False
if "RUNNER_DEBUG" in os.environ.keys() and os.environ["RUNNER_DEBUG"] == "1":
    use_verbose = True


# %%
# The workspace directory
if "GITHUB_WORKSPACE" in os.environ.keys():
    workspace_dir = os.environ.get("GITHUB_WORKSPACE", os.getcwd())
else:
    workspace_dir = os.getcwd()

if os.path.basename(os.path.normpath(workspace_dir)) == "continuous_integration":
    workspace_dir = os.path.dirname(workspace_dir)

workspace_path = os.path.abspath(os.path.realpath(workspace_dir))
print(f"Workspace Path: {workspace_path}")


# %%
# The examples directory
examples_dir = "./examples/"
examples_path = os.path.join(workspace_dir, examples_dir)
examples_path = os.path.abspath(os.path.realpath(examples_path))
print(f"Examples Path: {examples_path}")


# %%
# The extras directory
extras_dir = "./extras/"
extras_path = os.path.join(workspace_dir, extras_dir)
extras_path = os.path.abspath(os.path.realpath(extras_path))
print(f"Extras Path: {extras_path}")


# %%
# The continuous integration directory
ci_dir = "./continuous_integration/"
ci_path = os.path.join(workspace_dir, ci_dir)
ci_path = os.path.abspath(os.path.realpath(ci_path))
print(f"Continuous Integration Path: {ci_path}")
if not os.path.exists(ci_path):
    print(f"Creating the directory for CI: {ci_path}")
    os.makedirs(ci_path, exist_ok=True)


# %%
# A directory of files to save and upload as artifacts to use in future jobs
artifact_dir = os.path.join(
    os.path.join(workspace_dir, "continuous_integration_artifacts")
)
artifact_path = os.path.abspath(os.path.realpath(artifact_dir))
print(f"Artifact Path: {artifact_path}")
if not os.path.exists(artifact_dir):
    print(f"Creating the directory for artifacts: {artifact_path}")
    os.makedirs(artifact_dir)

# %%
# Set the compiler list
compiler_list = ["arduino-cli", "pio"]
pio_skip_boards = []
acli_skip_boards = []

# The library config file
ms_config_file = "ModSensorConfig.h"

compilers = ["Arduino CLI", "PlatformIO"]
non_acli_flag = [
    "-Wall",
    "-Wextra",
    "-D SDI12_EXTERNAL_PCINT",
    "-D NEOSWSERIAL_EXTERNAL_PCINT",
]
ms_config_path = os.path.join(
    workspace_dir,
    "home",
    "arduino",
    "user",
    "libraries",
    "ModularSensors",
    "src",
    ms_config_file,
)
ms_config_path = os.path.abspath(os.path.realpath(ms_config_path))
print(f"Modular Sensors Config Path: {ms_config_path}")


# %%
# The locations of some important files

# The massive "menu" example
menu_example_name = "menu_a_la_carte"
menu_file_path = os.path.join(
    os.path.join(examples_path, menu_example_name), menu_example_name + ".ino"
)

# %%
# Find all of the non-menu examples
non_menu_examples = []
excluded_folders = [
    ".history",
    "archive",
    "logger_test",
    "tests",
    "more",
    menu_example_name,  # "OutdatedExamples"
]
for root, subdirs, files in os.walk(examples_path):
    folder_name = os.path.basename(root)
    if folder_name in set(excluded_folders):
        subdirs.clear()  # Prevent os.walk from descending into excluded directories
        continue
    for filename in files:
        file_path = os.path.join(root, filename)
        if filename == os.path.split(root)[-1] + ".ino" and not any(
            e in os.path.normpath(root).split(os.sep) for e in set(excluded_folders)
        ):
            non_menu_examples.append(os.path.relpath(root, workspace_path))
            # print(f"\t- example: {filename} (full path: {file_path})")
            if use_verbose:
                print(f"::debug::\t- example: {filename} (full path: {file_path})")


def get_example_folder(subfolder_name):
    return os.path.join(examples_path, subfolder_name)


def get_example_filepath(subfolder_name):
    ex_folder = get_example_folder(subfolder_name)
    ex_file = os.path.join(ex_folder, subfolder_name + ".ino")
    return ex_file


# %%
# Pull files to convert between boards and platforms and FQBNs
# Translation between board names on PlatformIO and the Arduino CLI
response = requests.get(
    "https://raw.githubusercontent.com/EnviroDIY/workflows/main/scripts/platformio_to_arduino_boards.json"
)
with open(os.path.join(ci_path, "platformio_to_arduino_boards.json"), "wb") as f:
    f.write(response.content)
with open(os.path.join(ci_path, "platformio_to_arduino_boards.json")) as f:
    pio_to_acli = json.load(f)


# %%
# Arduino CLI configuration
# Always use the generic one from the shared workflow repository
downloaded_arduino_cli_config = False
if "GITHUB_WORKSPACE" in os.environ.keys():
    arduino_cli_config = os.path.join(ci_path, "arduino_cli.yaml")
    arduino_cli_format = "json"
    if not os.path.isfile(arduino_cli_config):
        downloaded_arduino_cli_config = True
        # download the default file
        response = requests.get(
            "https://raw.githubusercontent.com/EnviroDIY/workflows/main/scripts/arduino_cli.yaml"
        )
        # copy to the CI directory
        with open(os.path.join(ci_path, "arduino_cli.yaml"), "wb") as f:
            f.write(response.content)
        # also copy to the artifacts directory
        shutil.copyfile(
            os.path.join(ci_path, "arduino_cli.yaml"),
            os.path.join(artifact_path, "arduino_cli.yaml"),
        )
else:
    arduino_cli_config = os.path.abspath(
        os.path.join(ci_path, "arduino_cli_local.yaml")
    )
    arduino_cli_format = "json"

# %%
# PlatformIO configuration
# If one exists in a "continuous_integration" subfolder of the repository, use it.
# Otherwise, use the generic one from the shared workflow repository
default_pio_config_file = False
pio_config_file = os.path.join(ci_path, "platformio.ini")
if not os.path.isfile(pio_config_file):
    # download the default file
    response = requests.get(
        "https://raw.githubusercontent.com/EnviroDIY/workflows/main/scripts/platformio.ini"
    )
    # make a directory for it and copy it there
    with open(os.path.join(ci_path, "platformio.ini"), "wb") as f:
        f.write(response.content)
    # also copy to the artifacts directory
    shutil.copyfile(
        os.path.join(ci_path, "platformio.ini"),
        os.path.join(artifact_path, "platformio.ini"),
    )
    # mark we're using default
    default_pio_config_file = True

pio_config = ProjectConfig(pio_config_file)

extra_configs = None
pio_config_expanded = deepcopy(pio_config)
pio_extra_config_file = os.path.join(ci_path, "platformio_extra_flags.ini")
if os.path.isfile(pio_extra_config_file):
    pio_config_expanded.read(pio_extra_config_file)

board_to_pio_env = {}
pio_env_to_board = {}
for pio_env_name in pio_config.envs():
    board_to_pio_env[pio_config.get("env:{}".format(pio_env_name), "board")] = (
        pio_env_name
    )
for pio_env_name in pio_config_expanded.envs():
    pio_env_to_board[pio_env_name] = pio_config_expanded.get(
        "env:{}".format(pio_env_name), "board"
    )


# %%
# Parse the boards to build
boards = pio_config.envs()
# if extra_configs is not None:
#     boards += extra_configs.envs()


# %% read build flags out of the menu-a-la-cart example
# Parse any extra flags to add to the build commands
# Pattern for flags in the menu-a-la-cart example
pattern = re.compile(
    r"(?:#if|#elif) defined[\s\(](?P<flag1>BUILD_\w+)((?:[\s\n\\\)]*?\|\|[\s\n\\]*defined[\s\n\\\(]*?)(?P<flag_last>BUILD_\w+))*",
    re.MULTILINE,
)

# lists for the flags
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
textfile = open(menu_file_path, "r")
filetext = textfile.read()
textfile.close()

# find matches and add them to the lists
for match in re.finditer(pattern, filetext):
    # print(match.group("flag1"))
    # print(match.group("flag_last"))
    if (
        "_SENSOR_" in match.group("flag1")
        and match.group("flag1") not in all_sensor_flags + ignored_flags
    ):
        all_sensor_flags.append(match.group("flag1"))
    elif (
        "_MODEM_" in match.group("flag1")
        and match.group("flag1") not in all_modem_flags + ignored_flags
    ):
        all_modem_flags.append(match.group("flag1"))
    elif (
        "_PUB_" in match.group("flag1")
        and match.group("flag1") not in all_publisher_flags + ignored_flags
    ):
        all_publisher_flags.append(match.group("flag1"))
    else:
        if (
            match.group("flag1")
            not in all_sensor_flags
            + all_modem_flags
            + all_publisher_flags
            + all_other_flags
            + ignored_flags
        ):
            all_other_flags.append(match.group("flag1"))


# %%
# Source - https://stackoverflow.com/a/40623158
# Posted by Tarrasch, modified by community. See post 'Timeline' for change history
# Retrieved 2026-08-09, License - CC BY-SA 4.0
def dict_product(options):
    """
    >>> list(dict_product({'number': [1, 2], 'character': 'ab'}))
    [{'character': 'a', 'number': 1},
     {'character': 'a', 'number': 2},
     {'character': 'b', 'number': 1},
     {'character': 'b', 'number': 2}]
    """
    return (dict(zip(options.keys(), x)) for x in product(*options.values()))


def remove_duplicate_dicts(list_with_dup_dicts):
    # remove duplicates based on all keys except "job_group"
    seen = set()
    deduped_list = []

    for d in list_with_dup_dicts:
        json_str = json.dumps(
            {k: v for k, v in d.items() if k != "job_group"}, sort_keys=True
        )
        if json_str not in seen:
            seen.add(json_str)
            deduped_list.append(d)
    return deduped_list


# %%
# create a matrix for the non-menu examples
assembled_matrix = []
non_menu_matrix = list(
    dict_product(
        {
            "compiler": compiler_list,
            "example": [
                e
                for e in non_menu_examples
                if not any(f in e.lower() for f in ["data_saving", "mayfly", "drwi"])
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

simple_expandable_lists = {
    "All Sensor": all_sensor_flags,
    "All Modem": all_modem_flags,
    "All Publisher": all_publisher_flags,
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
                "array": array_flags[0 : len(e_list) if e_list == array_flags else 1],
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
print(f"Total matrix items after adding SDI12 configurations: {len(assembled_matrix)}")

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
print(f"Total matrix items after adding ADS configurations: {len(assembled_matrix)}")

# %%
# decide on the filtered matrix to use for the job matrix
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

# remove duplicates based on all keys except "compiler_flags"
final_matrix = remove_duplicate_dicts(assembled_matrix)
print(f"Final filtered matrix: {len(final_matrix)}")


# %%
# helper functions to create commands
def create_arduino_cli_compile_command(
    code_subfolder: str,
    fqbn: str,
    compiler_flags: List[str] = [],
) -> str:
    arduino_command_args = [
        "arduino-cli",
        "compile",
    ]
    if use_verbose:
        arduino_command_args += ["--verbose"]
    arduino_command_args += [
        "--warnings",
        "more",
        "--config-file",
        f'"{arduino_cli_config}"',
        "--format",
        f"{arduino_cli_format}",
        "--fqbn",
        fqbn,
    ]
    if len(compiler_flags) > 0:
        arduino_command_args += [
            "--build-property",
            "compiler.cpp.extra_flags=" + " ".join(compiler_flags),
        ]
    arduino_command_args += [
        f'"{os.path.join(workspace_path, code_subfolder)}"',
    ]
    return " ".join(arduino_command_args)


def create_pio_ci_compile_command(
    code_subfolder: str,
    pio_board_or_env: str | List[str],
    use_pio_config_file: bool,
    compiler_flags: List[str] = [],
    use_run: bool = False,
) -> str:
    pio_command_args = [
        "pio",
        "run" if use_run else "ci",
    ]
    if use_verbose:
        pio_command_args += ["--verbose"]
    if use_pio_config_file:
        pio_command_args += ["--project-conf", f'"{pio_config_file}"']
        if type(pio_board_or_env) == str:
            pio_command_args += ["--environment", pio_board_or_env]
        else:
            for pio_board_or_env_item in pio_board_or_env:
                pio_command_args += [
                    "--environment",
                    pio_board_or_env_item,
                ]
    elif not use_run:
        if type(pio_board_or_env) == str:
            pio_command_args += [
                "--board",
                pio_board_or_env,
            ]
        else:
            for pio_board_or_env_item in pio_board_or_env:
                pio_command_args += [
                    "--board",
                    pio_board_or_env_item,
                ]
    else:
        raise ValueError(
            "you must be using a pio config file if you are using the 'run' command"
        )
    if use_run:
        pio_command_args += [
            "--project-dir",
            f'"{os.path.realpath(os.path.join(artifact_dir, "pio_ci_build"))}"',
        ]
    if (
        len(compiler_flags) > 0 and not use_pio_config_file and not use_run
    ):  # these CANNOT be used with a pio config file
        pio_command_args += [
            "--project-option",
            f"\"build_flags = {' '.join(compiler_flags)}\"",
        ]
    else:
        if len(compiler_flags) > 0 and use_pio_config_file and not use_run:
            config_build_flags = pio_config_expanded.get(
                f"env:{pio_board_or_env}", "build_flags", []
            )
            missing_flags = [
                flag
                for flag in compiler_flags
                if flag not in config_build_flags
                and "-D" + flag not in config_build_flags
                and "-D " + flag not in config_build_flags
            ]
            if missing_flags:
                print(
                    f"Warning: extra compiler flags {missing_flags} will not be used because they are not in your pio config file."
                )
                print(
                    f"Warning: These flags are in your pio config file: {config_build_flags}"
                )
        elif len(compiler_flags) > 0 and use_run:
            print(
                f"NOTE: extra compiler flags {compiler_flags} are being ignored because you are using a pio run command."
            )
    if not use_run:
        pio_command_args += [
            f'"{os.path.join(workspace_path, code_subfolder)}"',
        ]

    return " ".join(pio_command_args)


def get_filename_slug(job_key, value) -> str:
    replace_list = [
        ("BUILD_MODEM_", ""),
        ("BUILD_SENSOR_", ""),
        ("BUILD_PUB_", ""),
        ("BUILD_TEST_", ""),
        ("_PUBLISHER", ""),
        ("TINY_GSM_MODEM_", ""),
        ("_", "-"),
        (" ", "-"),
    ]

    def replace_all(s):
        for old, new in replace_list:
            s = s.replace(old, new)
        return s

    if job_key in ["compiler", "board", "flag"]:
        return replace_all(value)
    elif (
        job_key in ["inline_flags", "compiler_flags"]
        and isinstance(value, list)
        and len(value) > 0
        and isinstance(value[0], list)
    ):
        return "-".join(
            [
                replace_all(f)
                for f in [
                    "-".join([replace_all(g) for g in sublist]) for sublist in value
                ]
            ]
        )
    elif job_key in ["inline_flags", "compiler_flags"] and isinstance(value, list):
        return "-".join([replace_all(f) for f in value])
    elif job_key in ["inline_flags", "compiler_flags"] and isinstance(value, str):
        return replace_all(value)
    elif job_key == "example":
        return replace_all(str(value).rsplit(os.path.sep)[-1])
    else:
        return replace_all(str(value))


def get_filename_for_log(job: dict, name_keys: list[str]) -> str:
    if "compiler" in job:
        compiler = job["compiler"]
    else:
        compiler = "arduino-cli" if "arduino-cli" in job["command"][0] else "pio"
    extension = "json" if compiler == "arduino-cli" else "log"

    file_name = ""
    for key in name_keys:
        if key not in job:
            raise ValueError(f"Job dictionary must contain the key '{key}'")
        if job[key] is not None and job[key] != "" and job[key] != []:
            file_name += "_" + get_filename_slug(key, job[key])

    if file_name.startswith("_"):
        file_name = file_name[1:]
    return os.path.abspath(
        os.path.join(
            artifact_path,
            f"{file_name}.{extension}",
        )
    )


def group_and_log_commands(
    build_commands: List[str],
    other_commands: List[str],
    group_title: str,
    output_filename: str,
) -> List[str]:
    command_list = []
    command_list.append("\necho ::group::{}".format(group_title))
    command_list.append("group_failed=0")
    command_list.extend(other_commands)
    for command in build_commands:
        if command.startswith("sed"):
            command_list.append(command)
        else:
            command_list.append(command + ' 2>&1 | tee -a "{}"'.format(output_filename))
            command_list.append("result_code=${PIPESTATUS[0]}")
            command_list.append(
                'if [ "$result_code" -ne "0" ]; then group_failed=1; status=1; fi'
            )
    # command_list.append(
    #     f'if [ "$group_failed" -eq "0" ]; then echo -e " - {group_title} :white_check_mark:" >> $GITHUB_STEP_SUMMARY; else echo -e " - {group_title} :x:" >> $GITHUB_STEP_SUMMARY; fi'
    # )
    command_list.append("echo ::endgroup::")
    command_list.append(
        f'if [ "$group_failed" -eq "0" ]; then echo -e "\\e[32m{group_title} successfully compiled\\e[0m"; else echo -e "\\e[31m{group_title} failed to compile\\e[0m"; fi'
    )
    return command_list


def create_command_list_from_matrix(matrix_item: dict, **kwargs):
    required_keys = ["compiler", "example", "board"]
    for key in required_keys:
        if key not in matrix_item:
            raise ValueError(f"Matrix item must contain the key '{key}'")
    compiler = matrix_item.get("compiler", "")
    example = matrix_item.get("example", "")
    board = matrix_item.get("board", "")
    compiler_flags = list(matrix_item.get("compiler_flags", []))
    inline_flags = list(matrix_item.get("inline_flags", []))
    for key, value in matrix_item.items():
        if (
            key not in required_keys
            and "inline_flags" not in key
            and "compiler_flags" not in key
            and len(value) > 0
        ):
            inline_flags.append(value)
    job_dict = deepcopy(matrix_item)
    job_dict["inline_flags"] = inline_flags
    output_file_name = get_filename_for_log(job_dict, list(matrix_item.keys()))
    if compiler == "arduino-cli":
        acli_board = board if board in pio_to_acli else pio_env_to_board.get(board)
        if acli_board not in pio_to_acli or board in acli_skip_boards:
            print(
                f"Skipping {example} for {board} because no matching Arduino FQBN was found."
            )
            return None
        fqbn = pio_to_acli[acli_board]["fqbn"]
        build_command = create_arduino_cli_compile_command(
            code_subfolder=example, fqbn=fqbn, compiler_flags=compiler_flags, **kwargs
        )
    elif compiler == "pio":
        if board in pio_skip_boards:
            print(
                f"Skipping {example} for {board} because it is in the list of boards to skip for PlatformIO."
            )
            return None
        pio_board_or_env = board
        if board in pio_env_to_board.keys():
            use_pio_config_file = True
        else:
            use_pio_config_file = False
        build_command = create_pio_ci_compile_command(
            code_subfolder=example,
            pio_board_or_env=pio_board_or_env,
            use_pio_config_file=use_pio_config_file,
            compiler_flags=compiler_flags,
            **kwargs,
        )
    else:
        raise ValueError("Invalid compiler provided.")

    example_name = f"{os.path.split(example)[-1]}"
    example_full_path = os.path.join(workspace_path, example, example_name + ".ino")
    sed_commands: List[str] = []
    for flag in inline_flags:
        if len(flag) > 0:
            define_name, _, define_value = flag.partition("=")
            sed_commands.append(
                f"sed -i '1i\\\n#if !defined({define_name})\\\n"
                f"#define {define_name} {define_value}\\\n"
                f'#endif\\\n\' "{example_full_path}"'
            )

    job_dict["output_file_name"] = output_file_name
    job_dict["other_commands"] = sed_commands
    job_dict["build_commands"] = [build_command]

    return deepcopy(job_dict)


# %%
# convert the matrix into a list of commands for each board and flag combination
print(f"Final matrix: {len(final_matrix)}")
complete_command_matrix: List[dict] = []
for matrix_item in final_matrix:
    command_block = create_command_list_from_matrix(matrix_item=matrix_item)
    if command_block is not None:
        complete_command_matrix.append(command_block)
    else:
        continue
print(f"Total command blocks: {len(complete_command_matrix)}")


# %%
# group the commands by how we want the collapsing in the logs to work
log_groupers = final_matrix[0].keys()
grouped_command_matrix: dict[str, dict[str, str | List[str]]] = {}
for matrix_item in complete_command_matrix:
    l_names = []
    for grouper in log_groupers:
        if grouper not in matrix_item.keys():
            raise ValueError(
                f"Matrix item {matrix_item} does not have the key {grouper}"
            )
        elif matrix_item[grouper] is None:
            raise ValueError(
                f"Matrix item {matrix_item} has a None value for the key {grouper}"
            )
        else:
            l_names.append(get_filename_slug(grouper, matrix_item[grouper]))
    l_key = "-".join(l_names)
    l_key = re.sub(r"[\-]{2,}", "-", l_key)
    l_key = re.sub(r"[_]{2,}", "_", l_key)
    l_command_list = group_and_log_commands(
        matrix_item["build_commands"],
        matrix_item["other_commands"],
        group_title=l_key,
        output_filename=matrix_item["output_file_name"],
    )
    if l_key not in grouped_command_matrix.keys():
        l_dict: dict[str, str | List[str]] = {
            "log_group": l_key,
            "group_commands": l_command_list,
        }
        for grouper in log_groupers:
            l_dict[grouper] = matrix_item[grouper]
        grouped_command_matrix[l_key] = l_dict
    else:
        grouped_command_matrix[l_key]["group_commands"] += l_command_list  # type: ignore
        # break
print(f"Total log groups: {len(grouped_command_matrix)}")


# %%
# group the commands into jobs
job_groupers = ["compiler", "board", "job_group"]
grouped_job_matrix = {}
for l_key, group_dict in grouped_command_matrix.items():
    j_names = []
    for grouper in job_groupers:
        if grouper not in group_dict.keys():
            raise ValueError(
                f"Matrix item {group_dict} does not have the key {grouper}"
            )
        elif group_dict[grouper] is None:
            raise ValueError(
                f"Matrix item {group_dict} has a None value for the key {grouper}"
            )
        else:
            j_names.append(get_filename_slug(grouper, group_dict[grouper]))
    job_name = " - ".join(j_names)
    job_tag = "-".join(j_names)
    if job_tag not in grouped_job_matrix.keys():
        j_dict: dict[str, str | List[str]] = {
            "job_name": job_name,
            "job_tag": job_tag.lower(),
            "job_command": group_dict["group_commands"],
        }
        for grouper in log_groupers:
            j_dict[grouper] = group_dict[grouper]
        grouped_job_matrix[job_tag] = j_dict
    else:
        grouped_job_matrix[job_tag]["job_command"] += group_dict["group_commands"]
print(f"Total jobs: {len(grouped_job_matrix)}")


# %%
# Convert commands in the matrix into bash scripts
start_job_commands: List[str] = ["status=0"]
end_job_commands: List[str] = ["\n\nexit $status"]
for job_tag, matrix_job in grouped_job_matrix.items():
    bash_file_name = job_tag + ".sh"
    print(f"Writing bash file to {os.path.join(artifact_path, bash_file_name)}")
    bash_out = open(os.path.join(artifact_path, bash_file_name), "w+")
    bash_out.write("#!/bin/bash\n\n")
    bash_out.write("""
set -e # Exit with nonzero exit code if anything fails
if [ "$RUNNER_DEBUG" = "1" ]; then
    echo "Enabling debugging!"
    set -v # Prints shell input lines as they are read.
    set -x # Print command traces before executing command.
fi

""")
    bash_out.write("\n".join(start_job_commands))
    bash_out.write("\n\n")
    bash_out.write("\n".join(matrix_job["job_command"]))
    bash_out.write("\n\n")
    bash_out.write("\n".join(end_job_commands))
    bash_out.close()
    matrix_job["script"] = os.path.join(artifact_path, bash_file_name)

# Remove the command from the dictionaries before outputting them
arduino_job_matrix = [
    {
        vk: vv
        for vk, vv in v.items()
        if vk == "job_name" or vk == "job_tag" or vk == "script"
    }
    for k, v in grouped_job_matrix.items()
    if v["compiler"] == "arduino-cli"
]
pio_job_matrix = [
    {
        vk: vv
        for vk, vv in v.items()
        if vk == "job_name" or vk == "job_tag" or vk == "script"
    }
    for k, v in grouped_job_matrix.items()
    if v["compiler"] == "pio"
]

# %%
# Write out output
print(
    'echo "arduino_job_matrix={}" >> $GITHUB_OUTPUT'.format(
        json.dumps(arduino_job_matrix)
    )
)
json_out = open(os.path.join(artifact_dir, "arduino_job_matrix.json"), "w+")
json.dump(arduino_job_matrix, json_out, indent=2)
json_out.close()

print('echo "pio_job_matrix={}" >> $GITHUB_OUTPUT'.format(json.dumps(pio_job_matrix)))
json_out = open(os.path.join(artifact_dir, "pio_job_matrix.json"), "w+")
json.dump(pio_job_matrix, json_out, indent=2)
json_out.close()


# %%
# different attempt to save output
if "GITHUB_WORKSPACE" in os.environ.keys():
    with open(os.environ["GITHUB_OUTPUT"], "a") as fh:
        print("arduino_job_matrix={}".format(json.dumps(arduino_job_matrix)), file=fh)
        print("pio_job_matrix={}".format(json.dumps(pio_job_matrix)), file=fh)


# %%
if "GITHUB_WORKSPACE" not in os.environ.keys():
    try:
        print("Deleting artifact directory")
        shutil.rmtree(artifact_dir)
    except:
        pass
    try:
        print("Deleting downloaded jsons")
        os.remove(
            os.path.join(ci_path, "platformio_to_arduino_boards.json")
        )  # remove downloaded file
        os.rmdir(ci_path)  # remove dir if empty
    except:
        pass
    if downloaded_arduino_cli_config:
        try:
            print("Deleting default Arduino CLI file")
            os.remove(arduino_cli_config)  # remove downloaded file
            os.rmdir(ci_path)  # remove dir if empty
        except:
            pass
    if default_pio_config_file:
        try:
            print("Deleting default_pio_config_file")
            os.remove(pio_config_file)  # remove downloaded file
            os.rmdir(ci_path)  # remove dir if empty
        except:
            pass


# %%
# cSpell:ignore fqbn acli arduinocli jsons Tarrasch Wextra endgroup PCINT PIPESTATUS
