#!/bin/sh

# exit when any command fails
set -e

# keep track of the last executed command
# trap 'last_command=$current_command; current_command=$BASH_COMMAND' DEBUG
# echo an error message before exiting
# trap 'echo "\"${last_command}\" command filed with exit code $?."' EXIT

mkdir temp
mkdir temp/menu_a_la_carte

echo "\e[36m--------------------------------------------------------------------------------------\e[0m"
echo "\e[36m--------------------------------------------------------------------------------------\e[0m"
echo "Modifying source for \e[32m$modemFlag\e[0m, \e[32m$sensorFlag\e[0m, and \e[32m$publisherFlag\e[0m"
echo "\e[36m--------------------------------------------------------------------------------------\e[0m"
echo "\e[36m--------------------------------------------------------------------------------------\e[0m"

sed -i "1s/^/#define MS_BUILD_TEST_PRE_NAMED_VARS\n/" examples/menu_a_la_carte/menu_a_la_carte.ino > temp/menu_a_la_carte/menu_a_la_carte.ino

if [ "$modemFlag" != "" ]
then
    sed -i "1s/^/#define $modemFlag\n/" examples/menu_a_la_carte/menu_a_la_carte.ino > temp/menu_a_la_carte/menu_a_la_carte.ino
fi
if [ "$sensorFlag" != "" ]
then
    sed -i "1s/^/#define $sensorFlag\n/" examples/menu_a_la_carte/menu_a_la_carte.ino > temp/menu_a_la_carte/menu_a_la_carte.ino
fi
if [ "$publisherFlag" != "" ]
then
    sed -i "1s/^/#define $publisherFlag\n/" examples/menu_a_la_carte/menu_a_la_carte.ino > temp/menu_a_la_carte/menu_a_la_carte.ino
fi

echo "First few lines of source"
head temp/menu_a_la_carte/menu_a_la_carte.ino