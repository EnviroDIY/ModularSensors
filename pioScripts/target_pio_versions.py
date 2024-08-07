Import("env")

# https://docs.platformio.org/en/latest/scripting/custom_targets.html

# Single action/command per 1 target
env.AddCustomTarget("sysenv", None, 'python -c "import os; print(os.environ)"')

# Multiple actions
env.AddCustomTarget(
    name="pioenv",
    dependencies=None,
    actions=["pio --version", "python --version"],
    title="Core Env",
    description="Show PlatformIO Core and Python versions",
)
