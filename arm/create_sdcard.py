#!/usr/bin/env python

import os, sys
import fileinput # For system.sh search-and-replace
from xml.etree import ElementTree
import shutil
import subprocess
import time

def main():
	# Make sure our current working directory is the script's location
	path = os.path.dirname(os.path.realpath(__file__))
	os.chdir(path)
	loadSettings()
	#buildBootloader()
	#buildKernel()
	#buildInstallQemu() # For now, assume this is installed, in the future, detect
	buildImage()
	setupCard()

# Utility function
# http://stackoverflow.com/questions/39086/search-and-replace-a-line-in-a-file-in-python
def replaceAll(file, searchExp, replaceExp):
	for line in fileinput.input(file, inplace=1):
		if searchExp in line:
			line = line.replace(searchExp, replaceExp)
		sys.stdout.write(line)

def gitCleanup():
	subprocess.call(['git', 'am', '--abort'])
	subprocess.call(['git', 'add', '.'])
	subprocess.call(['git', 'commit', '--allow-empty', '-a', '-m', 'empty cleanup commit'])
	subprocess.call(['git', 'checkout', 'origin/master', '-b', 'tmp-master'])
	subprocess.call(['git', 'branch', '-D', 'master']) # &>/dev/null || true
	subprocess.call(['git', 'checkout', 'origin/master', '-b', 'master'])
	subprocess.call(['git', 'branch', '-D', 'tmp-master']) # &>/dev/null || true
	subprocess.call(['git', 'pull'])

def loadSettings():
	global username, name, password, fqdn, mmc, macaddress, packages
	settings = ElementTree.parse('settings.xml').getroot()
	username = settings.findtext('username')
	name = settings.findtext('name')
	password = settings.findtext('password')
	fqdn = settings.findtext('fqdn') # Fully-qualified domain name
	mmc = settings.findtext('mmc')
	macaddress = settings.findtext('macaddress')
	packages = []
	for pkglist in settings.findall('packages'):
		if pkglist.text:
			for pkg in str(pkglist.text).split(' '):
				packages.append(pkg)
	print('Creating SD card with the following settings:')
	print('Username: ' + username)
	print('Name: ' + name)
	print('Password: ' + password)
	print('FQDN: ' + fqdn)
	print('MMC: ' + mmc)
	print('MAC address: ' + macaddress)
	print('Packages: ' + ' '.join(packages))

def buildBootloader():
	# Clone RCN's git repository
	print('Building bootloader')
	if not os.path.isdir('Bootloader-Builder'):
		subprocess.call(['git', 'clone', 'git://github.com/RobertCNelson/Bootloader-Builder.git', 'Bootloader-Builder'])
		os.chdir('Bootloader-Builder')
	else:
		os.chdir('Bootloader-Builder')
		gitCleanup()
	
	# Patch the bootloader
	# GPIO mux settings can be found in u-boot/board/ti/beagle/beagle.h
	patches = [
		'0001-Only-build-for-beagleboard.patch',
		'0002-Add-patch-to-change-boot-delay-to-0-seconds.patch']
	for patch in patches:
		subprocess.call(['git', 'am', os.path.join(os.path.realpath('..'), 'patches',
			'Bootloader-Builder', patch)])
	
	# Build the bootloader
	subprocess.call(['./build.sh'])
	os.chdir('..')

def buildKernel():
	global mmc
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
		gitCleanup()
	
	# Configure the kernel build script
	shutil.copyfile('system.sh.sample', 'system.sh') # Overwrites existing file
	file = open('system.sh', 'a') # Open for appending
	file.write('LINUX_GIT=' + os.path.join(os.path.realpath('..'), LinuxDirName) + '\n')
	file.write('ZRELADDR=0x80008000' + '\n') # For TI: OMAP3/4/AM35xx (BB is OMAP3)
	#file.write('BUILD_UIMAGE=1' + '\n') # Do I need to build uImage?
	file.write('MMC=' + mmc + '\n')
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
		subprocess.call(['git', 'pull'])
	
	# Build and install qemu
	os.chdir('qemu') # MUST CD INTO DIRECTORY
	subprocess.call(['./build_qemu.sh'])
	os.chdir('..')
	os.chdir('..')

def buildImage():
	global username, name, password, fqdn, packages
	
	# Look for a kernel image
	imgpath = False # Discovered image goes here
	for path in ['images', os.path.join('stable-kernel', 'deploy')]:
		if not os.path.exists(path):
			continue
		files = sorted(os.listdir(path), reverse=True)
		for f in files:
			if f.endswith('.deb') and 'image' in f:
				imgpath = os.path.realpath(os.path.join(path, f))
				print('Found kernel image: ' + imgpath)
				print('rootstock will use this local image instead of http://rcn-ee.net')
				break
		if imgpath:
			break
	
	# Clone RCN's git repository
	print('Building Ubuntu image')
	if not os.path.isdir('omap-image-builder'):
		subprocess.call(['git', 'clone', 'git://github.com/RobertCNelson/omap-image-builder.git', 'omap-image-builder'])
		os.chdir('omap-image-builder')
		subprocess.call(['git', 'checkout', '-b', 'mecanum'])
	else:
		os.chdir('omap-image-builder')
		subprocess.call(['git', 'reset', '--hard', 'HEAD'])
		gitCleanup()
		subprocess.call(['git', 'branch', '-D', 'mecanum'])
		subprocess.call(['git', 'checkout', '-b', 'mecanum'])
	
	patches = [
		'0001-Only-build-Precise-image.patch',
		'0002-Include-additional-packages-specified-in-settings.xm.patch',
		'0003-Force-MAC-address.patch',
		'0004-Remove-text-from-etc-flash-kernel.conf.patch',
		'0005-Run-script-to-install-ros.patch',
		'0006-Copy-ssh-keys-to-the-new-filesystem.patch']
	
	if imgpath:
		patches.append('0007-Primary-kernel-is-on-local-filesystem.patch')
	
	for p in patches:
		subprocess.call(['git', 'am', os.path.join(os.path.realpath('..'), 'patches',
			'omap-image-builder', p)])
	
	#subprocess.call(['git', 'checkout', 'v2012.4-1', '-b', 'v2012.4-1'])
	
	# Configure image builder
	replaceAll('build_image.sh', 'FQDN="omap"', 'FQDN="' + fqdn + '"')
	replaceAll('build_image.sh', 'USER_LOGIN="ubuntu"', 'USER_LOGIN="' + username + '"')
	replaceAll('build_image.sh', 'USER_PASS="temppwd"', 'USER_PASS="' + password + '"')
	replaceAll('build_image.sh', 'USER_NAME="Demo User"', 'USER_NAME="' + name + '"')
	replaceAll('build_image.sh', '__MECANUM_PACKAGES__', ','.join(packages))
	if imgpath:
		# Kernel image, e.g. linux-image-3.2.18-x12_1.0precise_armhf.deb
		replaceAll('build_image.sh', '__KERNEL_DEB_FILE__', imgpath)
	
	replaceAll('tools/fixup.sh', 'DE:AD:BE:EF:CA:FE', macaddress)
	# Attempt to copy our ssh keys to the new filesystem
	id_rsa = open('../ssh_keys/id_rsa', 'r')
	rsa_private = id_rsa.read()
	id_rsa.close()
	id_rsa_pub = open('../ssh_keys/id_rsa.pub', 'r')
	rsa_public = id_rsa_pub.read()
	id_rsa_pub.close()
	if (len(rsa_private) and len(rsa_public)):
		replaceAll('tools/fixup.sh', '#USER_NAME=__USER_NAME__', 'USER_NAME="' + username + '"')
		replaceAll('tools/fixup.sh', '__RSA_PRIVATE__', rsa_private)
		replaceAll('tools/fixup.sh', '__RSA_PUBLIC__', rsa_public)
	
	# Build the image
	subprocess.call(['./build_image.sh'])
	os.chdir('..')

def setupCard():
	global mmc
	fs = 'ext4' # btrfs is waaaaaaaaay too slow on a microSD card
	
	# Install dependencies
	subprocess.call(['sudo', 'apt-get', '-y', 'install',
		'uboot-mkimage', 'wget', 'pv', 'dosfstools', 'parted'])
	
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
	cmd = ['sudo', './setup_sdcard.sh', '--mmc', mmc,
		'--uboot', 'beagle_xm', '--rootfs', fs,
		'--boot_label', 'boot', '--rootfs_label', 'rootfs']
	if uboot and mlo:
		cmd.extend(['--bootloader', uboot, '--spl', mlo])
	subprocess.call(cmd)
	os.chdir('..')
	os.chdir('..')
	os.chdir('..')
	os.chdir('..')


if __name__ == "__main__":
	main()
