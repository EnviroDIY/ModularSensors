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

echo "Adding flag for pre-named variables"
sed "1s/^/#define BUILD_TEST_PRE_NAMED_VARS\n/" examples/menu_a_la_carte/menu_a_la_carte.ino > temp/menu_a_la_carte/menu_a_la_carte.ino

echo "Adding flag for $modemFlag"
sed -i "1s/^/#define $modemFlag\n/" temp/menu_a_la_carte/menu_a_la_carte.ino

echo "Adding flag for $sensorFlag"
sed -i "1s/^/#define $sensorFlag\n/" temp/menu_a_la_carte/menu_a_la_carte.ino

echo "Adding flag for $publisherFlag"
sed -i "1s/^/#define $publisherFlag\n/" temp/menu_a_la_carte/menu_a_la_carte.ino

echo "\e[32mFirst few lines of source\e[0m"
head temp/menu_a_la_carte/menu_a_la_carte.ino