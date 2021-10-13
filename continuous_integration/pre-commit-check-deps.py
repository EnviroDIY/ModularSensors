import json
import subprocess

# Open the current library.json file
library_json = open(
    "C:\\Users\\sdamiano\\Documents\\GitHub\\EnviroDIY\\ModularSensors\\library.json",
)
library_data = json.load(library_json)

# Open the current library.json file
dependency_json = open(
    "C:\\Users\\sdamiano\\Documents\\GitHub\\EnviroDIY\\ModularSensors\\continuous_integration\\dependencies.json",
)
dependency_data = json.load(dependency_json)

lib_dependencies = library_data["dependencies"]
out_dependencies = dependency_data["dependencies"]

if lib_dependencies != out_dependencies:
    dependency_data["action_cache_version"] += 1
    dependency_data["dependencies"] = library_data["dependencies"]

    with open(
        "C:\\Users\\sdamiano\\Documents\\GitHub\\EnviroDIY\\ModularSensors\\continuous_integration\\dependencies.json",
        "w",
    ) as outfile:
        json.dump(dependency_data, outfile, indent=2)

    subprocess.run("git add continuous_integration\\dependencies.json")
