#!/usr/bin/env python

import os
import subprocess
import urllib2

from common import getScriptDir, gitCloneAndEnter

def installRosDeps():
	# Make sure our current working dir is the script's location
	os.chdir(getScriptDir())
	
	if not os.path.exists('rosdeps'):
		os.makedirs('rosdeps')
	os.chdir('rosdeps')
	
	# yaml-cpp
	# Required by the navigation stack
	# If 0.3.0 doesn't work, use https://github.com/wg-debs/yaml_cpp-release (0.2.7)
	if not os.path.exists('/usr/local/lib/libyaml-cpp.a'):
		if os.path.exists('yaml-cpp'):
			subprocess.call(['rm', '-rf', 'yaml-cpp'])
		get('http://yaml-cpp.googlecode.com/files/yaml-cpp-0.3.0.tar.gz')
		os.chdir('yaml-cpp')
		os.makedirs('build')
		os.chdir('build')
		subprocess.call(['cmake', '..'])
		subprocess.call(['make'])
		subprocess.call(['sudo', 'make', 'install'])
		os.chdir('..')
		os.chdir('..')
		subprocess.call(['rm', '-rf', 'yaml-cpp'])
	
	# ros-fuerte-pcl
	# Required by the perception_pcl stack
	if not os.path.exists('/usr/local/lib/libpcl_common.so'):
		# Install build dependencies (others may be required)
		subprocess.call(['sudo', 'apt-get', 'install', 'libflann-dev'])
		if os.path.exists('pcl'):
			subprocess.call(['rm', '-rf', 'pcl'])
		gitCloneAndEnter('git://github.com/wg-debs/pcl.git')
		os.makedirs('build')
		os.chdir('build')
		subprocess.call(['cmake', '-DUSE_ROS=ON', '..'])
		subprocess.call(['make'])
		subprocess.call(['sudo', 'make', 'install'])
		os.chdir('..')
		os.chdir('..')
		subprocess.call(['rm', '-rf', 'pcl'])
	
	# All done
	os.chdir('..')
	subprocess.call(['rm', '-rf', 'rosdeps'])

def get(url):
	download = urllib2.urlopen(url)
	filename = url.rpartition('/')[2]
	output = open(filename, 'wb')
	output.write(download.read())
	output.close()
	if filename.endswith('.tar.gz'):
		subprocess.call(['tar', '-zxf', filename])


if __name__ == '__main__':
	installRosDeps()
