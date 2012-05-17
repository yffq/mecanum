#!/usr/bin/env python

print('Hello World')
# Enable restricted repositories by adding " restricted" to the end of the
# four lines in /etc/apt/sources.list:
# deb http://... precise main universe multiverse
# deb-src http://... precise main universe multiverse
# deb http:// precise-updates main universe multiverse
# deb-src http://... precise-updates main universe multiverse

# Install ROS packages -- skip this step because there are no armhf packages
# sudo sh -c 'echo "deb http://packages.ros.org/ros/ubuntu precise main" > /etc/apt/sources.list.d/ros-latest.list'
# wget http://packages.ros.org/ros.key -O - | sudo apt-key add -

# sudo apt-get update

# Install dependencies
# Bootstrap:
## build-essential python-yaml cmake subversion python-setuptools mercurial
# Core library dependencies (aka, 'ROS Base'):
## python-yaml libapr1-dev libaprutil1-dev libbz2-dev python-dev libgtest-dev
## python-paramiko liblog4cxx10-dev pkg-config
# ROS wants libboost1.40-all-dev, but Precise only offers 1.46 and 1.48
## libboost1.46-all-dev -or- libboost1.48-all-dev -or- libboost-all-dev
# 'ROS Full' -- don't install these (ros-fuerte-swig-wx isn't available anyways)
## python-wxgtk2.8 python-gtk2 python-matplotlib libwxgtk2.8-dev python-imaging
## libqt4-dev graphviz qt4-qmake python-numpy ros-fuerte-swig-wx
