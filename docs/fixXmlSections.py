#!/usr/bin/env python
import fileinput
import re
import os
import glob
import xml.etree.ElementTree as ET

fileDir = os.path.dirname(os.path.realpath("__file__"))
# print("Program Directory: {}".format(fileDir))
relative_dir = "../../Arduino-SDI-12Doxygen/xml/"
abs_file_path = os.path.join(fileDir, relative_dir)
abs_file_path = os.path.abspath(os.path.realpath(abs_file_path))
# print("XML Directory: {}".format(fileDir))

output_file = "examples.dox"
read_mes = [
    # "../../ModularSensorsDoxygen/xml_8ino-example.xml",
    "../../ModularSensorsDoxygen/xml/baro_rho_correction_8ino-example.xml",
    "../../ModularSensorsDoxygen/xml/data_saving_8ino-example.xml",
    "../../ModularSensorsDoxygen/xml/double_logger_8ino-example.xml",
    "../../ModularSensorsDoxygen/xml/DRWI_CitSci_8ino-example.xml",
    "../../ModularSensorsDoxygen/xml/DRWI_LTE_8ino-example.xml",
    "../../ModularSensorsDoxygen/xml/DRWI_NoCellular_8ino-example.xml",
    "../../ModularSensorsDoxygen/xml/logging_to_MMW_8ino-example.xml",
    "../../ModularSensorsDoxygen/xml/logging_to_ThingSpeak_8ino-example.xml",
    "../../ModularSensorsDoxygen/xml/menu_a_la_carte_8ino-example.xml",
    "../../ModularSensorsDoxygen/xml/simple_logging_8ino-example.xml",
    "../../ModularSensorsDoxygen/xml/simple_logging_LearnEnviroDIY_8ino-example.xml",
    "../../ModularSensorsDoxygen/xml/single_sensor_8ino-example.xml",
]
all_files = [f for f in os.listdir(abs_file_path) if os.path.isfile(
    os.path.join(abs_file_path, f))]

for filename in all_files:
    # print(filename)

    tree = ET.parse(os.path.join(abs_file_path, filename))
    root = tree.getroot()

    needs_to_be_fixed = False
    for definition in root.iter("compounddef"):
        # print(definition.attrib)
        compound_id = definition.attrib["id"]
        # print(compound_id)
        # print("---")

        for i in range(6):
            for section in definition.iter("sect" + str(i)):
                # print(section.attrib)
                section_id = section.attrib["id"]
                if not section_id.startswith(compound_id):
                    # print("problem!")
                    needs_to_be_fixed = True
                    dox_loc = section_id.find(".dox_")
                    section_suffix = section_id[dox_loc + 6:]
                    # print(section_suffix)
                    corrected_id = compound_id + "_" + section_suffix
                    # print(corrected_id)
                    section.attrib["id"] = corrected_id

    if needs_to_be_fixed:
        tree.write(os.path.join(abs_file_path, filename + "_fixed"))
        os.rename(os.path.join(abs_file_path, filename),
                  os.path.join(abs_file_path, filename + "_original"))
        os.rename(os.path.join(abs_file_path, filename + "_fixed"),
                  os.path.join(abs_file_path, filename))
    # print()
