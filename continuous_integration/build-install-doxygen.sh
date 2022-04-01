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

echo "\e[32m\n\n\nCurrent TeX version...\e[0m"
tex --version
echo "\n\n\n"

# install Graphviz for DOT class diagrams
sudo apt-get -y install graphviz

echo "\e[32m\n\n\nCurrent graphviz version...\e[0m"
dot -v
echo "\n\n\n"

cd $TRAVIS_BUILD_DIR

if [ ! -f $TRAVIS_BUILD_DIR/doxygen-src/build/bin/doxygen ]; then

    # Build instructions from: https://www.stack.nl/~dimitri/doxygen/download.html
    echo "\e[32mCloning doxygen repository...\e[0m"
    git clone https://github.com/doxygen/doxygen.git doxygen-src --branch $DOXYGEN_VERSION --depth 1

    cd doxygen-src

    echo "\e[32mCreate build folder...\e[0m"
    mkdir build
    cd build

    echo "\e[32mMake...\e[0m"
    cmake -G "Unix Makefiles" ..
    make
    echo "\e[32mDone building doxygen.\e[0m"
    echo "\e[32mdoxygen path: \e[0m" $(pwd)
fi

echo "\e[32m\n\n\nCurrent Doxygen version...\e[0m"
$TRAVIS_BUILD_DIR/doxygen-src/build/bin/doxygen -v
echo "\n\n\n"

# echo "\e[32mMove Doxygen to working directory"
# cp $TRAVIS_BUILD_DIR/doxygen-src/build/bin/* $TRAVIS_BUILD_DIR/code_docs/ModularSensors
# #make install

cd $TRAVIS_BUILD_DIR/code_docs/ModularSensors
