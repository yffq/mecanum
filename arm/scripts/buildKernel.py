#!/usr/bin/env python

import os
import subprocess
import shutil

from common import getScriptDir, gitCloneAndEnter, loadSettings, \
	getSetting, ensureDependencies, replaceAll

def buildKernel():
	os.chdir(getScriptDir())
	loadSettings()
	
	# Install dependencies
	ensureDependencies(['gcc-arm-linux-gnueabi'])
	
	# Clone the linux kernel source tree
	print('Cloning the Linux kernel source tree')
	linuxDirName = gitCloneAndEnter('git://git.kernel.org/pub/scm/linux/kernel/git/stable/linux-stable.git', 'master')
	os.chdir('..')
	
	# Clone RCN's git repository
	print('Building kernel')
	gitCloneAndEnter('git://github.com/RobertCNelson/stable-kernel.git', '4d82ccd1b2093')
	
	# Configure the kernel build script
	shutil.copyfile('system.sh.sample', 'system.sh') # Overwrites existing file
	file = open('system.sh', 'a') # Open for appending
	file.write('LINUX_GIT=' + os.path.join(os.path.realpath('..'), linuxDirName) + '\n')
	file.write('ZRELADDR=0x80008000' + '\n') # For TI: OMAP3/4/AM35xx (BB is OMAP3)
	#file.write('BUILD_UIMAGE=1' + '\n') # Do I need to build uImage?
	file.write('MMC=' + getSetting('mmc') + '\n')
	# Pull in Torvalds current master tree before applying local patchset
	# This is very useful during an intial 'rc0' merge.
	# It is never supported... Enable at your own risk
	#file.write('LATEST_GIT=1' + '\n')
	file.write('LOCAL_PATCH_DIR=' + os.path.join(os.path.realpath('..'),
		'patches', 'stable-kernel') + '\n')
	file.close()
	# Adding the CC parameter is a little more complex... we need to seek out
	# the config line and uncomment it.
	replaceAll('system.sh', '#CC=arm-linux-gnueabi-', 'CC=arm-linux-gnueabi-')
	
	# Build the kernel
	subprocess.call(['./build_deb.sh'])
	os.chdir('..')

if __name__ == '__main__':
	buildKernel()
