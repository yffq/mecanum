import re

class FSMResolver:
	def __init__(self, idpath):
		pass
	
	def resolve(self, fsmid):
		return {
			'blink': 'FSM_BLINK',
			'fade':  'FSM_FADE',
		}.get(fsmid.lower(), 'FSM_CHRISTMASTREE')

