Import('env')
from os.path import join, realpath

# append flags to local build environment (for just this library)
env.Append(
    CPPDEFINES=[
        ("NEOSWSERIAL_EXTERNAL_PCINT",),
        ("SDI12_EXTERNAL_PCINT",)
    ]
)
# print ">>>>>LOCAL ENV<<<<<"
# print env.Dump()

# append the same flags to the global build environment (for all libraries, etc)
global_env = DefaultEnvironment()
global_env.Append(
    CPPDEFINES=[
        ("NEOSWSERIAL_EXTERNAL_PCINT",),
        ("SDI12_EXTERNAL_PCINT",)
    ]
)
# print "<<<<<GLOBAL ENV>>>>>"
# print global_env.Dump()
