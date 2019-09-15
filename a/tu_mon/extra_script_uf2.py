Import("env")
env.AlwaysBuild(env.Alias("uf2conv",
    "$BUILD_DIR/${PROGNAME}.elf",
    ["uf2conv.py  $BUILD_DIR/${PROGNAME}.elf $BUILD_DIR/${PROGNAME}.elf"]))