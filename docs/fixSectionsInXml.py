#!/usr/bin/env python
#%%
import fileinput
import re
import os
import glob
import xml.etree.ElementTree as ET

fileDir = os.path.dirname(os.path.realpath("__file__"))
# print("Program Directory: {}".format(fileDir))
relative_dir = "../../ModularSensorsDoxygen/xml/"
abs_file_path = os.path.join(fileDir, relative_dir)
abs_file_path = os.path.abspath(os.path.realpath(abs_file_path))
# print("XML Directory: {}".format(fileDir))

all_files = [
    f
    for f in os.listdir(abs_file_path)
    if os.path.isfile(
        os.path.join(abs_file_path, f)
    )  # and f.endswith("8ino-example.xml")
]

compound_def = r"<compounddef id=\"(?P<doxygen_compound_id>.+?)\" kind=\"\w+?\">"
section_header = r"<sect(?P<section_number>[123456]) id=\"(?P<doxygen_sect_id>.+)\">"
doxy_file_location = r"<location file=\"(?P<file_location>.+)\"/>"
#%%
for filename in all_files:
    # print("Now on {}".format(os.path.join(abs_file_path, filename)))

    needs_to_be_fixed = False

    doxygen_compound_id = None
    section_number = None
    doxygen_sect_id = None
    sections_to_fix = []
    # First search the xml for the location of the original file.
    # This will be at the end of the xml, so we need to find this, close the file,
    # and then reopen to search for the compound def and section.
    original_xml = open(
        os.path.join(abs_file_path, filename), mode="r", encoding="utf-8"
    )
    # filetext = original_xml.read()
    for line in original_xml.readlines():
        if re.search(doxy_file_location, line) is not None:
            file_location = re.search(doxy_file_location, line).group("file_location")
    original_xml.close()

    # Now search for the sections
    original_xml = open(
        os.path.join(abs_file_path, filename), mode="r", encoding="utf-8"
    )
    for line in original_xml.readlines():
        # print(line, end="")
        if re.search(compound_def, line) is not None:
            doxygen_compound_id = re.search(compound_def, line).group(
                "doxygen_compound_id"
            )
        if re.search(section_header, line) is not None:
            section_number = re.search(section_header, line).group("section_number")
            doxygen_sect_id = re.search(section_header, line).group("doxygen_sect_id")
            # print(
            #     "section_number:", section_number, "doxygen_sect_id:", doxygen_sect_id
            # )
            if (
                not doxygen_sect_id.startswith(doxygen_compound_id)
                and file_location in doxygen_sect_id
            ):
                needs_to_be_fixed = True
                file_name_loc = doxygen_sect_id.find(file_location)
                section_suffix = doxygen_sect_id[
                    file_name_loc + len(file_location) + 2 :
                ]
                corrected_id = doxygen_compound_id + "_" + section_suffix
                sections_to_fix.append(
                    {
                        "doxygen_compound_id": doxygen_compound_id,
                        "section_number": section_number,
                        "doxygen_sect_id": doxygen_sect_id,
                        "file_location": file_location,
                        "needs_to_be_fixed": True,
                        "corrected_id": corrected_id,
                    }
                )
                print(
                    "Will correct:\n\t{}\nTo:\n\t{}".format(
                        doxygen_sect_id, corrected_id
                    )
                )

    original_xml.close()

    # Now we're going to open the file and copy to a new one with corrections applied
    if needs_to_be_fixed:
        original_xml = open(
            os.path.join(abs_file_path, filename), mode="r", encoding="utf-8"
        )
        corrected_xml = open(
            os.path.join(abs_file_path, filename + "_fixed"), mode="w", encoding="utf-8"
        )
        for line in original_xml.readlines():
            corrected_line = line
            for section_to_fix in sections_to_fix:
                corrected_line = re.sub(
                    r"<sect(?P<section_number>[123456]) id=\""
                    + section_to_fix["doxygen_sect_id"]
                    + r"\">",
                    r'<sect\g<section_number> id="'
                    + section_to_fix["corrected_id"]
                    + r'">',
                    corrected_line,
                )
                # corrected_line = corrected_line.replace(
                #     ' id="{}"'.format(section_to_fix["doxygen_sect_id"]),
                #     ' id="{}"'.format(section_to_fix["corrected_id"]),
                # )
            corrected_xml.write(corrected_line)
        original_xml.close()
        corrected_xml.close()

    if needs_to_be_fixed:
        os.rename(
            os.path.join(abs_file_path, filename),
            os.path.join(abs_file_path, filename + "_original"),
        )
        os.rename(
            os.path.join(abs_file_path, filename + "_fixed"),
            os.path.join(abs_file_path, filename),
        )
        print("Saved changed file")
        print("-----\n\n\n")
    # else:
    #     print("No changes needed")
    # print("-----\n")
    # break

# %%
