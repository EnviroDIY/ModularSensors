#!/bin/sh

# Exit with nonzero exit code if anything fails
set -e

# install all the dependencies for make for Doxygen and m.css
sudo apt-get update
sudo apt-get -y install build-essential
sudo apt-get -y install flex
sudo apt-get -y install bison

# install TeX Live for LaTeX formula rendering
sudo apt-get -y install texlive-base
sudo apt-get -y install texlive-latex-extra
sudo apt-get -y install texlive-fonts-extra
sudo apt-get -y install texlive-fonts-recommended

echo "Current TeX version..."
tex --version

# install Graphviz for DOT class diagrams
sudo apt-get -y install graphviz

echo "Current graphviz version..."
dot -v

cd $TRAVIS_BUILD_DIR

if [ ! -f $TRAVIS_BUILD_DIR/doxygen-src/build/bin/doxygen ]; then

    # Build instructions from: https://www.stack.nl/~dimitri/doxygen/download.html
    echo "Cloning doxygen repository..."
    git clone https://github.com/doxygen/doxygen.git doxygen-src --branch $DOXYGEN_VERSION --depth 1

    cd doxygen-src

    echo "Create build folder..."
    mkdir build
    cd build

    echo "Make..."
    cmake -G "Unix Makefiles" ..
    make
    echo "Done building doxygen."
    echo "doxygen path: " $(pwd)
fi

echo "Current Doxygen version..."
$TRAVIS_BUILD_DIR/doxygen-src/build/bin/doxygen -v

# echo "Move Doxygen to working directory"
# cp $TRAVIS_BUILD_DIR/doxygen-src/build/bin/* $TRAVIS_BUILD_DIR/code_docs/ModularSensors
# #make install

cd $TRAVIS_BUILD_DIR/code_docs/ModularSensors
