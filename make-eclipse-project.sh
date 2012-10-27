#!/bin/sh
# Run this in the stack directory to generate makefiles for the packages listed
# below. Make sure that this stack folder is included in ROS_PACKAGE_PATH.
rosmake --target=eclipse-project --specified-only \
	avr_controller

# The avr package needs special treatment:
cd avr
cmake -G"Eclipse CDT4 - Unix Makefiles"
