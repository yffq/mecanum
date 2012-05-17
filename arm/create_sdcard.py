#!/usr/bin/env python

import os, sys
import fileinput # For system.sh search-and-replace
import shutil
import subprocess

MMC = '/dev/mmcblk0'

def main():
	# Make sure our current working directory is the script's location
	path = os.path.dirname(os.path.realpath(__file__))
	os.chdir(path)
	#buildKernel()
	buildInstallQemu()
	buildImage()
	setupCard()

# Utility function
# http://stackoverflow.com/questions/39086/search-and-replace-a-line-in-a-file-in-python
def replaceAll(file, searchExp, replaceExp):
	for line in fileinput.input(file, inplace = 1):
		if searchExp in line:
			line = line.replace(searchExp, replaceExp)
		sys.stdout.write(line)

def buildKernel():
	global MMC
	# Install dependencies
	subprocess.call(['sudo', 'apt-get', '-y', 'install', 'gcc-arm-linux-gnueabi'])
	
	# Clone the linux kernel source tree
	print('Cloning the Linux kernel source tree')
	LinuxDirName = 'linux-stable'
	if not os.path.isdir(LinuxDirName):
		subprocess.call(['git', 'clone', 'git://git.kernel.org/pub/scm/linux/kernel/git/stable/linux-stable.git', LinuxDirName])
	else:
		os.chdir(LinuxDirName)
		subprocess.call(['git', 'pull', 'origin'])
		os.chdir('..')
	
	# Clone RCN's git repository
	print('Building kernel')
	if not os.path.isdir('stable-kernel'):
		subprocess.call(['git', 'clone', 'git://github.com/RobertCNelson/stable-kernel.git', 'stable-kernel'])
		os.chdir('stable-kernel')
	else:
		os.chdir('stable-kernel')
		subprocess.call(['git', 'pull', 'origin'])
	
	# Configure the kernel build script
	shutil.copyfile('system.sh.sample', 'system.sh') # Overwrites existing file
	file = open('system.sh', 'a') # Open for appending
	file.write('LINUX_GIT=' + os.path.join(os.path.realpath('..'), LinuxDirName) + '\n')
	file.write('ZRELADDR=0x80008000' + '\n') # For TI: OMAP3/4/AM35xx (BB is OMAP3)
	#file.write('BUILD_UIMAGE=1' + '\n') # Do I need to build uImage?
	file.write('MMC=' + MMC + '\n')
	# Pull in Torvalds current master tree before applying local patchset
	# This is very useful during an intial 'rc0' merge.
	# It is never supported... Enable at your own risk
	#file.write('LATEST_GIT=1' + '\n')
	file.close()
	# Adding the CC parameter is a little more complex... we need to seek out
	# the config line and uncomment it.
	replaceAll('system.sh', '#CC=arm-linux-gnueabi-', 'CC=arm-linux-gnueabi-')
	
	# Build the kernel
	subprocess.call(['./build_kernel.sh'])
	os.chdir('..')

def buildInstallQemu():
	# Install dependencies
	subprocess.call(['sudo', 'apt-get', '-y', 'install',
		'libglib2.0-dev', 'binfmt-support', 'qemu'])
	subprocess.call(['sudo', 'apt-get', '-y', 'build-dep', 'qemu'])
	
	# Clone RCN's git repository
	print('Building under Qemu')
	if not os.path.isdir('linaro-tools'):
		subprocess.call(['git', 'clone', 'git://github.com/RobertCNelson/linaro-tools.git', 'linaro-tools'])
		os.chdir('linaro-tools')
	else:
		os.chdir('linaro-tools')
		subprocess.call(['git', 'reset', '--hard', 'HEAD'])
		subprocess.call(['git', 'pull', 'origin'])
	
	# Build and install qemu
	os.chdir('qemu') # MUST CD INTO DIRECTORY
	subprocess.call(['./build_qemu.sh'])
	os.chdir('..')
	os.chdir('..')

# Observed: 42 minutes to build image
def buildImage():
	# Clone RCN's git repository
	print('Building Ubuntu image')
	if not os.path.isdir('omap-image-builder'):
		subprocess.call(['git', 'clone', 'git://github.com/RobertCNelson/omap-image-builder.git', 'omap-image-builder'])
		os.chdir('omap-image-builder')
		# Only build Precise image
		subprocess.call(['git', 'am', os.path.join(os.path.realpath('..'), 'patches',
			'omap-image-builder', 'only_build_precise.patch')])
	else:
		os.chdir('omap-image-builder')
		subprocess.call(['git', 'reset', '--hard', 'HEAD'])
		subprocess.call(['git', 'pull', 'origin'])
	
	#subprocess.call(['git', 'checkout', 'v2012.4-1', '-b', 'v2012.4-1'])
	
	# Configure image builder
	replaceAll('build_image.sh', 'FQDN="omap"', 'FQDN="mecanum"')
	replaceAll('build_image.sh', 'USER_LOGIN="ubuntu""', 'USER_LOGIN="garrett"')
	replaceAll('build_image.sh', 'USER_PASS="temppwd"', 'USER_PASS="password"')
	replaceAll('build_image.sh', 'USER_NAME="Demo User"', 'USER_NAME="Garrett"')
	# Only build Precise image

	# Build the image
	subprocess.call(['./build_image.sh'])

def setupCard():
	global MMC
	os.chdir('omap-image-builder')
	os.chdir('deploy')
	os.chdir('2012-05-16-STABLE') # TODO
	os.chdir('ubuntu-12.04-r3-minimal-armhf') # TODO
	subprocess.call(['sudo', './setup_sdcard.sh',
		'--mmc', MMC, '--uboot', 'beagle_xm', '--rootfs', 'btrfs'])
	os.chdir('..')
	os.chdir('..')
	os.chdir('..')
	os.chdir('..')
	

if __name__ == "__main__":
	main()

