.PHONY: travis-build

travis-build:
platformio ci --lib="." $(PLATFORMIO_CI_ARGS)
