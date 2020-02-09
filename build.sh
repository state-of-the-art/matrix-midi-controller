#!/bin/sh

[ "${ARDUINO_ROOT}" ] || ARDUINO_ROOT="${HOME}/local/arduino-1.8.10"
[ "${ARDUINO_LIBS}" ] || ARDUINO_LIBS="${HOME}/Arduino/libraries"

build_dir=$(mktemp -d "/tmp/arduino-build.XXXXXXXX")
trap "rm -rf ${build_dir}" EXIT

"${ARDUINO_ROOT}/arduino-builder" -compile -logger=machine -hardware "${ARDUINO_ROOT}/hardware" -tools "${ARDUINO_ROOT}/tools-builder" -tools "${ARDUINO_ROOT}/hardware/tools/avr" -built-in-libraries "${ARDUINO_ROOT}/libraries" -libraries "${ARDUINO_LIBS}" -fqbn=teensy:avr:teensy31:usb=serialmidi,speed=72,opt=o2std,keys=en-us -build-path "${build_dir}" -warnings=none -verbose mmc/mmc.ino

mv "${build_dir}/mmc.ino.hex" ./
