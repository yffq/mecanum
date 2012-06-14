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
#
# Oneiric includes gcc-avr 1:4.5.3-3, which may contain bugs. Case in point:
# The compiler bug (http://gcc.gnu.org/bugzilla/show_bug.cgi?id=45263) has
# tainted many users (the "-3" on the version means Debian backported the
# patch, so no more compiler bug).
#
# gcc-avr can be updated to 4.7 using Quantal Quetzal's packages. Add the
# following lines to /etc/apt/sources.list:
# # Ubuntu Quantal Quetzal Repositories (for gcc-avr 1:4.7.0-2)
# deb http://ports.ubuntu.com/ubuntu-ports/ quantal main universe
#
# Then run apt-get update and sudo apt-get install gcc-avr=1:4.7.0-2.
#
# In retrospect, avr-libc v1.8.0 breaks arduino-core. Probably don't upgrade.

project(avr)

# If None is specified, the debug flag -g causes debug info to be included in the hex
set(CMAKE_BUILD_TYPE Release)

set(ARDUINO_DEFAULT_BOARD mega2560) # Default Board ID, when not specified
set(ARDUINO_DEFAULT_PORT /dev/ttyACM0) # Default Port, when not specified

# Hack so that Eclipse can find the pins_arduino.h file specific to the mega2560
# The "mega" part of this directory is determined by looking in boards.txt (see
# the arduino sdk path) and finding the boardID.build.variant setting.
include_directories("${ARDUINO_VARIANTS_PATH}/mega")

# Public include files so that programs can interface with the Arduino hardware
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include")

# Local include files so that public include files can find their referenced classes
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/src")

# Tell CMake where to search for libraries use the link_directories command.
# The command has to be used before defining any firmware or libraries
# requiring those libraries. An Arduino library is ***any directory which
# contains a header named after the directory.***
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/libraries)

# Generate ParamServer.h
#execute_process(COMMAND python ${CMAKE_CURRENT_SOURCE_DIR}/scripts/GenParams.py)

set(mecanum_srcs src/main.cpp
                 src/AnalogPublisher.cpp
                 src/BatteryMonitor.cpp
                 src/Blink.cpp
                 src/ChristmasTree.cpp
                 src/DigitalPublisher.cpp
                 src/Fade.cpp
                 src/FSMVector.cpp
                 src/MecanumMaster.cpp
                 src/Mimic.cpp
                 src/TinyBuffer.cpp
                 src/Toggle.cpp
)

#set(debug_srcs   ${mecanum_srcs})

generate_arduino_firmware(mecanum
    SRCS ${mecanum_srcs}
#    LIBS lib/main_lib
)

# Generate ParamServer.h
# On Linux, this only runs after the target is built
#add_custom_command(TARGET mecanum PRE_BUILD COMMAND python ${CMAKE_CURRENT_SOURCE_DIR}/scripts/GenParams.py)

#generate_arduino_firmware(debug
#    SRCS ${debug_srcs}
#    HDRS src/FiniteStateMachine.h # For project-based systems
#    LIBS lib/main_lib
#)

#catkin_project(${PROJECT_NAME}
#  INCLUDE_DIRS include # avr includes don't do much good
#  LIBRARIES ${PROJECT_NAME}
#)

