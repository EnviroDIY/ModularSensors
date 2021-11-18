#!/bin/sh

# Exit with nonzero exit code if anything fails
set -e

echo "\n\e[32mCurrent Arduino CLI version:\e[0m"
arduino-cli version

echo "\n\e[32mDeleting any archived zips\e[0m"
rm -f home/arduino/downloads/ModularSensors.zip

echo "\n\e[32mDownloading library zip from ${LIBRARY_INSTALL_ZIP}\e[0m"
curl -L  --retry 15 --retry-delay 0 ${LIBRARY_INSTALL_ZIP} --create-dirs -o home/arduino/downloads/ModularSensors.zip

echo "\n\e[32mUnzipping the library\e[0m"
unzip -o home/arduino/downloads/ModularSensors.zip -d home/arduino/downloads/ -x "*.git/*" "continuous_integration/*" "docs/*" "examples/*"

echo "\n\e[32mEnsuring no old directories exist\e[0m"
rm -r -f home/arduino/user/libraries/ModularSensors

echo "\n\e[32mCreating a new directory for the testing version of Modular sensors\e[0m"
mkdir -p home/arduino/user/libraries/ModularSensors

echo "\n\e[32mMoving the unzipped library to the new directory\e[0m"
if [ -z "${GITHUB_HEAD_REF}" ]; then
echo "\n\e[36mExpected unzipped directory name (from SHA): home/arduino/downloads/ModularSensors-${GITHUB_SHA}\e[0m"
mv home/arduino/downloads/ModularSensors-${GITHUB_SHA}/* home/arduino/user/libraries/ModularSensors
else
SAVED_ZIP_NAME=${GITHUB_HEAD_REF} | sed -e 's/\//-/g'
echo "\n\e[36mExpected unzipped directory name (from head): home/arduino/downloads/ModularSensors-${SAVED_ZIP_NAME}\e[0m"
mv home/arduino/downloads/ModularSensors-${SAVED_ZIP_NAME}/* home/arduino/user/libraries/ModularSensors
fi

echo "\n\e[32mUpdating the library index\e[0m"
arduino-cli --config-file continuous_integration/arduino_cli.yaml lib update-index

echo "\n\e[32mListing libraries detected by the Arduino CLI\e[0m"
arduino-cli --config-file continuous_integration/arduino_cli.yaml lib list

echo "\n\e[32mListing the contents of the Arduino library directory\e[0m"
ls home/arduino/user/libraries