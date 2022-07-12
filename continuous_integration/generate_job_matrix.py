#!/usr/bin/env python
#%%
import json
import shutil
import re
from typing import List
from platformio.project.config import ProjectConfig
import re
import os
import copy

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

compilers = ["Arduino CLI", "PlatformIO"]
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


def get_example_folder(subfolder_name):
    return os.path.join(examples_path, subfolder_name)


def get_example_filepath(subfolder_name):
    ex_folder = get_example_folder(subfolder_name)
    ex_file = os.path.join(ex_folder, subfolder_name + ".ino")
    return ex_file


def create_arduino_cli_command(pio_env_name: str, code_subfolder: str) -> str:
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
        os.path.join(examples_path, code_subfolder),
    ]
    return " ".join(arduino_command_arguments)


def create_pio_ci_command(pio_env_file: str, pio_env: str, code_subfolder: str) -> str:
    pio_command_args = [
        "pio",
        "ci",
        # "--verbose",
        "--project-conf",
        pio_env_file,
        "--project-conf",
        pio_env_file,
        "--environment",
        pio_env,
        os.path.join(examples_path, code_subfolder),
    ]
    return " ".join(pio_command_args)


def add_log_to_command(command: str, group_title: str) -> List:
    command_list = []
    command_list.append("echo ::group::{}".format(group_title))
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


#%%
# set up outputs
arduino_job_matrix = []
pio_job_matrix = []
start_job_commands = "status=0"
end_job_commands = "exit $status"

#%%
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
            "command": " \n ".join(arduino_ex_commands + [end_job_commands]),
        }
    )
    pio_job_matrix.append(
        {
            "job_name": "{} - Platformio - Other Examples".format(pio_env),
            "command": " \n ".join(pio_ex_commands + [end_job_commands]),
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
    "^(?:#if|#elif) defined[\s\(](?P<flag1>BUILD_\w+)((?:[\s\n\\\)]*?\|\|[\s\n\\]*defined[\s\n\\\(]*?)(?P<flag_last>BUILD_\w+))*",
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


#%%
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
            "command": " \n ".join(arduino_modem_commands + [end_job_commands]),
        }
    )
    pio_job_matrix.append(
        {
            "job_name": "{} - PlatformIO - Modems".format(pio_env),
            "command": " \n ".join(pio_modem_commands + [end_job_commands]),
        }
    )

#%%
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
            "command": " \n ".join(arduino_pub_commands + [end_job_commands]),
        }
    )
    pio_job_matrix.append(
        {
            "job_name": "{} - PlatformIO - Publishers".format(pio_env),
            "command": " \n ".join(pio_pub_commands + [end_job_commands]),
        }
    )

#%%
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
            "command": " \n ".join(arduino_sensor_commands + [end_job_commands]),
        }
    )
    pio_job_matrix.append(
        {
            "job_name": "{} - PlatformIO - Sensors".format(pio_env),
            "command": " \n ".join(pio_sensor_commands + [end_job_commands]),
        }
    )

#%%
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
            "command": " \n ".join(arduino_loop_commands + [end_job_commands]),
        }
    )
    pio_job_matrix.append(
        {
            "job_name": "{} - PlatformIO - Loops".format(pio_env),
            "command": " \n ".join(pio_loop_commands + [end_job_commands]),
        }
    )

#%%
# Tack on a few more extra build configurations for the software serial libraries
for pio_env in pio_config.envs():
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
            "command": " \n ".join(arduino_serial_commands + [end_job_commands]),
        }
    )
    pio_job_matrix.append(
        {
            "job_name": "{} - PlatformIO - Serials".format(pio_env),
            "command": " \n ".join(pio_serial_commands + [end_job_commands]),
        }
    )


#%%
# Write out output
print("::set-output name=arduino_job_matrix::{}".format(json.dumps(arduino_job_matrix)))
json_out = open(os.path.join(artifact_dir, "arduino_job_matrix.json"), "w+")
json.dump(arduino_job_matrix, json_out, indent=2)
json_out.close()


print("::set-output name=pio_job_matrix::{}".format(json.dumps(pio_job_matrix)))
json_out = open(os.path.join(artifact_dir, "pio_job_matrix.json"), "w+")
json.dump(pio_job_matrix, json_out, indent=2)
json_out.close()


#%%
if "GITHUB_WORKSPACE" not in os.environ.keys():
    try:
        shutil.rmtree(artifact_dir)
    except:
        pass

# %%
