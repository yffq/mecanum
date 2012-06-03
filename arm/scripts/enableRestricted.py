#!/usr/bin/env python

# This script needs to be run with sudo

import sys
import fileinput
import subprocess

def enabledRestricted():
	# First, back up the file (fileinput uses .bak, so use .bak2)
	subprocess.call(['cp', '/etc/apt/sources.list', '/etc/apt/sources.list.bak2'])
	try:
		for line in fileinput.input('/etc/apt/sources.list', inplace = 1):
			if line.endswith('main universe multiverse\n'):
				line = line[:-1] + ' restricted\n'
			sys.stdout.write(line)
		# Succeeded, delete the backup
		subprocess.call(['rm', '/etc/apt/sources.list.bak2'])
	except:
		# Failed, restore the backup
		subprocess.call(['rm', '/etc/apt/sources.list'])
		subprocess.call(['mv', '/etc/apt/sources.list.bak2', '/etc/apt/sources.list'])

if __name__ == '__main__':
	enabledRestricted()
