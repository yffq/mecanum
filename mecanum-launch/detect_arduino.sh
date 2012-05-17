#!/bin/sh
ls -l /dev/serial/by-id 2> /dev/null | grep -i "arduino"

# output looks like:
# lrwxrwxrwx 1 root root 13 2012-03-03 20:49 usb-Arduino__www.arduino.cc__0042_64938333932351303182-if00 -> ../../ttyACM0
