cmake_minimum_required(VERSION 2.4.6)
include($ENV{ROS_ROOT}/core/rosbuild/rosbuild.cmake)

# Set the build type.  Options are:
#  Coverage       : w/ debug symbols, w/o optimization, w/ code-coverage
#  Debug          : w/ debug symbols, w/o optimization
#  Release        : w/o debug symbols, w/ optimization
#  RelWithDebInfo : w/ debug symbols, w/ optimization
#  MinSizeRel     : w/o debug symbols, w/ optimization, stripped binaries
set(ROS_BUILD_TYPE RelWithDebInfo)

rosbuild_init()

# Set the default path for built executables to the "bin" directory
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
# Set the default path for built libraries to the "lib" directory
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)
# Set the compiler optimization flags for BeagleBoard-XM arm-hf
set(BEAGLEBOARD_XM_FLAGS "-march=armv7-a -mtune=cortex-a8 -mfpu=neon -ftree-vectorize -ffast-math -mfloat-abi=hard")

#uncomment if you have defined messages
#rosbuild_genmsg()
#uncomment if you have defined services
#rosbuild_gensrv()

#set(AVRCONTROLLER_SRCS src/AVRController.cpp
#)

# Build the .so
#rosbuild_add_library(${PROJECT_NAME} ${AVRCONTROLLER_SRCS})
#rosbuild_add_boost_directories()
#rosbuild_link_boost(${PROJECT_NAME} system thread)
#rosbuild_add_compile_flags(${PROJECT_NAME} ${BEAGLEBOARD_XM_FLAGS})

# Build the node
#rosbuild_add_executable(avr_node src/AVRNode.cpp)
#target_link_libraries(avr_node ${PROJECT_NAME})
#rosbuild_add_compile_flags(avr_node ${BEAGLEBOARD_XM_FLAGS})

# Build Upstart test
set(UPSTART_SRCS src/Upstart.cpp
                 src/AVRController.cpp
                 src/GPIO.cpp
)
rosbuild_add_executable(upstart ${UPSTART_SRCS})
rosbuild_add_boost_directories()
rosbuild_link_boost(upstart system thread)
rosbuild_add_compile_flags(upstart ${BEAGLEBOARD_XM_FLAGS})

# Build the test
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread") # fix for Ubuntu 11.10+
rosbuild_add_gtest(avrtest test/avrtest.cpp
                           src/AVRController.cpp
                           src/GPIO.cpp
                           src/I2CBus.cpp
                           src/IMU.cpp
                           src/MotorController.cpp
                           src/Thumbwheel.cpp)
target_link_libraries(avrtest ${PROJECT_NAME})
rosbuild_add_compile_flags(avrtest ${BEAGLEBOARD_XM_FLAGS})

# Create the GPIO helper program
rosbuild_add_executable(gpio_export src/GPIOExport.cpp)
#execute_process(COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/gpio_export_rights.sh)
add_custom_command(TARGET gpio_export POST_BUILD COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/gpio_export_rights.sh)

# Create the helper shutdown program
rosbuild_add_executable(system_shutdown src/SystemShutdown.cpp)
#execute_process(COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/gpio_export_rights.sh)
add_custom_command(TARGET system_shutdown POST_BUILD COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/system_shutdown_rights.sh)

