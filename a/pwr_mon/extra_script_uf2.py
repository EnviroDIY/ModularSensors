Import("env","projenv")
# access to global construction environment
#print(env)

#print(projenv)
# Dump construction environment (for debug purpose)
#print(env.Dump())
#print(projenv.Dump())

# UF2 from .bin
env.AddPostAction(
    "$BUILD_DIR/${PROGNAME}.bin",
    env.VerboseAction(" ".join([
        "uf2conv.py", "-C", "-O", "-R", 
        "$BUILD_DIR/${PROGNAME}.uf2", "$BUILD_DIR/${PROGNAME}.bin"
    ]), "Building $BUILD_DIR/${PROGNAME}.bin")
)
