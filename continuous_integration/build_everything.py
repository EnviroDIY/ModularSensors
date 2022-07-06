#!/usr/bin/env python
#%%
# import json
# import yaml
import glob
from pathlib import Path
import random
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

#%%

if "GITHUB_WORKSPACE" in os.environ.keys():
    workspace_dir = os.environ.get("GITHUB_WORKSPACE")
else:
    fileDir = os.path.dirname(os.path.realpath("__file__"))
    workspace_dir = os.path.join(fileDir, "../")
    workspace_dir = os.path.abspath(os.path.realpath(workspace_dir))
# print("Program Directory: {}".format(fileDir))
examples_dir = "./examples/"
examples_path = os.path.join(workspace_dir, examples_dir)
examples_path = os.path.abspath(os.path.realpath(examples_path))
ci_dir = "./continuous_integration/"
ci_path = os.path.join(workspace_dir, ci_dir)
ci_path = os.path.abspath(os.path.realpath(ci_path))


if "GITHUB_WORKSPACE" in os.environ.keys():
    arduino_cli_config = os.path.join(ci_dir, "arduino_cli.yaml")
else:
    arduino_cli_config = os.path.join(ci_dir, "arduino_cli_local.yaml")

test_code_path = os.path.join(examples_path, "test_code")
file_to_compile = os.path.join(test_code_path, "test_code.ino")

#%%
# print("XML Directory: {}".format(fileDir))

all_examples = [
    f
    for f in os.listdir(examples_path)
    if os.path.isdir(os.path.join(examples_path, f))
    and f not in [".history", "logger_test", "YosemitechDO", "test_code"]
    and not f.startswith("test")
]
pio_config_file = os.path.join(ci_path, "platformio.ini")
menu_file = os.path.join(
    os.path.join(examples_path, "menu_a_la_carte"), "menu_a_la_carte.ino"
)
pio_config = ProjectConfig(pio_config_file)
pio_to_acli = {
    "mayfly": {
        "fqbn": "EnviroDIY:avr:envirodiy_mayfly",
        "extra_flags": [],
    },
    "megaatmega2560": {"fqbn": "arduino:avr:mega", "extra_flags": []},
    "zeroUSB": {
        "fqbn": "arduino:samd:mzero_bl",
        "extra_flags": [],
        # "extra_flags": [
        #     "-DARDUINO_SAMD_ZERO",
        #     "-D__SAMD21G18A__",
        #     "-DUSB_VID=0x2341",
        #     "-DUSB_PID=0x804d",
        #     "-DUSBCON",
        # ],
    },
    "adafruit_feather_m0": {
        "fqbn": "adafruit:samd:adafruit_feather_m0",
        "extra_flags": [],
        # "extra_flags": [
        #     "-DARDUINO_SAMD_ZERO",
        #     "-DARM_MATH_CM0PLUS",
        #     "-DADAFRUIT_FEATHER_M0",
        #     "-D__SAMD21G18A__",
        #     "-DUSB_VID=0x239A",
        #     "-DUSB_PID=0x800B",
        #     "-DUSBCON",
        #     "-DUSB_CONFIG_POWER=100",
        # ],
    },
    "sodaq_autonomo": {
        "fqbn": "SODAQ:samd:sodaq_autonomo",
        "extra_flags": [],
        # "extra_flags": [
        #     "-DVERY_LOW_POWER",
        #     "-D__SAMD21J18A__",
        #     "-DUSB_VID=0x2341",
        #     "-DUSB_PID=0x804d",
        #     "-DUSBCON",
        # ],
    },
}

#%% set flags
all_modem_flags = [
    "BUILD_MODEM_SIM_COM_SIM7080",
]
all_sensor_flags = [
    "NO_SENSORS",
]
all_publisher_flags = [
    "BUILD_PUB_ENVIRO_DIY_PUBLISHER",
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

#%% Create the matrix
menu_flag_matrix = []
for modem_flag in all_modem_flags:
    menu_flag_matrix.append([modem_flag, all_sensor_flags[0], all_publisher_flags[0]])
for sensor_flags in all_sensor_flags[1:]:
    menu_flag_matrix.append([all_modem_flags[0], sensor_flags, all_publisher_flags[0]])
for publisher_flag in all_publisher_flags[1:]:
    menu_flag_matrix.append([all_modem_flags[0], all_sensor_flags[0], publisher_flag])

#%%
full_build_matrix = []
for pio_env in pio_config.envs():
    matrix_item = {}
    matrix_item["pio_env_name"] = pio_env
    env_key = "env:{}".format(pio_env)
    matrix_item["use_pio"] = True

    if pio_env in ["mayfly", "mega", "arduino_zero", "adafruit_feather_m0"]:
        matrix_item["use_acli"] = True
    else:
        matrix_item["use_acli"] = False

    for example in all_examples:
        matrix_item["example"] = example
        if example == "menu_a_la_carte":
            for flag_set in menu_flag_matrix:
                matrix_item["in_file_defines"] = flag_set

                matrix_item["modem"] = flag_set[0].replace("BUILD_MODEM_", "")
                matrix_item["sensor"] = flag_set[1].replace("BUILD_SENSOR_", "")
                matrix_item["publisher"] = (
                    flag_set[2].replace("BUILD_PUB_", "").replace("_PUBLISHER", "")
                )
                if (
                    matrix_item["use_acli"]
                    or (
                        pio_env == "sdi12_non_concurrent"
                        and (
                            matrix_item["sensor"].startswith("BUILD_SENSOR_DECAGON")
                            or matrix_item["sensor"].startswith("BUILD_SENSOR_METER")
                            or matrix_item["sensor"]
                            in [
                                "BUILD_SENSOR_CAMPBELL_CLARI_VUE10",
                                "BUILD_SENSOR_CAMPBELL_RAINVUE10",
                                "BUILD_SENSOR_IN_SITU_RDO",
                                "BUILD_SENSOR_IN_SITU_TROLL_SDI12A",
                                "BUILD_SENSOR_ZEBRA_TECH_D_OPTO",
                            ]
                        )
                    )
                    or (
                        pio_env == "software_wire"
                        and (
                            matrix_item["sensor"]
                            in [
                                "BUILD_SENSOR_PALEO_TERRA_REDOX",
                                "BUILD_SENSOR_RAIN_COUNTER_I2C",
                            ]
                        )
                    )
                    or (
                        pio_env == "ads1015"
                        and (
                            matrix_item["sensor"]
                            in [
                                "BUILD_SENSOR_APOGEE_SQ212",
                                "BUILD_SENSOR_CAMPBELL_OBS3",
                                "BUILD_SENSOR_TIADS1X15",
                                "BUILD_SENSOR_TURNER_CYCLOPS",
                            ]
                        )
                    )
                    or (
                        pio_env in ["AltSoftSerial", "NeoSWSerial", "software_serial"]
                        and (
                            matrix_item["sensor"]
                            in [
                                "BUILD_SENSOR_MAX_BOTIX_SONAR",
                                "BUILD_SENSOR_YOSEMITECH_Y504",
                            ]
                        )
                    )
                ):
                    full_build_matrix.append(matrix_item)
        else:
            if matrix_item["use_acli"]:
                matrix_item["in_file_defines"] = []
                matrix_item["modem"] = ""
                matrix_item["sensor"] = ""
                matrix_item["publisher"] = ""
                full_build_matrix.append(copy.deepcopy(matrix_item))

#%%
def prepare_example(example, in_file_defines):
    textfile = open(example, "r")
    as_written = textfile.read()
    textfile.close()

    rnum = random.randint(0, 100000)

    if not os.path.exists(os.path.join(examples_path, "test_code_{}".format(rnum))):
        os.makedirs(os.path.join(examples_path, "test_code_{}".format(rnum)))

    prepared_example = open(
        os.path.join(
            os.path.join(
                os.path.join(examples_path, "test_code_{}".format(rnum)),
                "test_code_{}.ino".format(rnum),
            )
        ),
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

    return rnum


#%%
compile_commands = []
for matrix_item in full_build_matrix:
    compile_command = copy.deepcopy(matrix_item)
    ex_num = prepare_example(
        example=os.path.join(
            os.path.join(examples_path, example), "{}.ino".format(example)
        ),
        in_file_defines=matrix_item["in_file_defines"],
    )
    env_key = "env:{}".format(matrix_item["pio_env_name"])

    my_env = os.environ.copy()
    my_env["PLATFORMIO_SRC_DIR"] = os.path.join(
        examples_path, "test_code_{}".format(ex_num)
    )
    my_env["PLATFORMIO_DEFAULT_ENVS"] = matrix_item["pio_env_name"]
    compile_command["env"] = my_env

    arduino_command_arguments = [
        "arduino-cli",
        "compile",
        # "--verbose",
        "--warnings",
        "all",
        # "--clean",
        "--config-file",
        arduino_cli_config,
        "--format",
        "text",
        # "--log-file",
        # os.path.join(
        #     os.path.join(examples_path, "test_code_{}".format(ex_num)),
        #     "test_code_{}.log".format(ex_num),
        # ),
        # "--log-level",
        # "warn",
        # "--log-format",
        # "json",
        "--fqbn",
        pio_to_acli[pio_config.get(env_key, "board")]["fqbn"],
    ]

    if len(pio_to_acli[pio_config.get(env_key, "board")]["extra_flags"]) > 0:
        arduino_command_arguments.append("--build-property")
        arduino_command_arguments.append(
            "build.extra_flags={extra_flags}".format(
                extra_flags=" ".join(
                    pio_to_acli[pio_config.get(env_key, "board")]["extra_flags"]
                )
            )
        )

    arduino_command_arguments.append(
        os.path.join(
            os.path.join(examples_path, "test_code_{}".format(ex_num)),
            "test_code_{}.ino".format(ex_num),
        )
    )
    if compile_command["use_acli"]:
        compile_command["Compiler"] = "Arduino CLI"
        compile_command["args"] = copy.deepcopy(arduino_command_arguments)
        compile_commands.append(copy.deepcopy(compile_command))

    pio_command_args = [
        "pio",
        "ci",
        # "--verbose",
        "--project-conf",
        pio_config_file,
        os.path.join(examples_path, "test_code_{}".format(ex_num)),
    ]
    if compile_command["use_pio"]:
        compile_command["Compiler"] = "PlatformIO"
        compile_command["args"] = copy.deepcopy(pio_command_args)
        compile_commands.append(copy.deepcopy(compile_command))

#%%
# this starts all of the compile jobs at once
# https://stackoverflow.com/questions/16450788/python-running-subprocess-in-parallel

# processes = []
# for compile_command in compile_commands:
#     stdout_temp = tempfile.TemporaryFile()
#     stderr_temp = tempfile.TemporaryFile()
#     compile_process = subprocess.Popen(
#         compile_command["args"],
#         env=compile_command["env"],
#         # text=True,
#         # capture_output=True,
#         cwd=workspace_dir,
#         # stdout=stdout_temp,
#         # stderr=stderr_temp,
#         stdout=subprocess.PIPE,
#         stderr=subprocess.PIPE,
#     )
#     # processes.append(
#     #     (compile_process, stdout_temp, stderr_temp, copy.deepcopy(compile_command))
#     # )
#     processes.append((compile_process, copy.deepcopy(compile_command)))

limit = 4
groups = [
    (
        (
            subprocess.Popen(
                compile_command["args"],
                env=compile_command["env"],
                # text=True,
                # capture_output=True,
                cwd=workspace_dir,
                # stdout=stdout_temp,
                # stderr=stderr_temp,
                stdout=subprocess.PIPE,
                stderr=subprocess.PIPE,
            ),
            copy.deepcopy(compile_command),
        )
        for compile_command in compile_commands
    )
] * limit  # itertools' grouper recipe

#%%
results = []
# for compile_process, stdout_temp, stderr_temp, compile_command in processes:
for processes in zip_longest(*groups):  # run len(processes) == limit at a time
    for compile_process, compile_command in filter(None, processes):
        print(
            "::group::{}-{}-{}-{}-{}-{}".format(
                compile_command["Compiler"],
                compile_command["pio_env_name"],
                compile_command["example"],
                compile_command["modem"],
                compile_command["sensor"],
                compile_command["publisher"],
            )
        )
        for c in iter(lambda: compile_process.stdout.read(1), b""):
            # sys.stdout.buffer.write(c)
            print(c.decode("utf-8"), end="")
        # while compile_process.poll() is None:
        #     stdout_temp.seek(0, os.SEEK_END)
        #     # read last line of file
        #     line = stdout_temp.readline()
        #     if line != "":
        #         print(line)
        #     # print(stdout_temp.read().decode("utf-8"))
        # stdout_temp.close()
        print("::endgroup::")
        # stderr_temp.seek(0)
        # compile_errors = stderr_temp.read().decode("utf-8")
        compile_errors = compile_process.stderr.read().decode("utf-8")
        print(compile_errors)
        # stderr_temp.close()
        print(
            "\n--------------------------------------------------------------------------------------------------------------------\n"
        )
        results.append(
            {
                "Compiler": compile_command["Compiler"],
                "Example": compile_command["example"],
                "Board": pio_config.get(
                    "env:{}".format(compile_command["pio_env_name"]), "board"
                ),
                "build_flags": pio_config.get(
                    "env:{}".format(compile_command["pio_env_name"]), "build_flags"
                )
                if compile_command["Compiler"] == "PlatformIO"
                else [],
                "compile_result": compile_process.returncode,
                "cause": compile_errors if compile_process.returncode else "",
                "Modem": compile_command["modem"],
                "Sensor": compile_command["sensor"],
                "Publisher": compile_command["publisher"],
            }
        )
#%%
temp_dirs = glob.glob(examples_path + "\\*test_code*", recursive=True)
for temp_dir in temp_dirs:
    try:
        shutil.rmtree(temp_dir)
    except:
        pass


#%%
frame_results = pd.DataFrame(results)
frame_results["Build Flags"] = frame_results["build_flags"].apply(
    lambda res: "".join(
        ["<li>{}</li>".format(flag.replace("-D", "").strip()) for flag in res]
    )
)
frame_results["Compiler Result"] = np.where(
    frame_results["compile_result"], ":x:", ":white_check_mark:"
)
frame_results["Failed"] = frame_results["compile_result"].astype(bool)
frame_results["Run"] = 1
frame_results["Failure Cause"] = (
    frame_results["cause"].str.replace("\r", "").str.replace("\n", "<br/><br/>")
)
total_failures = len(frame_results.loc[frame_results["Failed"]].index)


#%%
markdown_results = open(os.path.join(workspace_dir, "compile_results.md"), "w+")
markdown_results.write("## Result of compiling all examples and configurations\n\n")
markdown_results.write("{} total failures\n".format(total_failures))
markdown_results.write("### Failures by Compiler\n\n")
markdown_results.write(
    frame_results.groupby("Compiler")[["Run", "Failed"]].sum().to_markdown()
)
markdown_results.write("\n\n")
markdown_results.write("### Failures by Board\n\n")
markdown_results.write(
    frame_results.groupby("Board")[["Run", "Failed"]].sum().to_markdown()
)
markdown_results.write("\n\n")
markdown_results.write("### Failures by Example\n\n")
markdown_results.write(
    frame_results.groupby("Example")[["Run", "Failed"]].sum().to_markdown()
)
markdown_results.write("\n\n")
markdown_results.write(
    frame_results[
        [
            "Compiler",
            "Board",
            "Example",
            "Modem",
            "Sensor",
            "Publisher",
            "Build Flags",
            "Compiler Result",
            # "Failure Cause",
        ]
    ].to_markdown(index=False)
)
markdown_results.close()

# %%
if total_failures > 0:
    fail_message = "::error::{} Total compilation failures".format(total_failures)
    print(fail_message)
    sys.exit(fail_message)
