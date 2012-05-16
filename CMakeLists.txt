# Arduino Toolchain. See https://github.com/queezythegreat/arduino-cmake
#set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/avr/arduino-cmake/cmake/ArduinoToolchain.cmake)

cmake_minimum_required(VERSION 2.8)

# ROSMAKE Code

include($ENV{ROS_ROOT}/core/rosbuild/rosbuild.cmake)
rosbuild_make_distribution(0.1.0)

# CATKIN Code

#project(mecanum)

# Find the catkin package first and then call catkin_stack(). This is only
# necessary if you need the variables exported by catkin_stack.
#find_package(catkin REQUIRED)
# catkin_stack() or catkin_stack(ros_comm)

# This line is for anything that is common to all subprojects and not handled
# by catkin_stack.
# optional find_package(xxx REQUIRED)

# Call catkin_python_setup() if the project contains a setup.py / Python code
# which should installed.
# optional catkin_python_setup()


#foreach(subdir
#    avr
#    mecanum
#    )
#  add_subdirectory(${subdir})
#endforeach()

# install all manifests
#install_matching_to_share(manifest.xml)

#install(FILES stack.yaml stack.xml 
#        DESTINATION share/${PROJECT_NAME})

