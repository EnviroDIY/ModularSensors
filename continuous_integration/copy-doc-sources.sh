#!/bin/bash

# Makes the bash script print out every command before it is executed, except echo
trap '[[ $BASH_COMMAND != echo* ]] && echo $BASH_COMMAND' DEBUG

# Script modified from scripts by Jeroen de Bruijn, thephez, and Adafruit
# https://gist.github.com/vidavidorra/548ffbcdae99d752da02
# https://github.com/thephez/doxygen-travis-build
# https://learn.adafruit.com/the-well-automated-arduino-library/travis-ci

# Exit with nonzero exit code if anything fails
set -e

# Create a clean working directory for this script.
mkdir $GITHUB_WORKSPACE/code_docs
cd $GITHUB_WORKSPACE/code_docs

# Re-clone the main repo, not sparsely
git clone -b master --depth 1 https://github.com/EnviroDIY/ModularSensors ModularSensors

# Clone the wiki, because we'll be using it in the docs
git clone --depth 1 https://github.com/EnviroDIY/ModularSensors.wiki

# Clone m.css for its theming
git clone --depth 1 https://github.com/SRGDamia1/m.css m.css

# Get the current gh-pages branch
# git clone -b gh-pages https://git@$GH_REPO_REF
# cd $GH_REPO_NAME
git clone -b gh-pages --depth 1 https://github.com/EnviroDIY/ModularSensors ModularSensorsDoxygen
cd ModularSensorsDoxygen
echo "Documentation path: " $(pwd)

# Remove everything currently in the gh-pages branch.
# GitHub is smart enough to know which files have changed and which files have
# stayed the same and will only update the changed files. So the gh-pages branch
# can be safely cleaned, and it is sure that everything pushed later is the new
# documentation.
rm -rf *

# Need to create a .nojekyll file to allow filenames starting with an underscore
# to be seen on the gh-pages site. Therefore creating an empty .nojekyll file.
# Presumably this is only needed when the SHORT_NAMES option in Doxygen is set
# to NO, which it is by default. So creating the file just in case.
echo "" > .nojekyll
ls
