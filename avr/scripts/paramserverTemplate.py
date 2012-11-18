import re

import pprint

class Property:
	def __init__(self, tagName, propertyName):
		self.tagName = tagName.lower()
		self.propertyName = propertyName.lower()
	
	def getTagName(self):
		return self.tagName
	
	def render(self, tagObject, parentTree):
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
		print('Rendering Property ' + self.tagName + '.' + self.propertyName)
		#pprint.PrettyPrinter(indent=1).pprint(tagObject)
		if self.propertyName in tagObject:
			print('Return value is ' + tagObject[self.propertyName])
			return tagObject[self.propertyName]
		elif parentTree is not None:
			for tag in parentTree:
				if self.propertyName in tag:
					print('Return value is ' + tag[self.propertyName])
					return tag[self.propertyName]
		print('Property name not found, raising exception')
		raise Exception() # property name not found in object or any of object's parents


class Comma:
	def __init__(self, parentTagName):
		self.tagName = parentTagName.lower()
		
	def getTagName(self):
		return self.tagName
	
	def render(self, tagObject, parentTree):
		'''
		Render a template using data from object. Object structure looks like:
		{
			"TAGNAME": [
				{
					"PROP1": value1,
					"PROP2": value2,
					"SUBTAG1NAME": [
						{
							"PROP1": value3
							"PROP2": value4,
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
		print('Rendering Comma')
		# Get the tag object that this comma tag refers to
		# We only consider parentTree and ignore tagObject, because the comma
		# inclusion decision only concerns the parent tag
		if parentTree is not None:
			for tag in parentTree:
				if self.tagName in tag:
					# Found our tag. Match tagObject until we hit the end
					i = 0
					for i in range(len(tag[self.tagName])):
						if tagObject == tag[self.tagName][i]:
							print('Returning [' + (',' if i != len(tag[self.tagName]) else '') + ']')
							return ',' if i != len(tag[self.tagName]) else ''
		# A match must be found, if not something went wrong
		print('Something went wrong, raising exception')
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
		# objects. Use 'ROOT' as a psuedo name if not specified; render()
		# expects this.
		self.tagName = name.lower() if name is not None else 'root'
		self.elements = []
		
		# Consume text as we parse out tags
		while len(text):
			if text[:2] == '%>':
				print('TEXT STARTS WITH %>')
			
			# Ignore all characters up to the first <%
			splitText = text.split('<%', 1)
			self.elements.append(splitText[0])
			print(self.tagName + ': Found text length %d' % len(splitText[0]))
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
			print(self.tagName + ': Found tag named ' + tagName + ', let\'s see what it is...')
			
			# The tag name is pushed onto the stack, so truncate text here
			# text now begins immediately after the tag name
			text = text[len(tagName):]
			
			# If a newline immediately follows the tag name, skip it
			if text[0] == '\n':
				text = text[1:]
			
			# Handle the comments tag <%-- --%>
			if tagName[:2] == '--':
				splitText = text.split('--%>', 1)
				if len(splitText) > 1:
					text = splitText[1]
					print(self.tagName + ': Found a comment, ignoring %d characters' % len(splitText[0]))
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
					# Assume the tag containing the comma is its parent
					self.elements.append(Comma(self.tagName))
					print(self.tagName + ': Found a comma')
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
					print(self.tagName + ': Found a Property: ' + parts[0] + '.' + parts[1])
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
				print(self.tagName + ': Found a subtag ' + tagName + ', length: %d' % i)
				
				# Now skip the trailing %>
				i += 2
				
				# Fast forward i characters for the next iteration
				text = text[i:]
				
				# Ignore a newline directly after the end tag
				if text[0] == '\n':
					text = text[1:]
			else:
				# No tag found, append the raw text and finish up
				self.elements.append(text)
				print(self.tagName + ': No more tags, adding %d characters of text' % len(text))
				break # outer while loop
	
	def getTagName(self):
		'''
		This is used to identify which tag name this element expects to be
		rendered for.
		'''
		return self.tagName
	
	def render(self, tagObject, parentTree=None):
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
		
		The top-level tag name is root (
		
		parentTree is a list if accumulated parent objects, in order of reverse
		depth, with only property (non-list) elements included. This is
		included so subtags can use attributes of their parent tag.
		'''
		print('Rendering tag ' + self.tagName)
		
		output = ''
		# If an element in the tag fails, skip the whole tag
		try:
			for element in self.elements:
				if not isinstance(element, str):
					tagName = element.getTagName()
					print('Processing element ' + tagName)
					
					# If true, the property or comma tag refers to a parent object,
					# so no need to dig deeper
					if not isinstance(element, Tag) and tagName not in tagObject:
						print('Property or comma belongs to parent object')
						output += element.render(tagObject, parentTree)
						continue
					
					# Append the object to the parent tree. Include subtags, so
					# that the comma tag can check if it's the last instance.
					# Even though we only need object's properties and subtags,
					# this will copy the whole tree. Kind of inefficient, but
					# at least we get the information we need.
					parentTree = parentTree[:] if parentTree else []
					parentTree.append(tagObject)
					
					# Same test, and now we know the tagName is in tagObject
					if not isinstance(element, Tag):
						output += element.render(tagObject[tagName], parentTree)
					else:
						# Render Tag objects in serial
						for tag in tagObject[tagName]:
							output += element.render(tag, parentTree)
				else:
					print('Adding string of length %d to output' % len(element))
					if len(element) <= 78:
						print('[' + element + ']')
					output += element
		except:
			print("Exception occurred, returning ''")
			output = ''
		# All done
		return output


