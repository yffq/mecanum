#!/usr/bin/env python
import os, sys, inspect

def getScriptDir():
	path = os.path.split(inspect.getfile(inspect.currentframe()))[0]
	cmd_folder = os.path.realpath(os.path.abspath(path))
	return cmd_folder

class HeaderFile:
	def parse(filepath):
		self.filepath = filepath
		return True
	
	def getPath():
		return filepath

def GenParams():
	print('-- Parsing FiniteStateMachine headers to generate ParamServer.h')

	output = os.path.realpath(os.path.join(getScriptDir(), '..', 'include', 'ParamServer.h'))
	headerdir = os.path.realpath(os.path.join(getScriptDir(), '..', 'src'))

	outputmod = os.path.getmtime(output)
	run = False
	for header in os.listdir(headerdir):
		if os.path.getmtime(os.path.join(headerdir, header)) > outputmod:
			run = True
			break
	if !run:
		print('-- No files modified, exiting')
		return
	
	# Create a list of parsed header files
	headerfiles = []
	for header in os.listdir(headerdir):
		# Create a new object and attempt to parse the file
		headerfile = HeaderFile()
		if headerfile.parse(os.path.join(headerdir, header)):
			# If parsing succeeds, add the file object to the list
			headerfiles.append(headerfile)

	
	os.listdir(path)

	file = open('test.h', 'w')
	file.write('class test2\n{\n};')
	file.close()
	print('-- Successfully generated ParamServer.h')


if __name__ == '__main__':
	GenParams()
