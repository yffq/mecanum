

class Attribute:
	def __init__(self, name):
		self.object = name.split('.')[0]
		self.property = name.split('.')[1]
	
	def render(object):
		return self.object + '.' + self.property # TODO


class Object:
	def __init__(self, name, text):
		self.object = name
		self.text = text
		# TODO
	
	def render(object):
		return '<%' + self.object + '%>' # TODO


class Tag:
	def __init__(self, text, name='ROOT'):
		self.elements = []
		
		while len(text):
			splitText = text.split('<%', 1)
			self.elements.append(splitText[0])
			if len(splitText) == 1
				break # No more text to parse
			
			# Match the tag name
			text = splitText[1]
			result = re.match('[A-Za-z0-9_.,-]+', text)
			if not result:
				# No tag name, ignore the result
				self.elements.append('<%')
				continue
			
			tagName = result.group()
			
			# The tag name is pushed onto the stack, so truncate text here
			text = text[len(tagName):]
			
			# Handle the comments tag <%-- --%>
			if tagName[:2] == '--':
				splitText = text.split('--%>', 1)
				if len(splitText) > 1:
					text = splitText[1]
					# Ignore the newline directly after the end tag
					if text[0] == '\n':
						text = text[1:]
				else:
					# Otherwise, ignore the tag and append the skipped text
					self.elements.append('<%' + tagName)
				continue
			
			# Handle the comma tag <%,%>
			if tagName == ',':
				# Ignore all text until the end tag
				splitText = text.split('%>', 1)
				if len(splitText) > 1:
					text = splitText[1]
				else:
					self.elements.append(Comma())
					#self.elements.append(Comma(self))
				continue
			
			# TODO: properties tag
			if '.' in tagName:
				self.elements.append('Property')
				continue
			
			# Extract the sub-tag and remove it from the beginning of text
			stack = [tagName]
			i = 0
			while len(stack) > 0 and i < len(text):
				if text[i : i + 2] == '%>':
					i += 2
					stack.pop()
				elif text[i : i + 2] == '<%':
					i += 2
					result = re.match('[A-Za-z0-9_.,-]+', text[i:])
					if result:
						foundTagName = result.group()
						# Don't push <%-- onto the stack
						if foundTagName[:2] == '--':
							# Skip the comment by incrementing i the length 
							# of the comment. Unlike above, we ignore the
							# comment even if its terminator isn't found. Oh well.
							i += len(text[i:].split('--%>', 1)[0]) + len('--%>')
						else:
							stack.append(foundTagName)
							i += len(foundTagName)
				else:
					i += 1
			
			# If stack was depleted, we found a sub-tag
			if len(stack) == 0:
				self.elements.append(SubTag(text[:i], tagName))
				text = text[i:]
			else:
				self.elements.append(text)
				break


