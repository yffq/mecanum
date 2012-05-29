# Arduino Toolchain. See https://github.com/queezythegreat/arduino-cmake
# ROS sets this to 'rostoolchain.cmake'. Override it to build our Arduino code.
cmake_minimum_required(VERSION 2.8)
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/arduino-cmake/cmake/ArduinoToolchain.cmake)

# Our AVR code depends on the arduino-core package corresponding to the Arduino 1.0
# release. A script to detect the arduino-core version might look something like:
#
# $ dpkg --get-selections | grep arduino-core
# arduino-core		install
# $ apt-cache show arduino-core | grep -i version
# Version: 1:1.0+dfsg-9
#
# If arduino-core isn't present, the sources can be found at:
# http://arduino.googlecode.com/files/arduino-1.0-src.tar.gz
#
# On the ARM port of Ubuntu Oneiric, arduino-core was at version 0022. I'm not
# sure if this was due to the version (Oneiric) or the ARM port. I'll try
# Precise on the ARM processor and see if arduino-core 1.0 is offered by apt.

project(avr)

set(ARDUINO_DEFAULT_BOARD mega2560) # Default Board ID, when not specified
set(ARDUINO_DEFAULT_PORT /dev/ttyACM0) # Default Port, when not specified

# Hack so that Eclipse can find the pins_arduino.h file specific to the mega2560
# The "mega" part of this directory is determined by looking in boards.txt (see
# the arduino sdk path) and finding the boardID.build.variant setting.
include_directories("${ARDUINO_VARIANTS_PATH}/mega")

# Public include files so that programs can interface with the Arduino hardware
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include")

# Tell CMake where to search for libraries use the link_directories command.
# The command has to be used before defining any firmware or libraries
# requiring those libraries. An Arduino library is ***any directory which
# contains a header named after the directory.***
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/libraries)

generate_arduino_firmware(avr_base
    SRCS src/main.cpp
         src/BatteryMonitor.cpp
         src/Blink.cpp
         src/ChristmasTree.cpp
         src/Fade.cpp
         src/FSMVector.cpp
         src/MecanumMaster.cpp
         src/Mimic.cpp
         src/Toggle.cpp
#    HDRS src/FiniteStateMachine.h # For project-based systems
#    LIBS lib/main_lib
)

#catkin_project(${PROJECT_NAME}
#  INCLUDE_DIRS include # avr includes don't do much good
#  LIBRARIES ${PROJECT_NAME}
#)

