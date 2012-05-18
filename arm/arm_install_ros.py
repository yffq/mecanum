#!/usr/bin/env python

print('Hello World')
distro='fuerte'
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
# pip (because easy_install is no longer maintained and much less powerful
# and user-friendly than pip.
## python-pip

# Install ROS utilities
#sudo pip install -U rosinstall rospkg rosdep
# TODO: Maybe this should be:
#sudo pip install -U rosinstall vcstools rospkg rosdep

# Install core libraries into /opt/ros/fuerte
# Do this only if /opt/ros doesn't exist
#rm -rf ~/ros-underlay (if it exists)
#rosinstall --catkin ~/ros-underlay http://ros.org/rosinstalls/fuerte-ros-base.rosinstall
#cd ~/ros-underlay
#mkdir build
#cd build
#cmake .. -DCMAKE_INSTALL_PREFIX=/opt/ros/fuerte
#make (-j8)
#sudo make install
#cd ..
#cd ..
#rm -rf ~/ros-underlay

# Higher-level robotics libraries and tools
# See REP 113 (http://www.ros.org/reps/rep-0113.html) for variants
# Use the 'mobile' capability variant extending the base 'robot' variant
# This gives us the following stacks: ros, ros_comm, bond_core, common_msgs,
# common, diagnostics, driver_common, eigen, filters, bullet, geometry,
# nodelet_core, orocos_kinematics_dynamics, pluginlib, assimp, robot_model,
# executive_smach, xacro, navigation, slam_gmapping, laser_pipeline,
# perception_pcl
variant='mobile'
distro='fuerte'
target='~/ros'
#'rosinstall ' + target + ' "http://packages.ros.org/cgi-bin/gen_rosinstall.py?"' +
#	'rosdistro=' + distro + '&variant=' + variant + '&overlay=no"'

# Environment setup
#'echo "source ' + target + '/setup.bash" > ~/.bashrc'

# Build higher-level libraries and tools
#sudo rosdep init
#rosdep update ### This might need sudo
# Install system dependencies
#rosdep install -a
# Build the ROS stacks using rosmake
#rosmake -a
