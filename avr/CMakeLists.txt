# Arduino Toolchain. See https://github.com/queezythegreat/arduino-cmake
# ROS sets this to rostoolchain.cmake. Override it to build our Arduino code.
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/arduino-cmake/cmake/ArduinoToolchain.cmake)

cmake_minimum_required(VERSION 2.8)

project(avr)

set(ARDUINO_DEFAULT_BOARD mega2560) # Default Board ID, when not specified
set(ARDUINO_DEFAULT_PORT /dev/ttyACM0) # Default Port, when not specified

generate_arduino_firmware(avr_base
    SRCS src/base.cpp
#    HDRS src/main.h
#    LIBS lib/main_lib
)

#catkin_project(${PROJECT_NAME}
#  INCLUDE_DIRS include # avr includes don't do much good
#  LIBRARIES ${PROJECT_NAME}
#)

