#!/usr/bin/env python

import os
import fileinput
import subprocess
import urllib2

from common import getScriptDir

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
	
	if not os.path.exists('/opt/ros/fuerte/setup.bash'):
		subprocess.call(['sudo', './installRosCore.py'])
		if not os.path.exists('/opt/ros/fuerte/setup.bash'):
			print('Script failed: sudo ./installRosCore.py')
			return
	
	# Acquire sudo privileges for later
	subprocess.call(['sudo', 'cat', '/dev/null'])
	
	url = 'http://packages.ros.org/cgi-bin/gen_rosinstall.py?rosdistro=' + \
			distro + '&variant=' + variant + '&overlay=no'
	subprocess.call(['rosinstall', os.path.expanduser(target), url])
	
	# Environment setup
	addBashSource('/opt/ros/fuerte/setup.bash')
	# Update the environment so that the changes take effect immediately
	if not os.environ.has_key('ROS_ROOT'):
		updateEnvironment('/opt/ros/fuerte/setup.bash')
	# Bug: Why does this get a newline after ~ ?
	targetsrc = os.path.expanduser(target + '/setup.bash').replace('\n', '')
	addBashSource(targetsrc)
	if not os.environ.has_key('ROS_WORKSPACE'):
		updateEnvironment(targetsrc)
	
	# Build higher-level libraries and tools
	subprocess.call(['sudo', 'rosdep', 'init'])
	subprocess.call(['sudo', 'rosdep', 'update']) # This might not need sudo
	
	# Install dependencies (from source, if necessary)
	installDependencies()
	
	# Build the ROS stacks using rosmake
	subprocess.call(['rosmake', '-a'])

def addBashSource(sourcetarget):
	envstr = 'source ' + sourcetarget + '\n'
	# Bug: sometimes, path get a newline after ~
	bashrc = os.path.expanduser('~/.bashrc').replace('\n', '')
	if not contains(bashrc, envstr):
		file = open(bashrc, 'a')
		file.write(envstr)
		file.close()
	return

# Remember, include '\n' in your searchExp
def contains(filepath, searchExp):
	for line in fileinput.input(filepath):
		if line == searchExp:
			fileinput.close()
			return True
	fileinput.close()
	return False

def updateEnvironment(source):
	command = ['bash', '-c', 'source ' + source + ' && env']
	proc = subprocess.Popen(command, stdout=subprocess.PIPE)
	for line in proc.stdout:
		(key, _, value) = line.partition("=")
		# Same bug as above: everything seems to get a \n appended to it
		os.environ[key] = value.replace('\n', '')
	proc.communicate()
	#pprint.pprint(dict(os.environ))
	return

def installDependencies():
	# Install system dependencies
	subprocess.call(['rosdep', 'install', '-a']) # -ay


if __name__ == '__main__':
	installMobile()
