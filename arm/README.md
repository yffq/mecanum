arm
===

This package contains scripts to create an ARM firmware image. Once the image is installed on the ARM board, the scripts can be used to install ROS dependencies and set up the ROS environment.

To alter the image's configuration, modify the values in settings.xml. Then, the image can be built using the following commands in the scripts directory:

./buildBootloader.py
./buildKernel.py
./buildQemu.py
./buildImage.py
./createCard.py

Once the image is installed, the ROS environment can be created:

./installRosMobile.py
