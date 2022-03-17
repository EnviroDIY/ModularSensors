import glob
import re
import sys

modemHeaderFiles = glob.glob("../src/modems/*.h")
sensorHeaderFiles = glob.glob("../src/sensors/*.h")
publisherHeaderFiles = glob.glob("../src/publishers/*.h")

header_files = modemHeaderFiles + sensorHeaderFiles + publisherHeaderFiles

#%% function to find the lowest level class
def find_subclasses(class_name):
    subclass_pattern = r"class[\s\n]+(\w+)[\s\n]+:[\s\n]+public[\s\n]+" + re.escape(
        class_name
    )

    subclass_list = []
    num_subclasses = 0
    for sensor_header in header_files:
        textfile = open(sensor_header, mode="r", encoding="utf-8")
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
    "../examples/menu_a_la_carte\\menu_a_la_carte.ino", mode="r", encoding="utf-8"
)
menu_example_code = menu_example_file.read()
menu_example_file.close()
menu_walk_file = open(
    "../examples/menu_a_la_carte\\ReadMe.md", mode="r", encoding="utf-8"
)
menu_example_walk = menu_walk_file.read()
menu_walk_file.close()

missing_classes = []
missing_build_flags = []
missing_snips = []
missing_walks = []
for must_doc_class in must_doc_classes:
    # print(must_doc_class["class_name"])
    matches = re.findall(must_doc_class["class_name"], menu_example_code)
    if len(matches) == 0:
        print("\t{} MISSING".format(must_doc_class["class_name"]))
        missing_classes.append(must_doc_class["class_name"])
    if "build_flag" in must_doc_class.keys():
        matches = re.findall(must_doc_class["build_flag"], menu_example_code)
        if (
            len(matches) == 0
            or (must_doc_class["super_class"] == "Sensor" and len(matches) < 2)
        ) and (must_doc_class["class_name"] not in ["MaximDS3231", "ProcessorStats"]):
            print("\t{}".format(must_doc_class["build_flag"]))
            missing_build_flags.append(must_doc_class["class_name"])
    if "menu_snip" in must_doc_class.keys():
        start_pattern = (
            r"((?:Start)|(?:End))\s+\[("
            + re.escape(must_doc_class["menu_snip"])
            + r")\]"
        )
        matches = re.findall(start_pattern, menu_example_code)
        if len(matches) < 2:
            print("\t{}".format(must_doc_class["menu_snip"]))
            missing_snips.append(must_doc_class["class_name"])

        snip_pattern = r"@menusnip\{(" + re.escape(must_doc_class["menu_snip"]) + r")\}"
        matches = re.findall(snip_pattern, menu_example_walk)
        if len(matches) == 0:
            print("\t@menusnip{{{}}}".format(must_doc_class["menu_snip"]))
            missing_walks.append(must_doc_class["class_name"])

print("The following classes are not included at all in the menu example:")
print(missing_classes)
print("The following expected build flags are missing from the menu example:")
print(missing_build_flags)
print("The following expected snipped flags are missing from the menu example:")
print(missing_snips)
print(
    "The following expected snipped flags are missing from the menu walkthrough/ReadMe:"
)
print(missing_walks)

menu_declared_snips = list(
    dict.fromkeys(re.findall(r"(?:(?:Start)|(?:End)) \[(\w+)\]", menu_example_code))
)

missing_walks = []
for snip in menu_declared_snips:
    snip_pattern = r"@menusnip\{(" + re.escape(snip) + r")\}"
    expl_snip = re.findall(snip_pattern, menu_example_walk)
    if len(expl_snip) == 0:
        print(snip_pattern)
        missing_walks.append(snip)
print(
    "The following expected snipped flags are missing from the menu walkthrough/ReadMe:"
)
print(missing_walks)

# %%
if len(missing_classes + missing_walks) > 0:
    sys.exit(
        "Some classes are not properly documented in the Menu-a-la-carte example and its walkthrough."
    )
