#!/usr/bin/env python

import os, sys
import shutil # for copyfile
import fileinput # for inline text replacement
from xml.etree import ElementTree
import subprocess

# Configuration parameters
distro='fuerte'
variant='mobile'
target='~/fuerte'

def main():
	# Make sure our current working dir is the script's location
	path = os.path.dirname(os.path.realpath(__file__))
	os.chdir(path)
	enabledRestricted()
	installDependencies()
	installUtilities()
	installCore()
	installMobile()

def enabledRestricted():
	# First, back up the file
	subprocess.call(['sudo', 'cp', '/etc/apt/sources.list', '/etc/apt/sources.list.bak'])
	try:
		for lin in fileinput.input('/etc/apt/sources.list', inplace = 1):
			if line.endswith('main universe multiverse'):
				line = line + ' restricted'
			sys.out.write(line)
		subprocess.call(['sudo', 'rm', '/etc/apt/sources.list.bak'])
	except:
		subprocess.call(['sudo', 'rm', '/etc/apt/sources.list'])
		subprocess.call(['sudo', 'mv', '/etc/apt/sources.list.bak', '/etc/apt/sources.list'])

def installDependencies():
	# Enable ROS packages -- skip this step because there are no armhf packages
	# sudo sh -c 'echo "deb http://packages.ros.org/ros/ubuntu precise main" > /etc/apt/sources.list.d/ros-latest.list'
	# wget http://packages.ros.org/ros.key -O - | sudo apt-key add -
	
	# Read in the packages from settings.xml
	packages = []
	for packageNode in ElementTree.parse('settings.xml').getroot().findall('packages'):
		if packageNode.text:
			for pkg in str(packageNode.text).split(' '):
				packages.append(pkg)
	
	# Do the install
	subprocess.call(['sudo', 'apt-get', 'update'])
	subprocess.call(['sudo', 'apt-get', 'install', '-y'].extend(packages))

# Install rosinstall, rospkg and rosdep utilities
def installUtilities():
	subprocess.call(['sudo', 'pip', 'install', '-U', 'rosinstall', 'rospkg', 'rosdep'])

# Install core libraries into /opt/ros/fuerte
# If /opt/ros exists, we may have done this already
# If ./ros-underlay exists, the previous install might not have finished
def installCore():
	subprocess.call(['rosinstall', '--catkin', os.path.realpath('ros-underlay'),
		'http://ros.org/rosinstalls/fuerte-ros-base.rosinstall'])
	os.chdir('ros-underlay')
	subprocess.call(['mkdir', 'build'])
	os.chdir('build')
	subprocess.call(['cmake', '..', '-DCMAKE_INSTALL_PREFIX=/opt/ros/fuerte'])
	subprocess.call(['make']) # make -j8 would run out of memory around 90% of the way through
	subprocess.call(['sudo', 'make', 'install'])
	os.chdir('..')
	os.chdir('..')
	# Cleanup after ourselves
	subprocess.call(['rm', '-rf', 'ros-underlay')])

# Higher-level robotics libraries and tools
# See REP 113 (http://www.ros.org/reps/rep-0113.html) for variants
# Use the 'mobile' capability variant extending the base 'robot' variant
# This gives us the following stacks: ros, ros_comm, bond_core, common_msgs,
# common, diagnostics, driver_common, eigen, filters, bullet, geometry,
# nodelet_core, orocos_kinematics_dynamics, pluginlib, assimp, robot_model,
# executive_smach, xacro, navigation, slam_gmapping, laser_pipeline,
# perception_pcl
def installMobile():
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
	main()
