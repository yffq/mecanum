#!/usr/bin/env python

import os
import subprocess

from common import getScriptDir, gitCloneAndEnter

def buildBootloader():
	print('Building bootloader')
	os.chdir(getScriptDir())
	# Clone RCN's git repository
	gitCloneAndEnter('git://github.com/RobertCNelson/Bootloader-Builder.git')
	
	# Patch the bootloader
	# GPIO mux settings can be found in u-boot/board/ti/beagle/beagle.h
	patches = [
		'0001-Only-build-for-beagleboard.patch',
		'0002-Add-patch-to-change-boot-delay-to-0-seconds.patch',
		'0003-Add-patch-to-set-BeagleBoard-xM-pin-mux.patch']
	patchdir = os.path.realpath(os.path.join(getScriptDir(), 'patches', 'Bootloader-Builder'))
	for patch in patches:
		subprocess.call(['git', 'am', os.path.join(patchdir, patch)])
	
	# Build the bootloader
	subprocess.call(['./build.sh'])
	os.chdir('..')

if __name__ == '__main__':
	buildBootloader()
