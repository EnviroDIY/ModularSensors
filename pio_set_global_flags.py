Import('env')
from os.path import join, realpath

# pass flags to a global build environment (for all libraries, etc)
global_env = DefaultEnvironment()
global_env.Append(
    CPPDEFINES=[
        "NEOSWSERIAL_EXTERNAL_PCINT",
        "SDI12_EXTERNAL_PCINT"
    ]
)
