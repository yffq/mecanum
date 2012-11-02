cmake_minimum_required(VERSION 2.4.6)
include($ENV{ROS_ROOT}/core/rosbuild/rosbuild.cmake)

# Set the build type.  Options are:
#  Coverage       : w/ debug symbols, w/o optimization, w/ code-coverage
#  Debug          : w/ debug symbols, w/o optimization
#  Release        : w/o debug symbols, w/ optimization
#  RelWithDebInfo : w/ debug symbols, w/ optimization
#  MinSizeRel     : w/o debug symbols, w/ optimization, stripped binaries
#set(ROS_BUILD_TYPE RelWithDebInfo)

rosbuild_init()

#set the default path for built executables to the "bin" directory
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
#set the default path for built libraries to the "lib" directory
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

#uncomment if you have defined messages
#rosbuild_genmsg()
#uncomment if you have defined services
#rosbuild_gensrv()

set(AVRCONTROLLER_SRCS src/AVRController.cpp
)

# Build the .so
rosbuild_add_library(${PROJECT_NAME} ${AVRCONTROLLER_SRCS})
###target_link_libraries(${PROJECT_NAME} another_library)
rosbuild_add_boost_directories()
rosbuild_link_boost(${PROJECT_NAME} system thread)

#rosbuild_add_executable(avr_node src/AVRNode.cpp)
#target_link_libraries(avr_node ${PROJECT_NAME})

# Build the test
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread") # fix for Ubuntu 11.10+
rosbuild_add_gtest(avrtest test/avrtest.cpp
                           src/BBExpansionPin.cpp
                           src/GPIO.cpp
                           src/Thumbwheel.cpp)
target_link_libraries(avrtest ${PROJECT_NAME})


