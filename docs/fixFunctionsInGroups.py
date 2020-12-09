#!/usr/bin/env python
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

all_group_files = [
    f
    for f in os.listdir(abs_file_path)
    if os.path.isfile(os.path.join(abs_file_path, f))
    and f.startswith("group")
    and not f.endswith("fixed")
]

for filename in all_group_files:
    abs_in = os.path.join(abs_file_path, filename)
    abs_out = os.path.join(abs_file_path, filename + "_fixed")
    print("{}".format(abs_in))
    # print("out: {}".format(abs_out))
    # with open(output_file, 'w+') as out_file:
    with open(abs_out, "w+") as out_file:
        with open(abs_in, "r") as in_file:  # open in readonly mode
            lines = in_file.readlines()
            for line in lines:
                new_line = line
                new_line = new_line.replace("&lt;mcss:", "<mcss:")
                new_line = new_line.replace(
                    "&quot;http://mcss.mosra.cz/doxygen/&quot;",
                    '"http://mcss.mosra.cz/doxygen/"',
                )
                new_line = new_line.replace("&lt;/mcss:span&gt;", "</mcss:span>")
                new_line = new_line.replace("&lt;/span&gt;", "</span>")
                new_line = new_line.replace(
                    "mcss:class=&quot;m-dim&quot;&gt;", 'mcss:class="m-dim">'
                )
                new_line = new_line.replace(
                    "class=&quot;m-dim&quot;&gt;", 'class="m-dim">'
                )
                new_line = new_line.replace(
                    "mcss:class=&quot;m-param&quot;&gt;", 'mcss:class="m-param">'
                )
                new_line = new_line.replace(
                    "class=&quot;m-param&quot;&gt;", 'class="m-param">'
                )
                new_line = new_line.replace(
                    "mcss:class=&quot;m-doc-wrap&quot;&gt;", 'mcss:class="m-doc-wrap">'
                )
                new_line = new_line.replace(
                    "class=&quot;m-doc-wrap&quot;&gt;", 'class="m-doc-wrap">'
                )
                new_line = new_line.replace("&lt;span", "<span")
                # new_line = new_line.replace("&lt;mcss:", "<mcss:")
                # new_line = new_line.replace("&lt;mcss:", "<mcss:")
                # new_line = new_line.replace("&lt;mcss:", "<mcss:")
                # new_line = new_line.replace("&lt;mcss:", "<mcss:")
                # new_line = new_line.replace("&lt;mcss:", "<mcss:")
                # new_line = new_line.replace("&lt;mcss:", "<mcss:")
                new_line = new_line.replace(
                    '<parameterlist kind="param">',
                    '<mcss:class xmlns:mcss="http://mcss.mosra.cz/doxygen/" mcss:class="m-table m-fullwidth m-flat"/>\n<table rows="3" cols="2"><row><entry thead="yes" colspan="2"><para>Parameters</para></entry></row>',
                )
                new_line = new_line.replace("</parameterlist>", "</table>")

                new_line = new_line.replace("<parameteritem>", "<row>")
                new_line = new_line.replace("</parameteritem>", "</row>")

                new_line = new_line.replace("<parameternamelist>\n", "")
                new_line = new_line.replace("</parameternamelist>\n", "")

                new_line = new_line.replace(
                    "<parametername>",
                    '<entry thead="no"><mcss:span xmlns:mcss="http://mcss.mosra.cz/doxygen/" mcss:class="m-param">',
                )
                new_line = new_line.replace("</parametername>", "</mcss:span></entry>")

                new_line = new_line.replace(
                    "<parameterdescription>", '<entry thead="no">'
                )
                new_line = new_line.replace("</parameterdescription>", "</entry>")

                out_file.write(new_line)

    os.rename(
        os.path.join(abs_file_path, filename),
        os.path.join(abs_file_path, filename + "_original"),
    )
    os.rename(
        os.path.join(abs_file_path, filename + "_fixed"),
        os.path.join(abs_file_path, filename),
    )
