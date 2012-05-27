#!/usr/bin/env python

import os, sys
import shutil # for copyfile
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
	subprocess.call(['sudo', './arm_enable_restricted.py'])

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
	#subprocess.call(['sudo', 'apt-get', 'install', '-y'].extend(packages)) # TypeError: 'NoneType' object is not iterable
	cmd = ['sudo', 'apt-get', 'install', '-y']
	cmd.extend(packages)
	subprocess.call(cmd)

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
	# Cleanup after ourselves (sudo make install left some root-owned files)
	subprocess.call(['sudo', 'rm', '-rf', 'ros-underlay'])

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


pip_messages = '''
Downloading/unpacking rosdep
  Downloading rosdep-0.9.3.tar.gz (56Kb): 56Kb downloaded
  Running setup.py egg_info for package rosdep
    failed to load symbols, rosdep will not function properly
    Cannot import rospkg, rosdep will not function properly

Downloading/unpacking pyyaml (from rosinstall)
  Downloading PyYAML-3.10.zip (364Kb): 364Kb downloaded
  Running setup.py egg_info for package pyyaml

  Found existing installation: PyYAML 3.10
    Uninstalling PyYAML:
      Successfully uninstalled PyYAML
  Running setup.py install for pyyaml
    checking if libyaml is compilable
    gcc -pthread -fno-strict-aliasing -DNDEBUG -g -fwrapv -O2 -Wall -Wstrict-prototypes -fPIC -I/usr/include/python2.7 -c build/temp.linux-armv7l-2.7/check_libyaml.c -o build/temp.linux-armv7l-2.7/check_libyaml.o
    build/temp.linux-armv7l-2.7/check_libyaml.c:2:18: fatal error: yaml.h: No such file or directory
    compilation terminated.
    
    libyaml is not found or a compiler error: forcing --without-libyaml
    (if libyaml is installed correctly, you may need to
     specify the option --include-dirs or uncomment and
     modify the parameter include_dirs in setup.cfg)
    
Successfully installed rosinstall rospkg rosdep vcstools pyyaml
'''
