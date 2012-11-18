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
		if self.propertyName in tagObject:
			print(self.tagName + '.' + self.propertyName + ': Rendering Property normally')
			return tagObject[self.propertyName]
		else:
			print(self.tagName + '.' + self.propertyName + ': property name not found in:')
			pprint.PrettyPrinter(indent=1).pprint(tagObject)
		raise Exception()


class Comma:
	def __init__(self, parentTagName):
		self.tagName = parentTagName.lower()
		
	def getTagName(self):
		return self.tagName
	
	def render(self, tagObject, parentTree):
		print('Rendering Comma: ' + self.tagName)
		# Get the tag object that this comma tag refers to
		# We only consider parentTree and ignore tagObject, because the comma
		# inclusion decision only concerns the parent tag
		
		#print('tagObject is:')
		#pprint.PrettyPrinter(indent=1).pprint(tagObject)
		
		if parentTree is not None and len(parentTree) > 0:
			print(self.tagName + ': parentTree is not None nor []')
			#pprint.PrettyPrinter(indent=1).pprint(parentTree)
			topTag = parentTree.pop()
			if self.tagName in tag:
				# Found our tag. Match tagObject until we hit the end
				i = 0
				for i in range(len(tag[self.tagName])):
					if tagObject == tag[self.tagName][i]:
						print(self.tagName + ': Returning [' + (',' if i != len(tag[self.tagName]) else '') + ']')
						return ',' if i != len(tag[self.tagName]) else ''
			else:
				print(self.tagName + ': tagName not in tag')
		else:
			print('parentTree is None or []')
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
		
		parentTree = parentTree[:] if parentTree else []
		
		output = ''
		# If an element in the tag fails, skip the whole tag (but note, not its parent)
		try:
			for element in self.elements:
				if isinstance(element, Tag):
					tagName = element.getTagName()
					print(self.tagName + ': visiting Tag subtag ' + tagName)
					if tagName in tagObject:
						parentTree.append(tagObject)
						# Render tagItems, one after the other
						for subTagObject in tagObject[tagName]:
							output += element.render(subTagObject, parentTree)
					else:
						# If the object wasn't supplied, skip it and move on
						print('JK!')
						output += ''
				elif isinstance(element, Property):
					print(self.tagName + ': visiting Property subtag ' + element.getTagName())
					# Properties always belong to tags (sometimes super-tags), so
					# only render proper tags. Otherwise, render a big error message.
					
					tagName = element.getTagName()
					if tagName == self.tagName:
						output += element.render(tagObject, parentTree)
					else:
						while tagName != tagObject.getName() and len(parentTree) > 0:
							tagObject = parentTree.pop()
						if tagName == tagObject.getName():
							output += element.render(tagObject, parentTree)
						else:
							output += '<%TOO CLOSE FOR MISSILES, SWITCHING TO GUNS%>'
				elif isinstance(element, Comma):
					print(self.tagName + ': visiting Comma subtag ' + element.getTagName())
					output += element.render(tagObject, parentTree)
				else: # isinstance(element, str)
					output += element
		except:
			print(self.tagName + ": Exception occurred, returning ''")
			output = ''
		# All done
		return output


