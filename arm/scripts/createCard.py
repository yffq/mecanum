#!/usr/bin/env python

import os
import subprocess
import shutil

from common import getScriptDir, loadSettings, getSetting, ensureDependencies

def createCard():
	os.chdir(getScriptDir())
	loadSettings()
	
	fs = 'ext4' # btrfs is waaaaaaaaay too slow on a microSD card
	# Set swap equal to amount of RAM for kernel compiling, consider disabling
	# swap for production images
	swap = 512 # MB, set to zero to disable
	
	# Install dependencies
	ensureDependencies(['uboot-mkimage', 'wget', 'pv', 'dosfstools', 'parted'])
	
	# Look for u-boot and MLO
	useStable = True # as opposed to latest GIT
	uboot = False
	mlo = False
	ubootdir = os.path.join('Bootloader-Builder', 'deploy', 'beagleboard')
	if os.path.exists(ubootdir):
		found = useStable
		for f in sorted(os.listdir(ubootdir), reverse=True):
			if os.path.isfile(os.path.join(ubootdir, f)) and 'MLO' in f:
				# Stable will end in -r1. GIT will end in -def (three hex chars)
				# Therefore, in the real stable image comes last and the latest
				# GIT will be second-to-last. Simply use "useStable" as a flag
				# to target the second-to-last.
				if found:
					mlo = os.path.realpath(os.path.join(ubootdir, f))
					print('Found MLO: ' + mlo)
					break
				else:
					found = True
		found = useStable
		for f in sorted(os.listdir(ubootdir), reverse=True):
			if os.path.isfile(os.path.join(ubootdir, f)) and 'u-boot' in f:
				if found:
					uboot = os.path.realpath(os.path.join(ubootdir, f))
					print('Found u-boot: ' + uboot)
					break
				else:
					found = True
	
	# Build the image
	deploy = os.path.join('omap-image-builder', 'deploy')
	if not os.path.exists(deploy):
		print('Error: omap-image-builder directory doesn\'t exist. Try running buildImage()')
		return
	os.chdir(deploy)
	for f in sorted(os.listdir('.'), reverse=True):
		if not os.path.isfile(f):
			os.chdir(f)
			break
	else:
		print('Error: images not found. Try running buildImage()')
		return
	# Enter the only folder
	for f in os.listdir('.'):
		if not os.path.isfile(f):
			os.chdir(f)
			break
	else:
		print('Error: images not found. Try running buildImage()')
		return
	cmd = ['sudo', './setup_sdcard.sh', '--mmc', getSetting('mmc'),
		'--uboot', 'beagle_xm', '--rootfs', fs,
		'--boot_label', 'boot', '--rootfs_label', 'rootfs']
	if uboot and mlo:
		cmd.extend(['--bootloader', uboot, '--spl', mlo])
	if swap:
		cmd.extend(['--swap_file', str(swap)])
	subprocess.call(cmd)
	os.chdir('..')
	os.chdir('..')
	os.chdir('..')
	os.chdir('..')

if __name__ == '__main__':
	createCard()
