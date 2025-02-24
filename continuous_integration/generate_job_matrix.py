#!/usr/bin/env python
# %%
import json
import shutil
import re
from typing import List
from platformio.project.config import ProjectConfig
import re
import os
import copy
import requests

# %%
# set verbose
use_verbose = False
if "RUNNER_DEBUG" in os.environ.keys() and os.environ["RUNNER_DEBUG"] == "1":
    use_verbose = True


# %%
# Some working directories

# The workspace directory
if "GITHUB_WORKSPACE" in os.environ.keys():
    workspace_dir = os.environ.get("GITHUB_WORKSPACE")
else:
    workspace_dir = os.getcwd()
workspace_path = os.path.abspath(os.path.realpath(workspace_dir))
print(f"Workspace Path: {workspace_path}")

# %%
# The examples directory
examples_dir = "./examples/"
examples_path = os.path.join(workspace_dir, examples_dir)
examples_path = os.path.abspath(os.path.realpath(examples_path))
print(f"Examples Path: {examples_path}")

# The continuous integration directory
ci_dir = "./continuous_integration/"
ci_path = os.path.join(workspace_dir, ci_dir)
ci_path = os.path.abspath(os.path.realpath(ci_path))
print(f"Continuous Integration Path: {ci_path}")
if not os.path.exists(ci_path):
    print(f"Creating the directory for CI: {ci_path}")
    os.makedirs(ci_path, exist_ok=True)

# A directory of files to save and upload as artifacts to use in future jobs
artifact_dir = os.path.join(
    os.path.join(workspace_dir, "continuous_integration_artifacts")
)
artifact_path = os.path.abspath(os.path.realpath(artifact_dir))
print(f"Artifact Path: {artifact_path}")
if not os.path.exists(artifact_dir):
    print(f"Creating the directory for artifacts: {artifact_path}")
    os.makedirs(artifact_dir)

compilers = ["Arduino CLI", "PlatformIO"]
non_acli_build_flag = [
    "-Wall",
    "-Wextra",
    "-D SDI12_EXTERNAL_PCINT",
    "-D NEOSWSERIAL_EXTERNAL_PCINT",
]


# %%
# The locations of some important files

# The massive "menu" example
menu_example_name = "menu_a_la_carte"
menu_file_path = os.path.join(
    os.path.join(examples_path, menu_example_name), menu_example_name + ".ino"
)


# %%
# Pull files to convert between boards and platforms and FQBNs
response = requests.get(
    "https://raw.githubusercontent.com/EnviroDIY/workflows/main/scripts/platformio_to_arduino_boards.json"
)
with open(os.path.join(ci_path, "platformio_to_arduino_boards.json"), "wb") as f:
    f.write(response.content)
# Translation between board names on PlatformIO and the Arduino CLI
with open(os.path.join(ci_path, "platformio_to_arduino_boards.json")) as f:
    pio_to_acli = json.load(f)

# Find all of the non-menu examples
non_menu_examples = [
    f
    for f in os.listdir(examples_path)
    if os.path.isdir(os.path.join(examples_path, f))
    and f not in [".history", "logger_test", menu_example_name]
]

# %%
# read configurations based on existing files and environment variables

# Arduino CLI configuration
# Always use the generic one from the shared workflow repository
if "GITHUB_WORKSPACE" in os.environ.keys():
    arduino_cli_config = os.path.join(ci_path, "arduino_cli.yaml")
    if not os.path.isfile(arduino_cli_config):
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
    arduino_cli_config = os.path.join(ci_dir, "arduino_cli_local.yaml")

# PlatformIO configuration
pio_config_file = os.path.join(ci_path, "platformio.ini")
pio_extra_config_file = os.path.join(ci_path, "platformio_extra_flags.ini")
pio_config = ProjectConfig(pio_config_file)
pio_extra_config = ProjectConfig(pio_extra_config_file)


def get_example_folder(subfolder_name):
    return os.path.join(examples_path, subfolder_name)


def get_example_filepath(subfolder_name):
    ex_folder = get_example_folder(subfolder_name)
    ex_file = os.path.join(ex_folder, subfolder_name + ".ino")
    return ex_file


def create_arduino_cli_command(pio_env_name: str, code_subfolder: str) -> str:
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
        "text",
        "--fqbn",
        pio_to_acli[pio_config.get("env:{}".format(pio_env_name), "board")]["fqbn"],
    ]
    for pio_build_flag in pio_config.get("env:{}".format(pio_env_name), "build_flags"):
        if pio_build_flag not in non_acli_build_flag:
            arduino_command_args += [
                "--build-property",
                f'"build.extra_flags=\\"{pio_build_flag}\\""',
            ]
    arduino_command_args += [
        f'"{os.path.join(examples_path, code_subfolder)}"',
    ]
    return " ".join(arduino_command_args)


def create_pio_ci_command(pio_env_file: str, pio_env: str, code_subfolder: str) -> str:
    pio_command_args = [
        "pio",
        "ci",
    ]
    if use_verbose:
        pio_command_args += ["--verbose"]
    pio_command_args += [
        "--project-conf",
        f'"{pio_env_file}"',
        "--environment",
        pio_env,
        f'"{os.path.join(examples_path, code_subfolder)}"',
    ]
    return " ".join(pio_command_args)


def add_log_to_command(command: str, group_title: str) -> List:
    command_list = []
    command_list.append("\necho ::group::{}".format(group_title))
    command_list.append(command + " 2>&1 | tee output.log")
    command_list.append("result_code=${PIPESTATUS[0]}")
    command_list.append(
        'if [ "$result_code" -eq "0" ]; then echo " - {title} :white_check_mark:" >> $GITHUB_STEP_SUMMARY; else echo " - {title} :x:" >> $GITHUB_STEP_SUMMARY; fi'.format(
            title=group_title
        )
    )
    command_list.append(
        'if [ "$result_code" -eq "0" ] && [ "$status" -eq "0" ]; then status=0; else status=1; fi'
    )
    command_list.append("echo ::endgroup::")
    command_list.append(
        'if [ "$result_code" -eq "0" ]; then echo -e "\\e[32m{title} successfully compiled\\e[0m"; else echo -e "\\e[31m{title} failed to compile\\e[0m"; fi'.format(
            title=group_title
        )
    )
    return command_list


def create_logged_command(
    compiler: str,
    group_title: str,
    code_subfolder: str,
    pio_env: str,
    pio_env_file: str = pio_config_file,
):
    output_commands = []
    lower_compiler = compiler.lower().replace(" ", "").strip()
    if lower_compiler == "platformio":
        build_command = create_pio_ci_command(pio_env_file, pio_env, code_subfolder)
    elif lower_compiler == "arduinocli":
        build_command = create_arduino_cli_command(pio_env, code_subfolder)
    else:
        build_command = ""

    command_with_log = add_log_to_command(build_command, group_title)
    output_commands.extend(command_with_log)

    return copy.deepcopy(output_commands)


def snake_to_camel(snake_str):
    crop_strings = [
        "BUILD_MODEM_",
        "BUILD_SENSOR_",
        "BUILD_PUB_",
        "_PUBLISHER",
        "BUILD_TEST_",
    ]
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


# %%
# set up outputs
arduino_job_matrix = []
pio_job_matrix = []
start_job_commands = "status=0"
end_job_commands = "\n\nexit $status"

# %%
# Create job info for the basic examples
# Use one job per board with one command per example
for pio_env in pio_config.envs():
    arduino_ex_commands = [
        start_job_commands,
        # 'echo "## [All Examples on {} with the Arduino CLI](https://github.com/EnviroDIY/ModularSensors/runs/$ACTION_RUN_ID?check_suite_focus=true#step:10:1)" >> $GITHUB_STEP_SUMMARY'.format(
        #     pio_env
        # ),
    ]
    pio_ex_commands = [
        start_job_commands,
        # 'echo "## [All Examples on {} with PlatformIO](https://github.com/EnviroDIY/ModularSensors/runs/$ACTION_RUN_ID?check_suite_focus=true#step:10:1)" >> $GITHUB_STEP_SUMMARY'.format(
        #     pio_env
        # ),
    ]

    for example in non_menu_examples:
        for compiler, command_list in zip(
            compilers, [arduino_ex_commands, pio_ex_commands]
        ):
            if compiler == "Arduino CLI" and example == "data_saving":
                # skip this one, it's too big
                pass
            else:
                command_list.extend(
                    create_logged_command(
                        compiler=compiler,
                        group_title=example,
                        code_subfolder=example,
                        pio_env=pio_env,
                    )
                )

    arduino_job_matrix.append(
        {
            "job_name": "{} - Arduino - Other Examples".format(pio_env),
            "command": "\n".join(arduino_ex_commands + [end_job_commands]),
        }
    )
    pio_job_matrix.append(
        {
            "job_name": "{} - Platformio - Other Examples".format(pio_env),
            "command": "\n".join(pio_ex_commands + [end_job_commands]),
        }
    )


# %%
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
    prepped_ex_folder = os.path.join(artifact_path, preped_folder_name)
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
        artifact_path, "test_ci_" + "_".join(added_envs) + ".ini"
    )

    for added_env in added_envs:
        env_key = "env:{}".format(added_env)
        new_config._parser.add_section(env_key)
        for option in pio_extra_config.options(env_key):
            new_config.set(env_key, option, pio_extra_config.get(env_key, option))

    new_config.save(new_config_path)
    return new_config_path


# %% read build flags out of the menu-a-la-cart example
# Pattern for flags in the menu-a-la-cart example
pattern = re.compile(
    r"^(?:#if|#elif) defined[\s\(](?P<flag1>BUILD_\w+)((?:[\s\n\\\)]*?\|\|[\s\n\\]*defined[\s\n\\\(]*?)(?P<flag_last>BUILD_\w+))*",
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


# %%
# Create jobs for all of the modems together
for pio_env in pio_config.envs():
    arduino_modem_commands = [
        start_job_commands,
        # 'echo "## [All Modems on {} with the Arduino CLI](https://github.com/EnviroDIY/ModularSensors/runs/$ACTION_RUN_ID?check_suite_focus=true#step:10:1)" >> $GITHUB_STEP_SUMMARY'.format(
        #     pio_env
        # ),
    ]
    pio_modem_commands = [
        start_job_commands,
        # 'echo "## [All Modems on {} with Platformio](https://github.com/EnviroDIY/ModularSensors/runs/$ACTION_RUN_ID?check_suite_focus=true#step:10:1)" >> $GITHUB_STEP_SUMMARY'.format(
        #     pio_env
        # ),
    ]

    for modem_flag in all_modem_flags:
        used_modem = snake_to_camel(modem_flag)
        prepped_ex_folder, _ = prepare_example(
            menu_example_name, [modem_flag, all_sensor_flags[0], all_publisher_flags[0]]
        )
        for compiler, command_list in zip(
            compilers, [arduino_modem_commands, pio_modem_commands]
        ):
            command_list.extend(
                create_logged_command(
                    compiler=compiler,
                    group_title=used_modem,
                    code_subfolder=prepped_ex_folder,
                    pio_env=pio_env,
                )
            )

    arduino_job_matrix.append(
        {
            "job_name": "{} - Arduino - Modems".format(pio_env),
            "command": "\n".join(arduino_modem_commands + [end_job_commands]),
        }
    )
    pio_job_matrix.append(
        {
            "job_name": "{} - PlatformIO - Modems".format(pio_env),
            "command": "\n".join(pio_modem_commands + [end_job_commands]),
        }
    )

# %%
# Create jobs for all of the publishers together
for pio_env in pio_config.envs():
    arduino_pub_commands = [
        start_job_commands,
        # 'echo "## [All Publishers on {} with the Arduino CLI](https://github.com/EnviroDIY/ModularSensors/runs/$ACTION_RUN_ID?check_suite_focus=true#step:10:1)" >> $GITHUB_STEP_SUMMARY'.format(
        #     pio_env
        # ),
    ]
    pio_pub_commands = [
        start_job_commands,
        # 'echo "## [All Publishers on {} with Platformio](https://github.com/EnviroDIY/ModularSensors/runs/$ACTION_RUN_ID?check_suite_focus=true#step:10:1)" >> $GITHUB_STEP_SUMMARY'.format(
        #     pio_env
        # ),
    ]

    for publisher_flag in all_publisher_flags[1:]:
        used_publisher = snake_to_camel(publisher_flag)
        prepped_ex_folder, _ = prepare_example(
            menu_example_name, [all_modem_flags[0], all_sensor_flags[0], publisher_flag]
        )
        for compiler, command_list in zip(
            compilers, [arduino_pub_commands, pio_pub_commands]
        ):
            command_list.extend(
                create_logged_command(
                    compiler=compiler,
                    group_title=used_publisher,
                    code_subfolder=prepped_ex_folder,
                    pio_env=pio_env,
                )
            )

    arduino_job_matrix.append(
        {
            "job_name": "{} - Arduino - Publishers".format(pio_env),
            "command": "\n".join(arduino_pub_commands + [end_job_commands]),
        }
    )
    pio_job_matrix.append(
        {
            "job_name": "{} - PlatformIO - Publishers".format(pio_env),
            "command": "\n".join(pio_pub_commands + [end_job_commands]),
        }
    )

# %%
# Create jobs for all of the sensors together
# The sensors are a bit different because we run extra PlatformIO enviroments for some of the sensors
for pio_env in pio_config.envs():
    arduino_sensor_commands = [
        start_job_commands,
        # 'echo "## [All Sensors on {} with the Arduino CLI](https://github.com/EnviroDIY/ModularSensors/runs/$ACTION_RUN_ID?check_suite_focus=true#step:10:1)" >> $GITHUB_STEP_SUMMARY'.format(
        #     pio_env
        # ),
    ]
    pio_sensor_commands = [
        start_job_commands,
        # 'echo "## [All Sensors on {} with Platformio](https://github.com/EnviroDIY/ModularSensors/runs/$ACTION_RUN_ID?check_suite_focus=true#step:10:1)" >> $GITHUB_STEP_SUMMARY'.format(
        #     pio_env
        # ),
    ]

    for sensor_flag in all_sensor_flags[1:]:
        used_sensor = snake_to_camel(sensor_flag)
        prepped_ex_folder, _ = prepare_example(
            menu_example_name, [all_modem_flags[0], sensor_flag, all_publisher_flags[0]]
        )
        for compiler, command_list in zip(
            compilers,
            [arduino_sensor_commands, pio_sensor_commands],
        ):
            command_list.extend(
                create_logged_command(
                    compiler=compiler,
                    group_title=used_sensor,
                    code_subfolder=prepped_ex_folder,
                    pio_env=pio_env,
                )
            )
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
            pio_sensor_commands.extend(
                create_logged_command(
                    compiler="PlatformIO",
                    group_title=used_sensor + " non-concurrent",
                    code_subfolder=prepped_ex_folder,
                    pio_env="sdi12_non_concurrent",
                    pio_env_file=pio_build_config,
                )
            )
        if used_sensor in ["PaleoTerraRedox", "RainCounterI2C"]:
            pio_build_config = extend_pio_config(["software_wire"])
            pio_sensor_commands.extend(
                create_logged_command(
                    compiler="PlatformIO",
                    group_title=used_sensor + " software wire",
                    code_subfolder=prepped_ex_folder,
                    pio_env="software_wire",
                    pio_env_file=pio_build_config,
                )
            )
        if used_sensor in [
            "ApogeeSq212",
            "CampbellObs3",
            "Tiads1X15",
            "TurnerCyclops",
        ]:
            pio_build_config = extend_pio_config(["ads1015"])
            pio_sensor_commands.extend(
                create_logged_command(
                    compiler="PlatformIO",
                    group_title=used_sensor + " ADS1015",
                    code_subfolder=prepped_ex_folder,
                    pio_env="ads1015",
                    pio_env_file=pio_build_config,
                )
            )

    arduino_job_matrix.append(
        {
            "job_name": "{} - Arduino - Sensors".format(pio_env),
            "command": "\n".join(arduino_sensor_commands + [end_job_commands]),
        }
    )
    pio_job_matrix.append(
        {
            "job_name": "{} - PlatformIO - Sensors".format(pio_env),
            "command": "\n".join(pio_sensor_commands + [end_job_commands]),
        }
    )

# %%
# Tack on a few extra build configurations for the menu example
for pio_env in pio_config.envs():
    arduino_loop_commands = [
        start_job_commands,
        # 'echo "## [Extra Loops on {} with the Arduino CLI](https://github.com/EnviroDIY/ModularSensors/runs/$ACTION_RUN_ID?check_suite_focus=true#step:10:1)" >> $GITHUB_STEP_SUMMARY'.format(
        #     pio_env
        # ),
    ]
    pio_loop_commands = [
        start_job_commands,
        # 'echo "## [Extra Loops on {} with Platformio](https://github.com/EnviroDIY/ModularSensors/runs/$ACTION_RUN_ID?check_suite_focus=true#step:10:1)" >> $GITHUB_STEP_SUMMARY'.format(
        #     pio_env
        # ),
    ]

    for loop_flag in [
        "BUILD_TEST_COMPLEX_LOOP",
        "BUILD_TEST_CREATE_IN_ARRAY",
        "BUILD_TEST_SEPARATE_UUIDS",
    ]:
        used_loop = snake_to_camel(loop_flag)
        prepped_ex_folder, _ = prepare_example(
            menu_example_name,
            [
                loop_flag,
                all_modem_flags[0],
                all_sensor_flags[0],
                all_publisher_flags[0],
            ],
        )
        for compiler, command_list in zip(
            compilers, [arduino_loop_commands, pio_loop_commands]
        ):
            command_list.extend(
                create_logged_command(
                    compiler=compiler,
                    group_title=used_loop,
                    code_subfolder=prepped_ex_folder,
                    pio_env=pio_env,
                )
            )

    arduino_job_matrix.append(
        {
            "job_name": "{} - Arduino - Loops".format(pio_env),
            "command": "\n".join(arduino_loop_commands + [end_job_commands]),
        }
    )
    pio_job_matrix.append(
        {
            "job_name": "{} - PlatformIO - Loops".format(pio_env),
            "command": "\n".join(pio_loop_commands + [end_job_commands]),
        }
    )

# %%
# Tack on a few more extra build configurations for the software serial libraries
for pio_env in ["mayfly"]:
    arduino_serial_commands = [
        start_job_commands,
        # 'echo "## [Extra Serials on {} with the Arduino CLI](https://github.com/EnviroDIY/ModularSensors/runs/$ACTION_RUN_ID?check_suite_focus=true#step:10:1)" >> $GITHUB_STEP_SUMMARY'.format(
        #     pio_env
        # ),
    ]
    pio_serial_commands = [
        start_job_commands,
        # 'echo "## [Extra Serials on {} with Platformio](https://github.com/EnviroDIY/ModularSensors/runs/$ACTION_RUN_ID?check_suite_focus=true#step:10:1)" >> $GITHUB_STEP_SUMMARY'.format(
        #     pio_env
        # ),
    ]

    for serial_flag in [
        "BUILD_TEST_ALTSOFTSERIAL",
        "BUILD_TEST_NEOSWSERIAL",
        "BUILD_TEST_SOFTSERIAL",
    ]:
        used_serial = snake_to_camel(serial_flag)
        prepped_ex_folder, _ = prepare_example(
            menu_example_name,
            [
                serial_flag,
                all_modem_flags[0],
                "BUILD_SENSOR_MAX_BOTIX_SONAR",
                "BUILD_SENSOR_YOSEMITECH_Y504",
                all_publisher_flags[0],
            ],
        )
        for compiler, command_list in zip(
            compilers, [arduino_serial_commands, pio_serial_commands]
        ):
            command_list.extend(
                create_logged_command(
                    compiler=compiler,
                    group_title=used_serial,
                    code_subfolder=prepped_ex_folder,
                    pio_env=pio_env,
                )
            )

    arduino_job_matrix.append(
        {
            "job_name": "{} - Arduino - Serials".format(pio_env),
            "command": "\n".join(arduino_serial_commands + [end_job_commands]),
        }
    )
    pio_job_matrix.append(
        {
            "job_name": "{} - PlatformIO - Serials".format(pio_env),
            "command": "\n".join(pio_serial_commands + [end_job_commands]),
        }
    )


# %%
# Convert commands in the matrix into bash scripts
for matrix_job in arduino_job_matrix + pio_job_matrix:
    bash_file_name = matrix_job["job_name"].replace(" ", "") + ".sh"
    print(f"Writing bash file to {os.path.join(artifact_path, bash_file_name)}")
    bash_out = open(os.path.join(artifact_path, bash_file_name), "w+")
    bash_out.write("#!/bin/bash\n\n")
    bash_out.write(
        """
set -e # Exit with nonzero exit code if anything fails
if [ "$RUNNER_DEBUG" = "1" ]; then
    echo "Enabling debugging!"
    set -v # Prints shell input lines as they are read.
    set -x # Print command traces before executing command.
fi

"""
    )
    bash_out.write(matrix_job["command"])
    bash_out.close()
    matrix_job["script"] = os.path.join(artifact_path, bash_file_name)

# Remove the command from the dictionaries before outputting them
for items in arduino_job_matrix + pio_job_matrix:
    if "command" in items:
        del items["command"]


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
        shutil.rmtree(artifact_dir)
    except:
        pass

# %%
