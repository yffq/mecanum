import re

class History:
	def __init__(self, tagName, tagObject, isLast):
		self.tagName = tagName
		self.tagObject = tagObject
		self.isLast = isLast


class Property:
	def __init__(self, tagName, propertyName):
		'''
		The first letter of the returned value propertyName is made to match
		the case of its propertyName.
		'''
		self.isCapitalized = propertyName[0].isupper()
		self.tagName = tagName.lower()
		self.propertyName = propertyName.lower()
	
	def getTagName(self):
		return self.tagName
	
	def render(self, tagObject, tagHistory):
		'''
		Render a template using data from object. Object structure looks like:
		{
			"prop1": value1,
			"prop2": value2,
			"subtag1name": [
				{
					"prop1": value3
					"prop2": value4,
					... (more subtag properties)
				},
				... (more subtag1 objects)
			],
			... (more tag properties and subtag objects)
		}
		'''
		# Base case: run out of history items, bail out of the parent tag
		if len(tagHistory) == 0:
			raise Exception()
		
		# Create a local copy so we can pop its cherry
		tagHistory = tagHistory[:]
		
		# Confirm that this property was meant for the top tag
		if self.tagName == tagHistory[len(tagHistory) - 1].tagName:
			if self.propertyName in tagObject:
				propertyValue = tagObject[self.propertyName]
				propertyValue = (propertyValue[0].upper() if self.isCapitalized else propertyValue[0].lower()) + propertyValue[1:]
				return propertyValue
			else:
				raise Exception()
		else:
			tagHistory.pop()
			return self.render(tagHistory[len(tagHistory) - 1].tagObject, tagHistory)


class Comma:
	def __init__(self, parentTagName):
		self.tagName = parentTagName.lower()
		
	def getTagName(self):
		return self.tagName
	
	def render(self, tagObject, tagHistory):
		# Only render the comma if the tagName matches the top history's tagName
		if len(tagHistory) > 0 and self.tagName == tagHistory[len(tagHistory) - 1].tagName:
			return ',' if not tagHistory[len(tagHistory) - 1].isLast else ''
		
		# Nothing string should happen, but if something did, invalidate the tag
		raise Exception()


class Tag:
	'''
	Templates are composed of tags. There are several kinds of tags:
	
	Repetition tags - These tags have the format <%TAGNAME repeated text %>.
	Tags are matched to a dictionary data structure, where the key is the tag
	name and the value is a list of objects. In the template's output, the tag
	is instantiated once for each object in the list, and the rendered tags are
	repeated serially and inserted into the document. Repetition tags can be
	nested within other repetition tags, where they recurse through the multi-
	level object data looking for their tag name. Also, imagine the entire
	template is wrapped in the single-repetition <%ROOT ... %> tag, as it were.
	If a repetition tag contains an Attribute tag with an undefined attribute,
	that repetition tag will be skipped for that instance.
	
	Attribute tags - Object properties can be accessed using
	<%TAGNAME.ATTRIBUTE%>. This tag is replaced by the property of its object.
	A nested tag can include an attribute from a parent tag.
	
	Comment tags - Text between <%-- and --%> is ignored.
	
	Comma tags - The <%,%> tag has a special function. When included in a
	repetition tag, it is rendered normally for each instance of the tag;
	however, it is omitted on the final repetition.
	
	If <% is not followed by [A-Za-z0-9_.,-]+ it will be included in the
	template verbatim.
	'''
	def __init__(self, text, name=None):
		# Our parsing result is a heterogeneous list including strings and tag
		# objects. Use 'root' as a pseudo name if not specified.
		self.tagName = name.lower() if name is not None else 'root'
		self.elements = []
		
		# Consume text as we parse out tags
		while len(text):
			# Ignore all characters up to the first <%
			splitText = text.split('<%', 1)
			self.elements.append(splitText[0])
			if len(splitText) == 1:
				break # No more text to parse
			
			# Match the tag name
			text = splitText[1]
			result = re.match('[A-Za-z0-9_.,-]+', text)
			if not result:
				# No tag name, ignore the result (<% is already stripped from text)
				self.elements.append('<%')
				continue
			
			tagName = result.group()
			
			# The tag name is pushed onto the stack, so truncate text here
			# text now begins immediately after the tag name
			text = text[len(tagName):]
			
			# If a whitespace immediately follows the tag name, skip it
			if text[0] == '\n' or text[0] == ' ' or text[0] == '\t':
				text = text[1:]
			
			# Handle the comments tag <%-- --%>
			if tagName[:2] == '--':
				splitText = text.split('--%>', 1)
				if len(splitText) > 1:
					text = splitText[1]
					# Ignore a newline directly after the end tag
					if text[0] == '\n':
						text = text[1:]
				else:
					# Otherwise, ignore the tag and append the skipped text
					self.elements.append('<%' + tagName)
				continue
			
			# Handle the comma tag <%,%>
			if tagName == ',':
				# Look for the end tag
				splitText = text.split('%>', 1)
				if len(splitText) > 1:
					# The comma belongs to its containing tag
					self.elements.append(Comma(self.tagName))
					text = splitText[1]
				else:
					# %> not found
					self.elements.append('<%' + tagName)
				continue
			
			if '.' in tagName:
				# Look for the end tag
				splitText = text.split('%>', 1)
				if len(splitText) > 1:
					parts = tagName.split('.', 1)
					self.elements.append(Property(parts[0], parts[1]))
					text = splitText[1]
				else:
					# %> not found
					self.elements.append('<%' + tagName)
				continue
			
			# Extract the sub-tag and remove it from the beginning of text
			stack = [tagName]
			i = 0
			while len(stack) > 0 and i < len(text):
				if text[i : i + 2] == '%>':
					# Don't increment by 2 on the last terminator, because we
					# don't want to include the last %> in the tag.
					stack.pop()
					if len(stack) > 0:
						i += 2
				elif text[i : i + 2] == '<%':
					i += 2
					result = re.match('[A-Za-z0-9_.,-]+', text[i:])
					if result:
						nestedTagName = result.group()
						# Don't push <%-- onto the stack
						if nestedTagName[:2] == '--':
							# Skip the comment by incrementing i the length 
							# of the comment. Unlike above, we ignore the
							# comment even if its terminator isn't found. Oh well.
							i += len(text[i:].split('--%>', 1)[0]) + len('--%>')
						else:
							stack.append(nestedTagName)
							i += len(nestedTagName)
				else:
					i += 1
			
			# If stack was depleted, we found a sub-tag
			if len(stack) == 0:
				self.elements.append(Tag(text[:i], tagName))
				
				# Now skip the trailing %>
				i += 2
				
				# Fast forward i characters for the next iteration
				text = text[i:]
				
				# Ignore a whitespace directly after the end tag
				if text[0] == '\n' or text[0] == ' ' or text[0] == '\t':
					text = text[1:]
			else:
				# No tag found, append the raw text and finish up
				self.elements.append(text)
				break # outer while loop
	
	def getTagName(self):
		'''
		This is used to identify which tag name this element expects to be
		rendered for.
		'''
		return self.tagName
	
	def unify(self, tagObject, tagHistory=None):
		'''
		Render a template using data from object. Object structure looks like:
		{
		  "tagname": [
		    {
		      "prop1": value1,
		      "prop2": value2,
		      "subtag1name": [
		        {
		          "prop1": value3
		          "prop2": value4,
		          ... (more subtag properties)
		        },
		        ... (more subtag1 objects)
		      ],
		      ... (more tag properties and subtag objects)
		    },
		    ... (more tag objects)
		  ]
		}
		'''
		# Create a local copy so we can append to it
		tagHistory = tagHistory[:] if tagHistory else []
		
		output = ''
		# If an element in the tag fails, skip the whole tag (but note, not its parent)
		try:
			for element in self.elements:
				if isinstance(element, Tag):
					tagName = element.getTagName()
					if tagName in tagObject:
						# Render tagItems, one after the other
						i = 0
						for subTagObject in tagObject[tagName]:
							# The sub tag knows its own tagName, but sub sub
							# tags rely on this information. This collapses
							# {tagName: [{}]} to {tagName: {}} for each item in
							# the tagName list.
							i += 1
							isLast = i == len(tagObject[tagName])
							tagHistory.append(History(tagName, subTagObject, isLast))
							output += element.unify(subTagObject, tagHistory)
							tagHistory.pop()
					else:
						# If the object wasn't supplied, skip it and move on
						output += ''
				elif not isinstance(element, str):
					output += element.render(tagObject, tagHistory)
				else:
					# Element is a plain old string
					output += element
		except:
			output = ''
		# All done
		return output

