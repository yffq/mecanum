#!/usr/bin/env python

# This script needs to be run with sudo

import os
import subprocess

from common import getScriptDir, loadSettings, getSetting, ensureDependencies

# Configuration parameters
distro='fuerte'
variant='mobile'
target='~/fuerte'

def main():
	# Make sure our current working dir is the script's location
	os.chdir(getScriptDir())
	loadSettings()
	enabledRestricted()
	installDependencies()
	installUtilities()
	installCore()

def enabledRestricted():
	subprocess.call(['./enableRestricted.py'])
	subprocess.call(['apt-get', 'update'])	

def installDependencies():
	# Enable ROS packages -- skip this step because there are no armhf packages
	# sudo sh -c 'echo "deb http://packages.ros.org/ros/ubuntu precise main" > /etc/apt/sources.list.d/ros-latest.list'
	# wget http://packages.ros.org/ros.key -O - | sudo apt-key add -
	ensureDependencies(getSetting('packages'))

# Install rosinstall, rospkg and rosdep utilities
def installUtilities():
	subprocess.call(['pip', 'install', '-U', 'rosinstall', 'rospkg', 'rosdep'])

# Install core libraries into /opt/ros/fuerte
# If /opt/ros exists, we may have done this already
# If ./ros-underlay exists, the previous install might not have finished
def installCore():
	subprocess.call(['rosinstall', '--catkin', os.path.realpath('ros-underlay'),
		'http://theworldaccordingtosue.com/ros/fuerte-ros-base.rosinstall'])
	os.chdir('ros-underlay')
	subprocess.call(['mkdir', 'build'])
	os.chdir('build')
	subprocess.call(['cmake', '..', '-DCMAKE_INSTALL_PREFIX=/opt/ros/fuerte'])
	subprocess.call(['make']) # make -j8 would run out of memory around 90% of the way through
	subprocess.call(['make', 'install'])
	os.chdir('..')
	os.chdir('..')
	# Cleanup after ourselves (sudo make install left some root-owned files)
	subprocess.call(['rm', '-rf', 'ros-underlay'])

if __name__ == '__main__':
	main()
