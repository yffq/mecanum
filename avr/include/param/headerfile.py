import re


def translateType(shorthand):
	if shorthand in ['int8', 'uint8', 'int16', 'uint16', 'int32', 'uint32']:
		return shorthand + '_t'
	else:
		return shorthand


class Parameters:
	def __init__(self):
		self.parameters = []
	
	def addLine(self, line):
		words = re.split('\W+', line)
		
		# Strip beginning and ending ''
		if words[0] == '':
			words = words[1:]
		if words[-1] == '':
			words = words[:-1]
		
		# Store the parameter name first, then the type, then the condition
		if len(words) == 2:
			paramsObject = {"name": words[1], "type": translateType(words[0])}
			self.parameters.append(paramsObject)
		elif len(words) >= 3:
			paramsObject = {"name": words[1], "type": translateType(words[0]), "test": words[2]}
			self.parameters.append(paramsObject)
		else:
			pass # Ignore strings that are too short
	
	def getParams(self):
		return self.parameters


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
		 * uint8  pin # IsAnalog
		 * uint32 delay
		 * ---
		 */
		
		An optional comment specifies a constriction (literally a function name
		from the ArduinoVerifier namespace). In this case,
		ArduinoVerifier::IsAnalog(pin) is called when verifying parameters.
		"""
		self.filepath = filepath
		self.fsms = []
		
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
		if not self.fsms:
			raise Exception()
	
	def handleDocstring(self, className, docstring):
		parameters = None
		publish = None
		subscribe = None
		
		for i in range(len(docstring) - 3): # Allow 3 lines for ---, a line, and ---
			if 'parameters' in docstring[i].lower() and '---' in docstring[i + 1]:
				parameters = Parameters()
				for line in docstring[i + 2 : ]:
					if '---' in line:
						break
					parameters.addLine(line)
			if 'publish' in docstring[i].lower() and '---' in docstring[i + 1]:
				publish = Parameters()
				for line in docstring[i + 2 : ]:
					if '---' in line:
						break
					publish.addLine(line)
			if 'subscribe' in docstring[i].lower() and '---' in docstring[i + 1]:
				subscribe = Parameters()
				for line in docstring[i + 2 : ]:
					if '---' in line:
						break
					subscribe.addLine(line)
		
		# FSM structure looks like
		# {
		#   "fsm": [
		#     {
		#       "name": classname,
		#       "id": FSM_ID,
		#       "parameter": [
		#         {
		#           "name": "pin",
		#           "type": "uint8_t",
		#           "test": "IsDigital"
		#         }
		#       ],
		#       "message": [
		#         {
		#           "which": "Publish",
		#           "parameter": [
		#             {
		#               "name": "pin",
		#               "type": "uint8_t"
		#             }
		#           ]
		#         },
		#         {
		#           "which": "Subscribe",
		#           "parameter": [
		#             {
		#               "name": "pin",
		#               "type": "uint8_t"
		#             }
		#           ]
		#         }
		#       ]
		#     }
		#   ]
		# }
		fsm = {"name": className, "id": "FSM_***"} # TODO: FSM ID
		
		if parameters and len(parameters.getParams()) > 0:
			fsm["parameter"] = parameters.getParams()
		if publish and len(publish.getParams()):
			fsm["message"] = [{"which": "Publish", "parameter": publish.getParams()}]
		else:
			fsm["message"] = []
		if subscribe:
			fsm["message"].append({"which": "Subscribe", "parameter": subscribe.getParams()})
		
		self.fsms.append(fsm)
	
	def getFSMs(self):
		return self.fsms

