#!/usr/bin/env python
import os, sys, inspect

def getScriptDir():
	path = os.path.split(inspect.getfile(inspect.currentframe()))[0]
	cmd_folder = os.path.realpath(os.path.abspath(path))
	return cmd_folder

class HeaderFile:
	



def GenParams():
	print('-- Parsing FiniteStateMachine headers to generate ParamServer.h')
	os.chdir(getScriptDir())
	file = open('test.h', 'w')
	file.write('class test2\n{\n};')
	file.close()
	print('-- Successfully generated ParamServer.h')


if __name__ == '__main__':
	GenParams()
