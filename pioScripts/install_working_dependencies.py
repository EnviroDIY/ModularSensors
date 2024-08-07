# %%
import copy
import os
import sys
from typing import Union
from SCons.Script import (
    ARGUMENTS,
    BUILD_TARGETS,
    COMMAND_LINE_TARGETS,
    AlwaysBuild,
    Builder,
    Default,
    DefaultEnvironment,
)
import subprocess
from os import makedirs
from os.path import isdir
import re
import json

from platformio.project.config import ProjectConfig

# from platformio.package.meta import PackageSpec


# %%
Import("env")
print("Working on environment (PIOENV) {}".format(env["PIOENV"]))
print("Enviroment and project settings:")
for project_directory in [
    "PROJECT_DIR",
    "PROJECT_CORE_DIR",
    "PROJECT_PACKAGES_DIR",
    "PROJECT_WORKSPACE_DIR",
    "PROJECT_INCLUDE_DIR",
    "PROJECT_SRC_DIR",
    "PROJECT_TEST_DIR",
    "PROJECT_DATA_DIR",
    "PROJECT_BUILD_DIR",
    "PROJECT_LIBDEPS_DIR",
    "LIBSOURCE_DIRS",
    "LIBPATH",
    "PIOENV",
    "BUILD_DIR",
    "BUILD_TYPE",
    "BUILD_CACHE_DIR",
    "LINKFLAGS",
]:
    print(f"{project_directory}: {env[project_directory]}")


shared_lib_dir = env["LIBSOURCE_DIRS"][0]
shared_lib_abbr = ".pio/libdeps/shared"
project_ini_file = f"{env['PROJECT_DIR']}\\platformio.ini"
library_json_file = f"{env['PROJECT_DIR']}\\library.json"
examples_deps_file = f"{env['PROJECT_DIR']}\\examples\\example_dependencies.json"

proj_config = ProjectConfig(path=project_ini_file)
envs = proj_config.envs()


# %%
# find dependencies in the platformio.ini file
def get_shared_lib_deps(env):
    # Get lib_deps
    config = env.GetProjectConfig()
    raw_lib_deps = env.GetProjectOption("custom_shared_lib_deps", "")
    lib_deps = config.parse_multi_values(raw_lib_deps)
    return lib_deps
    # convert to dict, taking advantage of PlatformIO's PackageSpec parser
    # lib_deps = []
    # for raw_lib_dep in raw_lib_deps:
    #     spec = PackageSpec(raw_lib_dep)
    #     dep_dict = {}
    #     if spec.owner is not None:
    #         dep_dict["owner"] = spec.owner
    #     if spec.name is not None:
    #         dep_dict["name"] = spec.name
    #     if spec.uri is not None:
    #         dep_dict["version"] = spec.uri
    #     elif spec.requirements is not None:
    #         dep_dict["version"] = spec.requirements
    #     lib_deps.append(copy.deepcopy(dep_dict))
    # return lib_deps


def get_ignored_lib_deps(env):
    return proj_config.get(section=f"env:{env}", option="lib_ignore")


# find dependencies based on the library specification
if os.path.isfile(os.path.join(env["PROJECT_DIR"], "library.json")):
    with open(os.path.join(env["PROJECT_DIR"], "library.json")) as f:
        library_specs = json.load(f)
else:
    library_specs = {"dependencies": []}
# find dependencies based on the examples
if os.path.isfile(
    os.path.join(env["PROJECT_DIR"], "examples/example_dependencies.json")
):
    with open(
        os.path.join(env["PROJECT_DIR"], "examples/example_dependencies.json")
    ) as f:
        example_specs = json.load(f)
else:
    example_specs = {"dependencies": []}

dependencies = get_shared_lib_deps(env)
if "dependencies" in library_specs.keys():
    dependencies.extend(library_specs["dependencies"])
if "dependencies" in example_specs.keys():
    dependencies.extend(example_specs["dependencies"])

# quit if there are no dependencies
if len(dependencies) == 0:
    print("No dependencies to install!")
    sys.exit()

# %%
print(f"\nInstalling and updating common libraries in {shared_lib_dir}")

# Create shared_lib_dir if it does not exist
if not isdir(shared_lib_dir):
    makedirs(shared_lib_dir)


def create_pio_ci_command(
    library: Union[dict | str],
    update: bool = True,
    include_version: bool = True,
) -> list:
    pio_command_args = [
        "pio",
        "pkg",
        "update" if update else "install",
        "--skip-dependencies",
        "-g",
        "--storage-dir",
        shared_lib_dir,
        "--library",
    ]
    if isinstance(library, str):
        pio_command_args.append(library)
        return pio_command_args

    if "owner" in library.keys() and "github" in library["version"]:
        pio_command_args.append(library["version"])
    elif (
        "owner" in library.keys()
        and "name" in library.keys()
        and "version" in library.keys()
    ):
        lib_dep = f"{library['owner']}/{library['name']}"
        if include_version:
            lib_dep += f"@{library['version']}"
        pio_command_args.append(lib_dep)
    elif "name" in library.keys() and "version" in library.keys():
        lib_dep = f"{library['name']}"
        if include_version:
            lib_dep += f"@{library['version']}"
        pio_command_args.append(lib_dep)
    else:
        pio_command_args.append(library["name"])
    return pio_command_args


deps_to_install = []


# %%
def install_shared_dependencies(verbose):
    if verbose:
        print("\nInstalling libraries")

    for lib in dependencies:
        lib_install_cmd = create_pio_ci_command(
            library=lib, update=False, include_version=True
        )
        deps_to_install.extend(lib_install_cmd[-1])

        # Run command
        print(lib_install_cmd)
        if verbose:
            print(f"Installing {lib}")
        install_result = subprocess.run(
            lib_install_cmd, capture_output=True, text=True, check=True
        )
        print(install_result.stdout)
        # print(install_result.stderr)


install_shared_dependencies(True)


# %%
def update_shared_dependencies(verbose):
    if verbose:
        print("\nUpdating libraries")

    for lib in dependencies:
        lib_update_cmd = create_pio_ci_command(
            library=lib, update=True, include_version=False
        )

        # Run update command
        print(lib_update_cmd)
        if verbose:
            print(f"Updating {lib}")
        update_result = subprocess.run(
            lib_update_cmd, capture_output=True, text=True, check=True
        )
        print(update_result.stdout)
        # print(update_result.stderr)


update_shared_dependencies(True)


# %%
def parse_global_installs(verbose: bool = False):
    # Build dependency list command
    # pio pkg list -v -g  --only-libraries  --storage-dir "C:\Users\sdamiano\Documents\GitHub\EnviroDIY\ModularSensors\.pio\libdeps\shared"
    list_cmd = ["pio", "pkg", "list", "-g", "--only-libraries", "-v"]

    # set the storage directory for the libraries
    list_cmd.extend(["--storage-dir", shared_lib_dir])

    # Run update command
    # print("Listing libraries")
    # print(list_cmd)
    list_result = subprocess.run(list_cmd, capture_output=True, text=True, check=True)
    # print(list_result.stdout)
    # print(list_result.stderr)

    lib_list_presort = []

    for line in list_result.stdout.split("\n"):
        if int(verbose) >= 1:
            print(line)
        # print(line.split())
        match = re.search(
            r".*? (?P<lib_name>[\w\s-]*?) @ (?P<lib_version>[\w\s\.\+-]*?) \(required: (?:git\+)?(?P<lib_req>.*?)(?: @ .*?)?, (?P<lib_dir>.*?)\)",
            line,
        )
        if match:
            if int(verbose) >= 1:
                print("Library name: {}".format(match.group("lib_name")))
                print("Library Version: {}".format(match.group("lib_version")))
                print("Library Req: {}".format(match.group("lib_req")))
                print("Library Storage Dir: {}".format(match.group("lib_dir")))
            shared_entry = list(
                filter(
                    lambda x: match.group("lib_req").lower() in x.lower(),
                    deps_to_install,
                )
            )
            is_in_shared = len(shared_entry) > 0
            if is_in_shared:
                shared_position = deps_to_install.index(shared_entry[0])
            else:
                shared_position = -1
            if match.group("lib_name") == "Adafruit BusIO":
                shared_position = -2
            match_groups = match.groupdict()
            match_groups["shared_entry"] = shared_entry
            match_groups["is_in_shared"] = is_in_shared
            match_groups["shared_position"] = shared_position
            lib_list_presort.append(match_groups)
        else:
            if int(verbose) >= 1:
                print("XXXXXXXXXXXXXXXXXXXXXXX NO MATCH XXXXXXXXXXXXXXXXXXXXXXX")
        if int(verbose) >= 1:
            print("##########")

    lib_list = sorted(lib_list_presort, key=lambda d: d["shared_position"])
    if int(verbose) >= 1:
        print(lib_list)
    return lib_list


lib_list = parse_global_installs(False)


# %%
def create_symlink_list(environment: str, verbose: bool = False):
    lib_symlinks = []
    ign_symlinks = []
    print("Creating symlink list")
    for lib in lib_list:
        is_ignored = lib["lib_name"] in get_ignored_lib_deps(environment)
        if not is_ignored or lib["lib_name"] in [
            "Adafruit GFX Library",
            "Adafruit SSD1306",
        ]:
            lib_symlinks.append(
                f"{lib['lib_name']}=symlink://{lib['lib_dir']}".replace(
                    shared_lib_dir, shared_lib_abbr
                )
                .replace("\\\\", "/")
                .replace("\\", "/")
            )
        else:
            ign_symlinks.append(
                f"{lib['lib_name']}=symlink://{lib['lib_dir']}".replace(
                    shared_lib_dir, shared_lib_abbr
                )
                .replace("\\\\", "/")
                .replace("\\", "/")
            )
    if int(verbose) >= 1:
        print(lib_symlinks)

    return lib_symlinks


common_lib_symlinks = create_symlink_list("env", False)
for item in common_lib_symlinks:
    print("   ", item)
# for env in envs:
#     env_symlinks = create_symlink_list(env, False)

# %%
