import os
import subprocess
from pathlib import Path
import argparse
import logging

# modified from https://github.com/thebigG/clang_format_all/blob/main/src/clang_format_all/clang_format_all.py

logging.basicConfig()
logger = logging.getLogger("check-all")
logger.setLevel(logging.INFO)

default_extensions = [
    ".cpp",
    ".cc",
    ".C",
    "CPP",
    ".c++",
    "cp",
    ".cxx",
    ".h",
    ".hh",
    ".hpp",
    ".hxx",
    ".H",
    ".tpp",
    ".ino",
]

default_exclusions = [
    ".history",
    ".git",
    ".github",
    ".pio",
    ".vscode",
    "build",
    "bin",
    "lib",
    "include",
    "external",
    "third_party",
]


def parse_args():
    parser = argparse.ArgumentParser(
        description="clang_format_all starts at this directory and drills down recursively"
    )

    parser.add_argument(
        "--root_dir",
        type=str,
        required=False,
        help="Root Directory",
        default=os.getcwd(),
    )

    parser.add_argument(
        "--file_extensions",
        type=str,
        required=False,
        nargs="+",
        help="File extensions to check or format",
        default=default_extensions,
    )

    parser.add_argument(
        "--exclude_dirs",
        type=str,
        required=False,
        nargs="+",
        help="Files/Folders to Exclude",
        default=default_exclusions,
    )

    args = parser.parse_args()
    return args


def format_all_walk_recursive(root_dir: str, exclude_files=None, file_extensions=None):
    if exclude_files is None:
        exclude_files = set()
    if file_extensions is None:
        file_extensions = []
    for root, dirs, files in os.walk(root_dir):
        if dirs:
            for d in dirs:
                format_all_walk_recursive((os.path.join(root, d)), file_extensions)
        for file in files:
            path = Path(os.path.join(root, file))
            if str(path) in exclude_files:
                continue
            if path.suffix in file_extensions:
                if (
                    subprocess.run(
                        ["clang-format", "-style=file", "-i", path], capture_output=True
                    ).returncode
                    != 0
                ):
                    logger.info(
                        '"%s": An error occurred while parsing this file.', path
                    )
                    exit(-1)
                else:
                    logger.info('"%s": parsed successfully.', path)


def get_all_files(root_dir: str) -> []:
    files_array = []
    for root, dirs, files in os.walk(root_dir):
        if dirs:
            for d in dirs:
                files_array += get_all_files((os.path.join(root, d)))
        for file in files:
            files_array.append(os.path.join(root, file))
    return files_array


def get_resolved_paths(unresolved_paths: [str]) -> [str]:
    resolved_paths = []
    for p in unresolved_paths:
        resolved_paths += get_all_files(str(Path(p).resolve()))

    return set(resolved_paths)


def main():
    args = parse_args()
    excluded_dirs = get_resolved_paths(args.exclude_dirs)
    format_all_walk_recursive(
        str(Path(args.root_dir).resolve()),
        excluded_dirs,
        args.file_extensions,
    )


if __name__ == "__main__":
    main()
