#!/usr/bin/env python

import os
import subprocess

from common import getScriptDir, gitCloneAndEnter, loadSettings, \
	getSetting, ensureDependencies, replaceAll

def buildImage():
	os.chdir(getScriptDir())
	loadSettings()
	
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
	gitCloneAndEnter('git://github.com/RobertCNelson/omap-image-builder.git')
	#subprocess.call(['git', 'reset', '--hard', 'HEAD'])
	
	patches = [
		'0001-Only-build-Precise-image.patch',
		'0002-Include-additional-packages-specified-in-settings.xm.patch',
		'0003-Force-MAC-address.patch',
		'0004-Remove-text-from-etc-flash-kernel.conf.patch',
		'0005-Copy-keys-to-the-new-filesystem.patch']
	if imgpath:
		patches.append('0006-Primary-kernel-is-on-local-filesystem.patch')
	if False:
		patches.append('0007-Run-script-to-install-ros.patch')
	for p in patches:
		subprocess.call(['git', 'am', os.path.join(getScriptDir(), 'patches',
			'omap-image-builder', p)])
	
	#subprocess.call(['git', 'checkout', 'v2012.4-1', '-b', 'v2012.4-1'])
	
	# Configure image builder
	replaceAll('build_image.sh', 'FQDN="omap"', 'FQDN="' + getSetting('fqdn') + '"')
	replaceAll('build_image.sh', 'USER_LOGIN="ubuntu"', 'USER_LOGIN="' + getSetting('username') + '"')
	replaceAll('build_image.sh', 'USER_PASS="temppwd"', 'USER_PASS="' + getSetting('password') + '"')
	replaceAll('build_image.sh', 'USER_NAME="Demo User"', 'USER_NAME="' + getSetting('name') + '"')
	replaceAll('build_image.sh', '__MECANUM_PACKAGES__', ','.join(getSetting('packages')))
	if imgpath:
		# Kernel image, e.g. linux-image-3.2.18-x12_1.0precise_armhf.deb
		replaceAll('build_image.sh', '__KERNEL_DEB_FILE__', imgpath)
	
	replaceAll('tools/fixup.sh', 'DE:AD:BE:EF:CA:FE', getSetting('macaddress'))
	# Attempt to copy our ssh keys to the new filesystem
	try:
		id_rsa = open(getScriptDir() + '../ssh_keys/id_rsa', 'r')
		rsa_private = id_rsa.read()
		id_rsa.close()
		id_rsa_pub = open(getScriptDir() + '../ssh_keys/id_rsa.pub', 'r')
		rsa_public = id_rsa_pub.read()
		id_rsa_pub.close()
		if (len(rsa_private) and len(rsa_public)):
			replaceAll('tools/fixup.sh', '#USER_NAME=__USER_NAME__', 'USER_NAME="' + getSetting('username') + '"')
			replaceAll('tools/fixup.sh', '__RSA_PRIVATE__', rsa_private)
			replaceAll('tools/fixup.sh', '__RSA_PUBLIC__', rsa_public)
	except:
		pass
	
	# Build the image
	subprocess.call(['./build_image.sh'])
	os.chdir('..')

if __name__ == '__main__':
	buildImage()
