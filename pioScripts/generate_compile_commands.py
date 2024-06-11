import os
import sys

Import("env")

if "compiledb" not in COMMAND_LINE_TARGETS:
    print("Generating compile commands!")

    # include toolchain paths
    env.Replace(COMPILATIONDB_INCLUDE_TOOLCHAIN=True)

    # override compilation DB path
    env.Replace(
        COMPILATIONDB_PATH=os.path.join("$PROJECT_DIR/.vscode", "compile_commands.json")
    )
