#!/bin/sh
if [ -z "$1" ]
then
  echo "Incorrect usage. Call this script with a gpio pin number"
  exit
fi
chmod a+w /sys/class/gpio/export
chmod a+w /sys/class/gpio/unexport
echo $1 > /sys/class/gpio/export
chmod a+w /sys/class/gpio/gpio$1/value \
               /sys/class/gpio/gpio$1/direction \
               /sys/class/gpio/gpio$1/edge
