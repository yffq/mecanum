#!/bin/bash

# Install ROS Fuerte
sudo sh -c 'echo "deb http://packages.ros.org/ros/ubuntu precise main" > /etc/apt/sources.list.d/ros-latest.list'
wget http://packages.ros.org/ros.key -O - | sudo apt-key add -
sudo apt-get update
sudo apt-get install ros-fuerte-ros-comm
if [ -f ~/ros_setup.sh ]
then

	cat > ~/ros_setup.sh <<EOF
#!/bin/sh
source /opt/ros/electric/setup.bash
export ROS_ROOT=/opt/ros/electric/ros
export PATH=$ROS_ROOT/bin:$PATH
export PYTHONPATH=$ROS_ROOT/core/roslib/src:$PYTHONPATH
export ROS_PACKAGE_PATH=~/ros_workspace:/opt/ros/electric/stacks:$ROS_PACKAGE_PATH
EOF

echo "export ROS_PACKAGE_PATH=~/ros_workspace:$ROS_PACKAGE_PATH" >> ~/.bashrc
echo "export ROS_WORKSPACE=~/ros_workspace" >> ~/.bashrc

fi

echo "source /opt/ros/fuerte/setup.bash" >> ~/.bashrc

# Install rosdep
sudo apt-get install easy_install
sudo easy_install -U rosdep rospkg
sudo rosdep init
rosdep update
arduino_core_found = $(rosdep db | grep "arduino-core")
if [ $? -eq 1]; then
	echo "yaml https://github.com/garbear/rosdistro/raw/master/rosdep/base.yaml" > /etc/ros/rosdep/sources.list.d/666-mecanum.list
	rosdep update
fi

# Install rosinstall
#sudo easy_install -U rosinstall vcstools

sudo sh -c 'echo "deb http://packages.ros.org/ros/ubuntu precise main" > /etc/apt/sources.list.d/ros-latest.list'


