#!/usr/bin/env python

import os
import subprocess

from common import getScriptDir, ensureDependencies

# Configuration parameters
distro='fuerte'
variant='mobile'
target='~/fuerte'

# Higher-level robotics libraries and tools
# See REP 113 (http://www.ros.org/reps/rep-0113.html) for variants
# Use the 'mobile' capability variant extending the base 'robot' variant
# This gives us the following stacks: ros, ros_comm, bond_core, common_msgs,
# common, diagnostics, driver_common, eigen, filters, bullet, geometry,
# nodelet_core, orocos_kinematics_dynamics, pluginlib, assimp, robot_model,
# executive_smach, xacro, navigation, slam_gmapping, laser_pipeline,
# perception_pcl
def installMobile():
	# Make sure our current working dir is the script's location
	os.chdir(getScriptDir())
	pass

#'rosinstall ' + target + ' "http://packages.ros.org/cgi-bin/gen_rosinstall.py?"' +
#	'rosdistro=' + distro + '&variant=' + variant + '&overlay=no"'

# Environment setup
#'echo "source ' + target + '/setup.bash" > ~/.bashrc'

# Build higher-level libraries and tools
#sudo rosdep init
#rosdep update ### This might need sudo
# Install system dependencies
#rosdep install -a
# Build the ROS stacks using rosmake
#rosmake -a

if __name__ == '__main__':
	installMobile()
