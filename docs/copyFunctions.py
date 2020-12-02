#!/usr/bin/env python
import fileinput
import re
import os
import glob
import xml.etree.ElementTree as ET
from html.parser import HTMLParser
from bs4 import BeautifulSoup

fileDir = os.path.dirname(os.path.realpath("__file__"))
# print("Program Directory: {}".format(fileDir))
relative_dir = "../../ModularSensorsDoxygen/m.css/"
abs_file_path = os.path.join(fileDir, relative_dir)
abs_file_path = os.path.abspath(os.path.realpath(abs_file_path))
# print("XML Directory: {}".format(fileDir))

all_files = [
    f
    for f in os.listdir(abs_file_path)
    if os.path.isfile(os.path.join(abs_file_path, f))
    and f.endswith(".html")
    and not f.endswith("fixed")
]


def get_section_to_paste(match: re.Match) -> str:
    source_file = match.group("copy_source_file")
    # print(source_file)
    source_section = match.group("copy_section_id")
    # print(source_section)
    with open(os.path.join(abs_file_path, source_file)) as fp:
        soup = BeautifulSoup(fp, "html.parser")
        details = soup.find(id=source_section)
        # print("Details:", details, "\n\n")
        link = details.find("a", class_="m-doc-self")
        # print("Link:", link, "\n\n")
        link["href"] = source_file + "#" + source_section
        # print("Link:", link, "\n\n")
        # print("Details:", details, "\n\n")
        return str(details)

    # tree = ET.parse()
    # root = tree.getroot()

    # for definition in root.iter("compounddef"):
    #     # print(definition.attrib)
    #     compound_id = definition.attrib["id"]
    #     # print(compound_id)
    #     # print("---")


# {{ <a href="class_a_o_song_a_m2315___humidity.html#ab201cd06c49eec79df6263b8da8f10e3" class="m-doc">AOSongAM2315_Humidity::<wbr />AOSongAM2315_Humidity</a> }}
files_to_copy_to = []
for filename in all_files:
    abs_in = os.path.join(abs_file_path, filename)
    abs_out = os.path.join(abs_file_path, filename + "_fixed")
    copy_paste_needed = False
    # with open(os.path.join(abs_file_path, filename)) as fp:
    #     soup = BeautifulSoup(fp, "html.parser")
    #     for find in soup.find_all(string=[re.compile("\{\{")]):
    #         print(find.find_parent("p").a.get("href"))

    with open(abs_in, "r") as in_file:  # open in readonly mode
        lines = in_file.readlines()
        i = 0
        new_lines = []
        for line in lines:
            i += 1
            new_line = line
            match = re.search(
                '\{\{ <a href="(?P<copy_source_file>.*?)#(?P<copy_section_id>.*?)" .*? \}\}',
                line,
            )
            if match is not None:
                copy_paste_needed = True
                new_line = re.sub(
                    '\{\{ <a href="(?P<copy_source_file>.*?)#(?P<copy_section_id>.*?)" .*? \}\}',
                    get_section_to_paste,
                    line,
                )
                print(
                    filename,
                    i,
                    match.group("copy_source_file"),
                    match.group("copy_section_id"),
                )
            new_lines.append(new_line)

    if copy_paste_needed:
        with open(abs_out, "w+") as out_file:
            for line in new_lines:
                out_file.write(line)

        os.rename(
            os.path.join(abs_file_path, filename),
            os.path.join(abs_file_path, filename + "_original"),
        )
        os.rename(
            os.path.join(abs_file_path, filename + "_fixed"),
            os.path.join(abs_file_path, filename),
        )
