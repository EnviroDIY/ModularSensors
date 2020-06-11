#!/bin/sh

# Exit with nonzero exit code if anything fails
set -e

cd $TRAVIS_BUILD_DIR/code_docs/ModularSensors

# Build instructions from: https://www.stack.nl/~dimitri/doxygen/download.html
echo "Cloning doxygen repository..."
git clone https://github.com/doxygen/doxygen.git doxygen-src

cd doxygen-src

echo "Create build folder..."
mkdir build
cd build

echo "Make..."
cmake -G "Unix Makefiles" ..
make

echo "Done building doxygen..."
./bin/doxygen -v

echo "doxygen path: " $(pwd)

echo "Install Doxygen"
cp ./bin/* $TRAVIS_BUILD_DIR/code_docs/ModularSensors
#make install

cd $TRAVIS_BUILD_DIR/code_docs/ModularSensors
