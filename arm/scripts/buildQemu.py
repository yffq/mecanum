#!/usr/bin/env python

import os
import subprocess

from common import getScriptDir, gitCloneAndEnter, ensureDependencies

def buildQemu():
	os.chdir(getScriptDir())
	# Install dependencies
	ensureDependencies(['libglib2.0-dev', 'binfmt-support', 'qemu'])
	subprocess.call(['sudo', 'apt-get', '-y', 'build-dep', 'qemu'])
	
	# Clone RCN's git repository
	print('Building under Qemu')
	gitCloneAndEnter('git://github.com/RobertCNelson/linaro-tools.git')
	
	# Build and install qemu
	os.chdir('qemu') # MUST CD INTO DIRECTORY
	subprocess.call(['./build_qemu.sh'])
	os.chdir('..')
	os.chdir('..')


if __name__ == '__main__':
	buildQemu()
