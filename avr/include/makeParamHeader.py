#!/usr/bin/env python

import os
import sys
import inspect

def getScriptDir():
	path = os.path.split(inspect.getfile(inspect.currentframe()))[0]
	cmd_folder = os.path.realpath(os.path.abspath(path))
	return cmd_folder

sys.path.insert(0, getScriptDir())
from param import headerfile, template


# File directories
TEMPLATE_FILE = os.path.realpath(os.path.join(getScriptDir(), 'ParamServer.tmpl.h'))
OUTPUT_FILE = os.path.realpath(os.path.join(getScriptDir(), 'ParamServer.h'))
HEADER_DIR = os.path.realpath(os.path.join(getScriptDir(), '..', 'src'))


class FSMMap:
	def __init__(self):
		self.map = {'fsm': []}
	
	def add(self, fsmlist):
		self.map['fsm'].extend(fsmlist)
	
	def hasFSM(self):
		return len(self.map['fsm']) > 0
	
	def getMap(self):
		return self.map


def GenHeader():
	print('-- Parsing FiniteStateMachine headers to generate ParamServer.h')
	
	# Compare against ParamServer.h's last update time
	if os.path.exists(OUTPUT_FILE):
		run = False
		outputmod = os.path.getmtime(OUTPUT_FILE)
		# First, check against the template file itself
		if os.path.getmtime(TEMPLATE_FILE) > outputmod:
			run = True
		for header in os.listdir(HEADER_DIR):
			# Only care about header files
			if os.path.splitext(os.path.join(HEADER_DIR, header))[1] != '.h':
				continue
			# Compare the modification time against our target header file
			if os.path.getmtime(os.path.join(HEADER_DIR, header)) > outputmod:
				run = True
				break
		if not run:
			print('-- No files modified, exiting')
			return
	
	# Create a dictionary of FSMs discovered in parsed header files. Use ROOT
	# as the wrapping root node (that's what our template expects)
	fsmMap = FSMMap()
	for header in os.listdir(HEADER_DIR):
		# Only care about header files
		if os.path.splitext(os.path.join(HEADER_DIR, header))[1] != '.h':
			continue
		# Create a new object and attempt to parse the file
		try:
			# Perform the parsing
			headerFileObject = headerfile.HeaderFile(os.path.join(HEADER_DIR, header))
			# If parsing succeeds, add the file object to the list
			fsmMap.add(headerFileObject.getFSMs())
		except:
			pass # No docstring found
	
	if not fsmMap.hasFSM():
		print('-- No headers parsed, exiting')
		return
	
	# Import the template text
	templateText = open(TEMPLATE_FILE).read()
	
	# Create a template root tag
	templateTag = template.Tag(templateText)
	
	# Unify the fsm data with the root tag
	outputText = templateTag.unify(fsmMap.getMap())
	
	# Store the result in the output file
	open(OUTPUT_FILE, 'w').write(outputText)
	
	print('-- Successfully generated ParamServer.h')


if __name__ == '__main__':
	GenHeader()
