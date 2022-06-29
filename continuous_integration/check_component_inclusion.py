#%%
import glob
import re
import sys
import os
import json
from pathlib import Path

script_dir = os.path.dirname(__file__)  # <-- absolute dir the script is in

modemHeaderFiles = glob.glob(os.path.join(script_dir, "../src/modems/*.h"))
sensorHeaderFiles = glob.glob(os.path.join(script_dir, "../src/sensors/*.h"))
publisherHeaderFiles = glob.glob(os.path.join(script_dir, "../src/publishers/*.h"))

header_files = modemHeaderFiles + sensorHeaderFiles + publisherHeaderFiles
# print(header_files)

#%% function to find the lowest level class
def find_subclasses(class_name):
    subclass_pattern = r"class[\s\n]+(\w+)[\s\n]+:[\s\n]+public[\s\n]+" + re.escape(
        class_name
    )

    subclass_list = []
    num_subclasses = 0
    for header_file in header_files:
        textfile = open(header_file, mode="r", encoding="utf-8")
        filetext = textfile.read()
        textfile.close()
        matches = re.findall(subclass_pattern, filetext)
        if matches != []:
            for subclass in matches:
                num_subclasses += 1
                # print(sensor_header, class_name, subclass, num_subclasses)
                sub_subs = find_subclasses(subclass)
                for sub in sub_subs:
                    subclass_list.append(sub)
    if num_subclasses == 0 and class_name not in subclass_list:
        subclass_list.append(class_name)
    return subclass_list


def camel_to_snake(name, lower_case=True):
    name1 = re.sub("(.)([A-Z][a-z]+)", r"\1_\2", name)
    name_lower = (
        re.sub("([a-z0-9])([A-Z])", r"\1_\2", name1)
        .lower()
        .replace("5_tm", "_5tm")
        .replace("tiina", "ti_ina")
        .replace("p_h", "_ph")
        .replace("mpl115_a2", "mpl115a2")
        .replace("i2_c", "i2c")
        .replace("2_g_", "_2g_")
        .replace("x_bee", "_xbee")
        .replace("u_bee", "_ubee")
        .replace("3_g_", "_3g_")
        .replace("r410_m", "r410m")
        .replace("__", "_")
    )
    if lower_case:
        return name_lower
    else:
        return name_lower.upper()


#%%
# make sure class names match file names
class_pattern = re.compile("^\s*class[\s\n]+(\w+)[\s\n]", re.MULTILINE)

for header_file in header_files:
    textfile = open(header_file, mode="r", encoding="utf-8")
    filetext = textfile.read()
    textfile.close()
    file_name = Path(os.path.basename(header_file)).stem
    class_matches = re.findall(class_pattern, filetext)
    for class_match in class_matches:
        if not class_match.startswith(file_name):
            # print(
            #     "Class {} is defined in file {}.  Class names should match their file names".format(
            #         class_match, file_name
            #     )
            # )
            sys.exit(
                "Class {} is defined in file {}.  Class names should match their file names".format(
                    class_match, file_name
                )
            )

#%%
# make sure there are examples of all classes in the menu example
must_doc_classes = []

modem_sub_classes = find_subclasses("loggerModem")
modem_sub_classes.append("SIMComSIM800")
modem_sub_classes.sort()
for modem_sub_class in modem_sub_classes:
    must_doc_classes.append(
        {
            "super_class": "loggerModem",
            "class_name": modem_sub_class,
            "build_flag": "BUILD_MODEM_{}".format(
                camel_to_snake(modem_sub_class, False)
            ),
            "menu_snip": camel_to_snake(modem_sub_class),
        }
    )

sensor_sub_classes = find_subclasses("Sensor")
sensor_sub_classes.sort()
for sensor_sub_class in sensor_sub_classes:
    must_doc_classes.append(
        {
            "super_class": "Sensor",
            "class_name": sensor_sub_class,
            "build_flag": "BUILD_SENSOR_{}".format(
                camel_to_snake(sensor_sub_class, False)
            ),
            "menu_snip": camel_to_snake(sensor_sub_class),
        }
    )
var_sub_classes = find_subclasses("Variable")
var_sub_classes.sort()
for var_sub_class in var_sub_classes:
    must_doc_classes.append(
        {"super_class": "Variable", "class_name": var_sub_class,}
    )

publisher_sub_classes = find_subclasses("dataPublisher")
publisher_sub_classes.sort()
for publisher_sub_class in publisher_sub_classes:
    must_doc_classes.append(
        {
            "super_class": "dataPublisher",
            "class_name": publisher_sub_class,
            "build_flag": "BUILD_PUB_{}".format(
                camel_to_snake(publisher_sub_class, False)
            ),
            "menu_snip": camel_to_snake(publisher_sub_class),
        }
    )

# print(must_doc_classes)
#%%
menu_example_file = open(
    os.path.join(script_dir, "../examples/menu_a_la_carte/menu_a_la_carte.ino"),
    mode="r",
    encoding="utf-8",
)
menu_example_code = menu_example_file.read()
menu_example_file.close()
menu_walk_file = open(
    os.path.join(script_dir, "../examples/menu_a_la_carte/ReadMe.md"),
    mode="r",
    encoding="utf-8",
)
menu_example_walk = menu_walk_file.read()
menu_walk_file.close()

missing_classes = []
missing_build_flags = []
missing_snips = []
missing_walks = []
for must_doc_class in must_doc_classes:
    matches = re.findall(must_doc_class["class_name"], menu_example_code)
    if len(matches) == 0:
        missing_classes.append("{} MISSING".format(must_doc_class["class_name"]))
    if "build_flag" in must_doc_class.keys():
        matches = re.findall(must_doc_class["build_flag"], menu_example_code)
        if (
            len(matches) == 0
            or (must_doc_class["super_class"] == "Sensor" and len(matches) < 2)
        ) and (must_doc_class["class_name"] not in ["MaximDS3231", "ProcessorStats"]):
            missing_build_flags.append("{}".format(must_doc_class["build_flag"]))
    if "menu_snip" in must_doc_class.keys():
        start_pattern = (
            r"((?:Start)|(?:End))\s+\[("
            + re.escape(must_doc_class["menu_snip"])
            + r")\]"
        )
        matches = re.findall(start_pattern, menu_example_code)
        if len(matches) < 2:
            missing_snips.append("{}".format(must_doc_class["menu_snip"]))

        snip_pattern = r"@menusnip\{(" + re.escape(must_doc_class["menu_snip"]) + r")\}"
        expl_snip = re.findall(snip_pattern, menu_example_walk)
        if len(expl_snip) == 0:
            missing_walks.append("@menusnip{{{}}}".format(must_doc_class["menu_snip"]))


def print_missing(missing_things):
    for missing_thing in missing_things:
        print("  - `{}`".format(missing_thing))


if len(missing_classes) > 0:
    print("- The following classes are not included at all in the menu example code:")
    print_missing(missing_classes)
if len(missing_build_flags) > 0:
    print(
        "- The following expected build flags are missing from the menu example code:"
    )
    print_missing(missing_build_flags)
if len(missing_snips) > 0:
    print(
        "- The following expected snippet markers are missing from the menu example code:"
    )
    print_missing(missing_snips)
if len(missing_walks) > 0:
    print(
        "- The following expected snippet markers are missing from the menu walkthrough/ReadMe:"
    )
    print_missing(missing_walks)

if len(missing_classes + missing_build_flags + missing_snips + missing_walks) > 0:
    out_message = {
        "passed": False,
        "missing documentation": {
            "missing classes": missing_classes,
            "missing build flags": missing_build_flags,
            "missing snippets": missing_snips,
            "missing walkthrough": missing_walks,
        },
    }
    sys.exit(1)
