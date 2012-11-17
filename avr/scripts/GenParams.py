#!/usr/bin/env python

import paramserverTemplate

import os
import sys
import inspect
import re

def getScriptDir():
	path = os.path.split(inspect.getfile(inspect.currentframe()))[0]
	cmd_folder = os.path.realpath(os.path.abspath(path))
	return cmd_folder


class Parameters:
	def __init__(self):
		self.params = []
	
	def addLine(self, line):
		words = re.split('\W+', line)
		
		# Strip beginning and ending ''
		if words[0] == '':
			words = words[1:]
		if words[-1] == '':
			words = words[:-1]
		
		# Store the parameter name first, then the type, then the condition
		if len(words) == 2:
			self.params.append((words[1], words[0]))
		elif len(words) >= 3:
			self.params.append((words[1], words[0], words[2]))
		else:
			pass # Ignore inadequate strings
	
	def getParams(self):
		return self.params
	
	def __str__(self):
		return str(self.params)


class Message:
	def __init__(self):
		self.params = []
	
	def addLine(self, line):
		words = re.split('\W+', line)
		
		# Strip beginning ''
		if words[0] == '':
			words = words[1:]
		
		# Store the parameter name first, then the type
		if len(words) >= 2:
			self.params.append((words[1], words[0]))
		else:
			pass # Ignore inadequate strings
	
	def getParams(self):
		return self.params
	
	def __str__(self):
		return str(self.params)


class HeaderFile:
	def __init__(self, filepath):
		"""
		Parse a header file looking for parameter and message declarations
		in the docstring of the class.
		
		Parsing is performed on all classes inheriting from FiniteStateMachine
		(see FiniteStateMachine.h). The following types are considered:
		
		* Parameters - The fields that must be included when constructing the FSM
		* Publish - The fields of the message published by the FSM (only 1 msg for now)
		* Subscribe - The fields of the message handled by the FSM (only 1 msg for now)
		
		When the above object types appear in the docstring, a '---' must be
		the following line. The type is terminated by the next '---' found. In
		between these delimiters, fields are declared using their storage
		requirements and their field name. For example,
		
		/*
		 * Parameters:
		 * ---
		 * uint8  id
		 * uint8  pin # IsAnalog
		 * uint32 delay
		 * ---
		 */
		
		An optional comment specifies a constriction (literally a function name
		from the ArduinoVerifier namespace). In this case,
		ArduinoVerifier::IsAnalog(pin) is called when verifying parameters.
		"""
		
		# Only care about .h files
		if os.path.splitext(filepath)[1] != '.h':
			raise
		
		self.filepath = filepath
		self.fsm = {}
		
		# State variables
		docstring = []
		inDocstring = False
		# Need class name and 'public FiniteStateMachine' within 3 lines after docstring
		needClassName = 0
		classNameCandidate = ''
		
		for line in open(filepath):
			line = line.strip()
			if line == '/**':
				# Found docstring start
				inDocstring = True
			
			elif line == '*/' and inDocstring:
				# Found docstring end, now we need class name and
				# "public FiniteStateMachine" in the next 3 lines
				inDocstring = False
				needClassName = 3
			
			elif inDocstring:
				docstring.append(line)
			
			elif needClassName > 0:
				# Decrement our attempts to find a class that extends FiniteStateMachine
				needClassName = needClassName - 1
				# Try to get class name from line
				if classNameCandidate == '' and line.startswith('class'):
					lineCopy = line[len('class'):].strip()
					if len(lineCopy):
						classNameCandidate = re.split('\s', lineCopy, 1)[0]
						# Don't set needClassName to 0 until we know for sure if it's a FSM
				
				if classNameCandidate != '' and 'public FiniteStateMachine' in line:
					# Found our FSM class name and docstring, now do something with it
					self.handleDocstring(classNameCandidate, docstring)
					# And now continue looking for more FSM docstrings of any subsequent classes
					classNameCandidate = ''
					docstring = []
					needClassName = 0
			
			elif needClassName == 0 and classNameCandidate != '':
				# Make sure to reset classNameCandidate if we run out of attempts
				classNameCandidate = ''
		
		# All done. Only continue without raising if a valid FSM docstring was found
		if not self.fsm:
			raise
	
	def handleDocstring(self, className, docstring):
		parameters = None # Empty Parameters()
		publish = None    # Empty Message()
		subscribe = None  # Empty Message()
		
		for i in range(len(docstring) - 3): # Allow 3 lines for ---, a line, and ---
			if 'parameters' in docstring[i].lower() and '---' in docstring[i + 1]:
				parameters = Parameters()
				for line in docstring[i + 2 : ]:
					if '---' in line:
						break
					parameters.addLine(line)
			if 'publish' in docstring[i].lower() and '---' in docstring[i + 1]:
				publish = Message()
				for line in docstring[i + 2 : ]:
					if '---' in line:
						break
					publish.addLine(line)
			if 'subscribe' in docstring[i].lower() and '---' in docstring[i + 1]:
				subscribe = Message()
				for line in docstring[i + 2 : ]:
					if '---' in line:
						break
					subscribe.addLine(line)
		
		if parameters:
			self.fsm[className] = (parameters, publish, subscribe)
	
	def getPath(self):
		return self.filepath
	
	def getFSMs(self):
		return self.fsm


def GenParams():
	print('-- Parsing FiniteStateMachine headers to generate ParamServer.h')

	output = os.path.realpath(os.path.join(getScriptDir(), '..', 'include', 'ParamServer.h'))
	headerdir = os.path.realpath(os.path.join(getScriptDir(), '..', 'src'))
	
	# Compare against ParamServer.h's last update time
	outputmod = os.path.getmtime(output)
	run = False
	for header in os.listdir(headerdir):
		if os.path.getmtime(os.path.join(headerdir, header)) > outputmod:
			run = True
			break
	if not run:
		print('-- No files modified, exiting')
		return
	
	# Create a dictionary of FSMs discovered in parsed header files
	fsmDict = {}
	for header in os.listdir(headerdir):
		# Create a new object and attempt to parse the file
		try:
			headerfile = HeaderFile(os.path.join(headerdir, header))
			# If parsing succeeds, add the file object to the list
			print('Found docstring in %s' % header)
			fsmDict.update(headerfile.getFSMs())
		except:
			print('No docstring found in %s' % header)
	
	# Header files parsed. Resulting structure looks like this:
	# fsmDict = {
	#   "Blink": (
	#     Parameters.getParams() = ("pin", "uint8", "IsDigital"), # Parameters
	#     Message.getParams() = ("pin", "uint8"),                 # Publish
	#     Message.getParams() = ("pin", uint8")                   # Subscribe
	#   ),
	#   "AnalogPublisher": (...)
	# }
	
	print('-- Successfully generated ParamServer.h')

if __name__ == '__main__':
	GenParams()
