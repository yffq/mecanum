#!/bin/sh
# Run this in the stack directory to generate makefiles for the given packages
# (space-separated). Make sure that the stack folder is included in
# ROS_PACKAGE_PATH.
rosmake --target=eclipse-project --specified-only avr_controller
