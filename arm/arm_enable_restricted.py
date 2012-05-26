#!/usr/bin/env python

# This script needs to be run with sudo

import fileinput
import subprocess

def enabledRestricted()
	# First, back up the file
	subprocess.call(['cp', '/etc/apt/sources.list', '/etc/apt/sources.list.bak'])
	try:
		for lin in fileinput.input('/etc/apt/sources.list', inplace = 1):
			if line.endswith('main universe multiverse'):
				line = line + ' restricted'
			sys.out.write(line)
		# Succeeded, delete the backup
		subprocess.call(['rm', '/etc/apt/sources.list.bak'])
	except:
		# Failed, restore the backup
		subprocess.call(['rm', '/etc/apt/sources.list'])
		subprocess.call(['mv', '/etc/apt/sources.list.bak', '/etc/apt/sources.list'])

if __name__ == '__main__':
	enabledRestricted()
