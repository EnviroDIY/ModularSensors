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

Import("env")

if "idedata" in COMMAND_LINE_TARGETS:
    env.Exit(0)

if env.IsCleanTarget():
    print("==== WE WANT TO DO A CLEAN ====")

if env.IsIntegrationDump():
    print("==== INTEGRATION DUMP ====")

# print("Working on environment (PIOENV) {}".format(env["PIOENV"]))
# print(
#     "Project workspace directory (PROJECT_WORKSPACE_DIR): {}".format(
#         env["PROJECT_WORKSPACE_DIR"]
#     )
# )
# print("Project source directory (PROJECT_SRC_DIR): {}".format(env["PROJECT_SRC_DIR"]))
# print(
#     "Project build directory (PROJECT_BUILD_DIR): {}".format(env["PROJECT_BUILD_DIR"])
# )
# print("Enviroment build directory: {}".format(env["BUILD_DIR"]))
# print("Project lib deps directory: {}".format(env["PROJECT_LIBDEPS_DIR"]))
# print("Enviroment lib path: {}".format(env["LIBPATH"]))
# print("Enviroment lib source directories: {}".format(env["LIBSOURCE_DIRS"]))

print("\nInstalling and updating common libraries")


def get_shared_lib_dir(env):
    # Get shared_lib_dir
    return env.GetProjectOption("custom_shared_lib_dir")


def get_shared_lib_deps(env):
    # Get lib_deps
    config = env.GetProjectConfig()
    raw_lib_deps = env.GetProjectOption("custom_shared_lib_deps")
    lib_deps = config.parse_multi_values(raw_lib_deps)
    return lib_deps


def get_ignored_lib_deps(env):
    # Get lib_deps
    config = env.GetProjectConfig()
    raw_lib_ignore = env.GetProjectOption("lib_ignore")
    lib_ignore = config.parse_multi_values(raw_lib_ignore)
    return lib_ignore


pio_pkg_command = [env.subst("$PYTHONEXE"), "-m", "platformio", "pkg"]


def install_shared_dependencies(env, verbose):
    # Add verbose text
    if int(verbose) >= 1:
        print("Installing to {}".format(get_shared_lib_dir(env)))

    # Create shared_lib_dirif it does not exist
    if not isdir(get_shared_lib_dir(env)):
        if int(verbose) >= 1:
            print(
                "Directory {} doesn't exist, creating it".format(
                    get_shared_lib_dir(env)
                )
            )
        makedirs(get_shared_lib_dir(env))

    # Build dependency installation command
    install_cmd = pio_pkg_command + ["install"]
    install_cmd.extend(["--storage-dir", get_shared_lib_dir(env)])

    # # Add verbose to command
    # if int(verbose) < 1:
    #     install_cmd.append("-s")

    # Add dependencies to command
    for lib in get_shared_lib_deps(env):
        install_cmd.append("-l")
        install_cmd.append(lib)

    # Run command
    # print(install_cmd)
    install_result = subprocess.run(
        install_cmd, capture_output=True, text=True, check=True
    )
    print(install_result.stdout)
    print(install_result.stderr)


def update_shared_dependencies(env, verbose):
    # Build dependency update command
    update_cmd = pio_pkg_command + ["update", "-g"]

    # Add verbose to command
    if int(verbose) < 1:
        update_cmd.append("-s")

    # set the storage directory for the libraries
    update_cmd.extend(["--storage-dir", get_shared_lib_dir(env)])

    # Add dependencies to command
    for lib in get_shared_lib_deps(env):
        update_cmd.append("-l")
        update_cmd.append(lib)

    # Run update command
    print("Updating libraries")
    # print(update_cmd)
    update_result = subprocess.run(
        update_cmd, capture_output=True, text=True, check=True
    )
    print(update_result.stdout)
    print(update_result.stderr)


def parse_global_installs(env, verbose):
    # Build dependency list command
    # pio pkg list -v -g  --only-libraries  --storage-dir "C:\Users\sdamiano\Documents\GitHub\EnviroDIY\ModularSensors\.pio\libdeps\shared"
    list_cmd = pio_pkg_command + [
        "list",
        "-g",
        "--only-libraries",
    ]

    # Add verbose to command
    if int(verbose) >= 1:
        list_cmd.append("-v")

    # set the storage directory for the libraries
    list_cmd.extend(["--storage-dir", get_shared_lib_dir(env)])

    # Run update command
    print("Listing libraries")
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
                    get_shared_lib_deps(env),
                )
            )
            is_in_shared = len(shared_entry) > 0
            if is_in_shared:
                shared_position = get_shared_lib_deps(env).index(shared_entry[0])
            else:
                shared_position = -1
            is_ignored = match.group("lib_name") in get_ignored_lib_deps(env)
            match_groups = match.groupdict()
            match_groups["shared_entry"] = shared_entry
            match_groups["is_in_shared"] = is_in_shared
            match_groups["is_ignored"] = is_ignored
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

    lib_used = []
    for lib in lib_list:
        if not lib["is_ignored"]:
            lib_used.append(f"{lib['lib_name']}=symlink://{lib['lib_dir']}")
    if int(verbose) >= 1:
        print(lib_used)

    platform = env.PioPlatform()
    prev_lib_deps = env.GetProjectOption("lib_deps")
    new_lib_deps = prev_lib_deps + lib_used
    env_section = "env:" + env["PIOENV"]
    platform.config.set(env_section, "lib_deps", new_lib_deps)


# Get verbose level
VERBOSE = ARGUMENTS.get("PIOVERBOSE", 0)

# Intall dependencies listed in env shared_lib_deps to shared_lib_dir
install_shared_dependencies(env, VERBOSE)
update_shared_dependencies(env, VERBOSE)
parse_global_installs(env, VERBOSE)
