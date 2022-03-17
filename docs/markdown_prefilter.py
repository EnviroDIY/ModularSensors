#!/usr/bin/env python
#%%
import enum
import fileinput
import re
import sys
import string

# a helper function to go from snake back to camel
def snake_to_camel(snake_str):
    components = snake_str.strip().split("_")
    # We capitalize the first letter of each component except the first one
    # with the 'title' method and join them together.
    camel_str = components[0] + "".join(x.title() for x in components[1:])
    if camel_str.startswith("_"):
        return camel_str[1:]
    else:
        return camel_str


def camel_to_snake(name):
    name = name.strip().replace(" ", "_")
    name = re.sub("(.)([A-Z][a-z]+)", r"\1_\2", name.strip())
    return re.sub("([a-z0-9])([A-Z])", r"\1_\2", name).lower()


def github_slugify(name):
    return (
        name.strip()
        .lower()
        .replace(
            "-", " "
        )  # convert dashes to spaces so they don't get lost with other punctuation
        .translate(str.maketrans("", "", string.punctuation))
        .replace("  ", " ")
        .replace("  ", " ")
        .replace(" ", "-")
    )


# Add a start comment to make the entire markdown file function as a comment block
# Without this doxygen sometimes recognizes its own special commands, but mostly doesn't
# Not needed anymore with doxygen 1.9.3?
# try:
#     sys.stdout.buffer.write("\n/**\n".encode("utf-8"))
# except Exception as e:
#     print("\n*/\n\n")

#%%
print_me = True
skip_me = False
i = 1

# when testing use:
# with fileinput.FileInput("..\\ChangeLog.md",
#     openhook=fileinput.hook_encoded("utf-8", "surrogateescape")
# ) as input:

with fileinput.FileInput(
    openhook=fileinput.hook_encoded("utf-8", "surrogateescape")
) as input:
    for line in input:
        if input.isfirstline():
            # Get the file name and directory
            # We'll use this to create the section id comment
            file_name_dir = input.filename()
            if "\\" in file_name_dir:
                seper = "\\"
            else:
                seper = "/"
            # sys.stdout.buffer.write("Separator: '{}'\n".format(seper).encode("utf-8"))
            file_dir = file_name_dir.rsplit(sep=seper, maxsplit=1)[0]
            file_name_ext = file_name_dir.rsplit(sep=seper, maxsplit=1)[1]
            file_name = file_name_ext.rsplit(sep=".", maxsplit=1)[0]
            file_ext = file_name_ext.rsplit(sep=".", maxsplit=1)[1]
            # sys.stdout.buffer.write(
            #     "File Directory: {}, File Name: {}, File Extension: {}\n".format(
            #         file_dir, file_name, file_ext
            #     ).encode("utf-8")
            # )
            # For the example walk-throughs, written in the ReadMe files,
            # we want the example name, which is part of the directory.
            if "examples" in file_dir and file_name == "ReadMe":
                file_name = "example_" + file_dir.rsplit(sep=seper, maxsplit=1)[-1]

        # print(i, print_me, skip_me, line)

        # I'm using these comments to fence off content that is only intended for
        # github mardown rendering
        if "[//]: # ( Start GitHub Only )" in line:
            print_me = False

        # copy the original line to work with
        massaged_line = line
        # Convert markdown comment tags to c++/dox style comment tags
        massaged_line = re.sub(r"\[//\]: # \( @(\w+?.*) \)", r"@\1", massaged_line)
        # allow thank you tags
        massaged_line = massaged_line.replace("thanks to @", "thanks to \@")

        # Convert GitHub pages url's to refs
        # I'm putting the long URL in the markdown because I want the links there to
        # work and go to the pages.  But when feeding it to Doxygen, I want them to be
        # ref's so Doxygen will both check the existence of the refs and create new
        # links for them.

        # For links to sections, doxygen cuts off the first letter of the section name
        # in the examples (UGH), so some acrobatics to find them
        massaged_line = re.sub(
            r"https://envirodiy.github.io/ModularSensors/[\w/-]+\.html#enu_walk_(?P<section_name>[\w/-]+)",
            r"@ref menu_walk_\g<section_name>",
            massaged_line,
        )
        # for classes, we need to switch camel and snake cases
        class_link = re.search(
            r"https://envirodiy.github.io/ModularSensors/(?:class)(?P<class_link>[\w/-]+)\.html",
            massaged_line,
        )
        if class_link is not None:
            camel_link = snake_to_camel(class_link.group("class_link"))
            massaged_line = re.sub(
                r"https://envirodiy.github.io/ModularSensors/(?:class)(?P<class_link>[\w/-]+)\.html",
                r"@ref #" + camel_link,
                massaged_line,
            )
        # for groups, we need to clean out extra underscores
        group_link = re.search(
            r"https://envirodiy.github.io/ModularSensors/(?:group__)(?P<group_link>[\w/-]+)\.html",
            massaged_line,
        )
        if group_link is not None:
            camel_link = group_link.group("group_link").replace("__", "_")
            massaged_line = re.sub(
                r"https://envirodiy.github.io/ModularSensors/(?:group__)(?P<group_link>[\w/-]+)\.html",
                r"@ref #" + camel_link,
                massaged_line,
            )
        # for examples, we need to clean out extra underscores
        example_link = re.search(
            r"https://envirodiy.github.io/ModularSensors/(?P<example_name>[\w/-]+)_8ino-example\.html",
            massaged_line,
        )
        if example_link is not None:
            camel_link = snake_to_camel(example_link.group("example_name"))
            massaged_line = re.sub(
                r"https://envirodiy.github.io/ModularSensors/(?P<example_name>[\w/-]+)_8ino-example\.html",
                "@ref " + snake_to_camel(example_link.group("example_name")) + ".ino",
                massaged_line,
            )

        # If it's the index itself, we want to replace with a reference to the mainpage
        massaged_line = re.sub(
            r"https://envirodiy.github.io/ModularSensors/index.html#(?P<section_name>[\w/-]+)",
            r"@ref \g<section_name>",
            massaged_line,
        )
        massaged_line = re.sub(
            r"https://envirodiy.github.io/ModularSensors/index.html",
            "@ref mainpage",
            massaged_line,
        )

        # for anything other link to the docs, we the text as it is and hope it
        # lines up with a real reference
        massaged_line = re.sub(
            r"https://envirodiy.github.io/ModularSensors/(?P<section_name>[\w/-]+)\.html",
            r"@ref \g<section_name>",
            massaged_line,
        )

        # Add a PHP Markdown Extra style header id to the end of header sections
        # use the GitHub anchor plus the file name as the section id.
        # GitHub anchors for headers are the text, stripped of punctuation,
        # with the spaces replaced by hyphens.
        markdown_header = re.match(
            r"(?P<heading_pounds>#{1,6})\s+(?P<section_name>[^<>\{\}\#]+)",
            massaged_line,
        )
        php_extra_header_label = re.search(r"\{#(.+)\}", massaged_line)
        anchor_header = re.search(
            r"<a name=\"(?P<section_anchor>\w+)\"></a>", massaged_line
        )
        if (
            file_name is not None
            and file_name != "ChangeLog"
            and markdown_header is not None
            and php_extra_header_label is None
            and anchor_header is None
        ):
            massaged_line = (
                markdown_header.group("heading_pounds")
                + " "
                + markdown_header.group("section_name").strip()
                + "  {#"
                + camel_to_snake(file_name)
                + "_"
                + github_slugify(markdown_header.group("section_name"))
                + "}\n"
            )

        elif (
            file_name is not None
            and file_name != "ChangeLog"
            and markdown_header is not None
            and php_extra_header_label is not None
        ):
            # unhide PHP Markdown Extra header id's hidding in GitHub flavored markdown comments
            massaged_line = re.sub(r"<!-- \{#(.+)\} -->", r"{#\1}", massaged_line,)
            # if input.isfirstline():
            # else:
            #     massaged_line = (
            #         markdown_header.group("heading_pounds")
            #         + " "
            #         + markdown_header.group("section_name").strip()
            #         + "  {#"
            #         + camel_to_snake(file_name)
            #         + "_"
            #         + github_slugify(markdown_header.group("section_name"))
            #         + "}\n"
            #     )

        elif (
            file_name is not None
            and file_name != "ChangeLog"
            and markdown_header is not None
            and anchor_header is not None
        ):
            # convert anchors to section names
            massaged_line = re.sub(
                r"<a name=\"(?P<section_anchor>\w+)\"></a>",
                r"{#\g<section_anchor>}",
                massaged_line,
            )

        # Special work-arounds for the change log
        if file_name is not None and file_name == "ChangeLog":
            if line.startswith("# ChangeLog"):
                massaged_line = "# ChangeLog {#change_log}\n"
            version_re = re.match(
                r"#{2}\s+(?P<changelog_link>\[(?P<version_number>[^\{\}\#]+?)\])(?P<version_info>.*)",
                massaged_line,
            )
            version_action_re = re.match(
                r"#{3}\s+(?P<section_name>(?:Changed)|(?:Added)|(?:Removed)|(?:Fixed)|(?:Known Issues))",
                massaged_line,
            )
            if version_re is not None:
                change_log_version = (
                    version_re.group("version_number").strip().lower().replace(".", "-")
                )
                change_log_link = version_re.group("changelog_link")
                massaged_line = (
                    "@section "
                    + camel_to_snake(file_name)
                    + "_"
                    + change_log_version
                    + " "
                    + version_re.group("version_number")
                    + version_re.group("version_info")
                    + "\n"
                    + "GitHub Release: "
                    + change_log_link
                    # + "\n" #NOTE:  Adding the new line here would offset all doxygen line numbers
                )
            if version_action_re is not None:
                massaged_line = (
                    massaged_line.rstrip()
                    + "  {#"
                    + camel_to_snake(file_name)
                    + "_"
                    + change_log_version
                    + "_"
                    + camel_to_snake(version_action_re.group("section_name"))
                    + "}\n"
                )

        # convert internal hash-tag links to reference links
        internal_hash_link = re.search(
            r"\]\(#(?P<internal_anchor>[\w/-]+)\)", massaged_line,
        )
        if internal_hash_link is not None:
            massaged_line = re.sub(
                r"\]\(#(?P<internal_anchor>[\w/-]+)\)",
                "](@ref "
                + camel_to_snake(file_name)
                + "_"
                + github_slugify(internal_hash_link.group("internal_anchor"))
                + ")",
                massaged_line,
            )

        # finally replace code blocks with doxygen's prefered code block
        massaged_line = (
            massaged_line.replace("```ini", "@code{.ini}")
            .replace("```cpp", "@code{.cpp}")
            .replace("```", "@endcode")
        )

        # hide lines that are not printed or skipped
        # write out an empty comment line to keep the line numbers identical
        if skip_me or not print_me:
            massaged_line = "<!--" + massaged_line.strip() + "-->\n"

        if (
            massaged_line.count("\n") != line.count("\n")
            or line.count("\n") != 1
            or massaged_line.count("\n") != 1
        ):
            raise Exception(
                '\n\nNot exactly one new lines\nFile:{}\nLine Number:{}\nNew Lines in Original: {}\nOriginal Line:\n"{}"\nNew Lines after Massage: {}\nMassaged Line:\n"{}"\n\n'.format(
                    input.filename(),
                    input.filelineno(),
                    line.count("\n"),
                    line,
                    massaged_line.count("\n"),
                    massaged_line,
                )
            )

        # write out the result
        try:
            sys.stdout.buffer.write(massaged_line.encode("utf-8"))
        except Exception as e:
            print(massaged_line, end="")

        # using skip_me to skip single lines, so unset it after reading a line
        if skip_me:
            skip_me = False

        # a page, section, subsection, or subsubsection commands followed
        # immediately with by a markdown header leads to that section appearing
        # twice in the doxygen html table of contents.
        # I'm putting the section markers right above the header and then will skip the header.
        if re.match(r"\[//\]: # \( @mainpage", line) is not None:
            skip_me = True
        if re.match(r"\[//\]: # \( @page", line) is not None:
            skip_me = True
        if re.match(r"\[//\]: # \( @.*section", line) is not None:
            skip_me = True
        if re.match(r"\[//\]: # \( @paragraph", line) is not None:
            skip_me = True

        # I'm using these comments to fence off content that is only intended for
        # github mardown rendering
        if "[//]: # ( End GitHub Only )" in line:
            print_me = True

        i += 1

#%%
# Close the comment for doxygen
# Not needed anymore with doxygen 1.9.3?
# try:
#     sys.stdout.buffer.write("\n*/\n\n".encode("utf-8"))
# except Exception as e:
#     print("\n*/\n\n")
