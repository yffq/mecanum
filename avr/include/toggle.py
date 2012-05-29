#!/usr/bin/env python

import serial
import array
import time

def main():
	print('Opening port')
	arduino = serial.Serial('/dev/ttyACM0', 115200, dsrdtr = False)
	time.sleep(4)
	
	print('Writing null character')
	arduino.write('\x00')
	time.sleep(4)
	
	print('El fin')
	
	#size = 2 # not counting this byte
	#fsm = 6 # FSM_TOGGLE
	#pin = 48 # BATTERY_EMPTY
	#data = array.array('B', [size, fsm, pin])
	#print(data.tostring()) # '\x02\x06\x30'
	#arduino.write(data)
	
	#data = array.array('B', [0]).tostring()
	#arduino.write(data)
	#while(True):
	#	pass

if __name__ == "__main__":
    main()
