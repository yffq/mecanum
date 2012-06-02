#!/usr/bin/env python

import sys
import serial
import time

print('Opening port')
arduino = serial.Serial('/dev/ttyACM0', 115200, timeout=1, dsrdtr=False)
time.sleep(3)
print('Flushing the buffer')
arduino.readline()

def a2s(arr):
    """ Array of integer byte values --> binary string
    """
    return ''.join(chr(b) for b in arr)

def getVoltage():
	''' Ask the arduino for a voltage reading.
	'''
	arduino.write(a2s([3,   # Message size
	                   8,   # FSM_ANALOGPUBLISHER
	                   11])) # BATTERY_VOLTAGE
	msgSize  = ord(arduino.read())
	fsmId    = ord(arduino.read())
	pin      = ord(arduino.read())
	highByte = ord(arduino.read())
	lowByte  = ord(arduino.read())
	R1 = 16.05 # kOhm
	R2 = 9.87  # kOhm
	offset = 1.0
	voltage = ((highByte << 8) + lowByte) * 5.0 / 1024 * (R1 + R2) / R2 * offset
	return voltage

def main():
	if len(sys.argv) > 1:
		version = sys.argv[1]
	else:
		version = str(0)
	file = open('voltages_' + version + '.csv', 'w')
	file.write('Seconds,Volts\n')
	start = time.time()
	
	for i in range(10):
		voltage = str(getVoltage())
		seconds = int(time.time() - start)
		file.write(str(seconds) + ',' + voltage + '\n')
		if seconds >= 60 * 60:
			# Hours
			sys.stdout.write(str(seconds / (60 * 60)) + ':')
			seconds = seconds % (60 * 60)
		if seconds >= 60:
			# Minutes
			sys.stdout.write(str(seconds / 60) + ':')
			seconds = seconds % 60
		# Seconds
		sys.stdout.write(str(seconds))
		print(' - Voltage: ' + voltage)
		time.sleep(30)
	
	file.close()
	return

if __name__ == "__main__":
    main()
