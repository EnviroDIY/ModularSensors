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
test_code_path = examples_path + "\\test_code"
file_to_compile = test_code_path + "\\test_code.ino"


# print("XML Directory: {}".format(fileDir))

all_examples = [
    f
    for f in os.listdir(examples_path)
    if os.path.isdir(os.path.join(examples_path, f))
    and f not in ["menu_a_la_carte", ".history", "logger_test", "YosemitechDO"]
]
pio_config_file = workspace_dir + "\\continuous_integration\\platformio.ini"
menu_file = examples_path + "\\menu_a_la_carte\\menu_a_la_carte.ino"
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


def prepare_example(example, build_flags):
    textfile = open(example, "r")
    as_written = textfile.read()
    textfile.close()

    if not os.path.exists(test_code_path):
        os.makedirs(test_code_path)

    prepared_example = open(file_to_compile, "w+")
    for flag in build_flags:
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
for example in all_examples:
    for pio_env in [
        use_env
        for use_env in pio_config.envs()
        if use_env not in ["software_wire", "complex_loop", "non_concurrent"]
    ]:
        env_key = "env:{}".format(pio_env)
        prepare_example(
            example=os.path.join(examples_path, example) + "\\{}.ino".format(example),
            build_flags=[
                flag.replace("-D", "").strip()
                for flag in pio_config.get(env_key, "build_flags")
            ],
        )
        print(
            "\n\nNow building {example} for {board} using the Arduino CLI".format(
                example=example, board=pio_env
            )
        )
        cli_result = subprocess.run(
            'arduino-cli --config-file "{workspace_dir}/continuous_integration/arduino_cli.yaml" compile --clean --format text --log-file "{workspace_dir}/arduino_cli_log.log" --log-level warn --log-format json --build-property "build.extra_flags={extra_flags}" --fqbn {fqbn} "{file_to_compile}"'.format(
                workspace_dir=workspace_dir,
                file_to_compile=file_to_compile,
                fqbn=pio_to_acli[pio_config.get(env_key, "board")]["fqbn"],
                extra_flags=" ".join(
                    pio_to_acli[pio_config.get(env_key, "board")]["extra_flags"]
                ),
            ),
            text=True,
            capture_output=True,
            cwd=workspace_dir,
        )
        results.append(
            {
                "Compiler": "Arduino CLI",
                "Example": example,
                "Board": pio_config.get(env_key, "board"),
                "build_flags": pio_config.get(env_key, "build_flags"),
                "compile_result": cli_result.returncode,
                "cause": cli_result.stderr if cli_result.returncode else "",
                "Modem": "",
                "Sensor": "",
                "Publisher": "",
            }
        )
        print(cli_result.stdout)
        print(cli_result.stderr)

        print(
            "\n\nNow building {example} for {board} using PlatformIO".format(
                example=example, board=pio_env
            )
        )
        my_env = os.environ.copy()
        my_env["PLATFORMIO_SRC_DIR"] = test_code_path
        my_env["PLATFORMIO_DEFAULT_ENVS"] = pio_env
        pio_result = subprocess.run(
            'pio run --environment {pio_env} --project-conf="{pio_config_file}"'.format(
                pio_env=pio_env, pio_config_file=pio_config_file
            ),
            env=my_env,
            text=True,
            capture_output=True,
            cwd=workspace_dir,
        )
        results.append(
            {
                "Compiler": "PlatformIO",
                "Example": example,
                "Board": pio_config.get(env_key, "board"),
                "build_flags": pio_config.get(env_key, "build_flags"),
                "compile_result": pio_result.returncode,
                "cause": pio_result.stderr if pio_result.returncode else "",
                "Modem": "",
                "Sensor": "",
                "Publisher": "",
            }
        )
        print(pio_result.stdout)
        print(pio_result.stderr)

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
flag_matrix = []
for modem_flag in all_modem_flags:
    flag_matrix.append([modem_flag, all_sensor_flags[0], all_publisher_flags[0]])
for sensor_flags in all_sensor_flags[1:]:
    flag_matrix.append([all_modem_flags[0], sensor_flags, all_publisher_flags[0]])
for publisher_flag in all_publisher_flags[1:]:
    flag_matrix.append([all_modem_flags[0], all_sensor_flags[0], publisher_flag])

#%%
# print(yaml.dump(matrix_includes, sort_keys=False))
for flag_set in flag_matrix:
    for pio_env in pio_config.envs():
        env_key = "env:{}".format(pio_env)
        modem = flag_set[0].replace("BUILD_MODEM_", "")
        sensor = flag_set[1].replace("BUILD_SENSOR_", "")
        publisher = flag_set[2].replace("BUILD_PUB_", "").replace("_PUBLISHER", "")
        prepare_example(
            example=menu_file,
            build_flags=pio_config.get(env_key, "build_flags"),
        )
        print(
            "\n\nNow building menu_a_la_carte for {board} with {modem}, {sensor}, and {publisher} using the Arduino CLI".format(
                board=env_key,
                modem=modem,
                sensor=sensor,
                publisher=publisher,
            )
        )
        cli_result = subprocess.run(
            'arduino-cli --config-file "{workspace_dir}/continuous_integration/arduino_cli.yaml" compile --clean --format text --log-file "{workspace_dir}/arduino_cli_log.log" --log-level warn --log-format json --build-property "build.extra_flags={extra_flags}" --fqbn {fqbn} "{file_to_compile}"'.format(
                workspace_dir=workspace_dir,
                file_to_compile=file_to_compile,
                fqbn=pio_to_acli[pio_config.get(env_key, "board")]["fqbn"],
                extra_flags=" ".join(
                    pio_to_acli[pio_config.get(env_key, "board")]["extra_flags"]
                ),
            ),
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
                "Modem": modem,
                "Sensor": sensor,
                "Publisher": publisher,
            }
        )
        print(cli_result.stdout)
        print(cli_result.stderr)

        print(
            "\n\nNow building {example} for {board} using PlatformIO".format(
                example=example, board=pio_env
            )
        )
        my_env = os.environ.copy()
        my_env["PLATFORMIO_SRC_DIR"] = test_code_path
        my_env["PLATFORMIO_DEFAULT_ENVS"] = pio_env
        pio_result = subprocess.run(
            'pio run --environment {pio_env} --project-conf="{pio_config_file}"'.format(
                pio_env=pio_env, pio_config_file=pio_config_file
            ),
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
                "Modem": modem,
                "Sensor": sensor,
                "Publisher": publisher,
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
markdown_results = open(workspace_dir + "\\compile_results.md", "w+")
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
    fail_message="::error::{} Total compilation failures".format(total_failures)
    print(fail_message)
    sys.exit(fail_message)
