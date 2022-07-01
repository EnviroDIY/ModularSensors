#!/usr/bin/env python
#%%
# import json
# import yaml
from pathlib import Path
import sys
import re
import subprocess
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

test_code_path = os.path.join(examples_path, "test_code")
file_to_compile = os.path.join(test_code_path, "test_code.ino")


# print("XML Directory: {}".format(fileDir))

all_examples = [
    f
    for f in os.listdir(examples_path)
    if os.path.isdir(os.path.join(examples_path, f))
    and f not in [".history", "logger_test", "YosemitechDO", "test_code"]
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
        "extra_flags": [
            "-DARDUINO_SAMD_ZERO",
            "-D__SAMD21G18A__",
            "-DUSB_VID=0x2341",
            "-DUSB_PID=0x804d",
            "-DUSBCON",
        ],
    },
    "adafruit_feather_m0": {
        "fqbn": "adafruit:samd:adafruit_feather_m0",
        "extra_flags": [
            "-DARDUINO_SAMD_ZERO",
            "-DARM_MATH_CM0PLUS",
            "-DADAFRUIT_FEATHER_M0",
            "-D__SAMD21G18A__",
            "-DUSB_VID=0x239A",
            "-DUSB_PID=0x800B",
            "-DUSBCON",
            "-DUSB_CONFIG_POWER=100",
        ],
    },
    "sodaq_autonomo": {
        "fqbn": "SODAQ:samd:sodaq_autonomo",
        "extra_flags": [
            "-DVERY_LOW_POWER",
            "-D__SAMD21J18A__",
            "-DUSB_VID=0x2341",
            "-DUSB_PID=0x804d",
            "-DUSBCON",
        ],
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
    print(match.group("flag1"))
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
    env_key = "env:{}".format(pio_env)
    matrix_item["pio_env_name"] = pio_env
    matrix_item["fqbn"] = pio_to_acli[pio_config.get(env_key, "board")]["fqbn"]
    in_file_defines = [
        flag.replace("-D", "").strip()
        for flag in pio_config.get(env_key, "build_flags")
    ]

    if len(pio_to_acli[pio_config.get(env_key, "board")]["extra_flags"]) > 0:
        matrix_item[
            "arduino_build_properties"
        ] = ' --build-property "build.extra_flags={extra_flags}"'.format(
            extra_flags=" ".join(
                pio_to_acli[pio_config.get(env_key, "board")]["extra_flags"]
            )
        )
    else:
        matrix_item["arduino_build_properties"] = ""

    for example in all_examples:
        matrix_item["example"] = example
        if example == "menu_a_la_carte":
            for flag_set in menu_flag_matrix:
                matrix_item["in_file_defines"] = in_file_defines + flag_set

                matrix_item["modem"] = flag_set[0].replace("BUILD_MODEM_", "")
                matrix_item["sensor"] = flag_set[1].replace("BUILD_SENSOR_", "")
                matrix_item["publisher"] = (
                    flag_set[2].replace("BUILD_PUB_", "").replace("_PUBLISHER", "")
                )
                full_build_matrix.append(matrix_item)
        else:
            if pio_env not in ["software_wire", "complex_loop", "non_concurrent"]:
                matrix_item["in_file_defines"] = in_file_defines
                matrix_item["modem"] = ""
                matrix_item["sensor"] = ""
                matrix_item["publisher"] = ""
                full_build_matrix.append(copy.deepcopy(matrix_item))

#%%
def prepare_example(example, in_file_defines):
    textfile = open(example, "r")
    as_written = textfile.read()
    textfile.close()

    if not os.path.exists(test_code_path):
        os.makedirs(test_code_path)

    prepared_example = open(file_to_compile, "w+")
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


results = []

#%%
for matrix_item in full_build_matrix:
    print(
        "\n\nNow building {example} for {env} with these in-file defines: {defines}".format(
            example=matrix_item["example"],
            env=matrix_item["pio_env_name"],
            defines=matrix_item["in_file_defines"],
        )
    )
    prepare_example(
        example=os.path.join(
            os.path.join(examples_path, example), "{}.ino".format(example)
        ),
        in_file_defines=matrix_item["in_file_defines"],
    )
    cli_result = subprocess.run(
        [
            "arduino-cli",
            "compile",
            "--clean",
            '--config-file "{}"'.format(os.path.join(ci_dir, "arduino_cli.yaml")),
            "--format text",
            '--log-file "{}"'.format(
                os.path.join(workspace_dir, "arduino_cli_log.log")
            ),
            "--log-level warn",
            "--log-format json",
            "{}".format(matrix_item["arduino_build_properties"]),
            "--fqbn {}".format(matrix_item["fqbn"]),
            '"{file_to_compile}"'.format(file_to_compile=file_to_compile),
        ],
        text=True,
        capture_output=True,
        cwd=workspace_dir,
    )
    results.append(
        {
            "Compiler": "Arduino CLI",
            "Example": "menu_a_la_carte",
            "Board": pio_config.get(env_key, "board"),
            "build_flags": pio_config.get(env_key, "build_flags"),
            "compile_result": cli_result.returncode,
            "cause": cli_result.stderr if cli_result.returncode else "",
            "Modem": matrix_item["modem"],
            "Sensor": matrix_item["sensor"],
            "Publisher": matrix_item["publisher"],
        }
    )
    print(cli_result.stdout)
    print(cli_result.stderr)

    my_env = os.environ.copy()
    my_env["PLATFORMIO_SRC_DIR"] = test_code_path
    my_env["PLATFORMIO_DEFAULT_ENVS"] = matrix_item["pio_env_name"]
    pio_result = subprocess.run(
        [
            "pio",
            "run",
            "--environment {}".format(matrix_item["pio_env_name"]),
            '--project-conf="{}"'.format(pio_config_file),
        ],
        env=my_env,
        text=True,
        capture_output=True,
        cwd=workspace_dir,
    )
    results.append(
        {
            "Compiler": "PlatformIO",
            "Example": "menu_a_la_carte",
            "Board": pio_config.get(env_key, "board"),
            "build_flags": pio_config.get(env_key, "build_flags"),
            "compile_result": pio_result.returncode,
            "cause": pio_result.stderr if pio_result.returncode else "",
            "Modem": matrix_item["modem"],
            "Sensor": matrix_item["sensor"],
            "Publisher": matrix_item["publisher"],
        }
    )
    print(pio_result.stdout)
    print(pio_result.stderr)

#%%
frame_results = pd.DataFrame(results)
total_failures = len(frame_results.loc[frame_results["compile_result"]].index)
frame_results["Build Flags"] = frame_results["build_flags"].apply(
    lambda res: "".join(
        ["<li>{}</li>".format(flag.replace("-D", "").strip()) for flag in res]
    )
)
frame_results["Compiler Result"] = np.where(
    frame_results["compile_result"], ":x:", ":white_check_mark:"
)
frame_results["Failed"] = frame_results["Compiler Result"].astype(bool)
frame_results["Run"] = 1
frame_results["Failure Cause"] = frame_results["cause"].str.replace("\n", "<br/><br/>")
frame_results.groupby("Compiler")["Failed"].sum()
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
            "Example",
            "Board",
            "Modem",
            "Sensor",
            "Publisher",
            "Build Flags",
            "Compiler Result",
            "Failure Cause",
        ]
    ].to_markdown(index=False)
)
markdown_results.close()

# %%
if total_failures > 0:
    fail_message = "::error::{} Total compilation failures".format(total_failures)
    print(fail_message)
    sys.exit(fail_message)
