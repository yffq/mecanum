arm
===

## Create SD Card
This package contains scripts to create an ARM firmware image. Once the image is installed on the ARM board, the scripts can be used to install ROS dependencies and set up the ROS environment.

To alter the image's configuration, modify the values in settings.xml. Then, the image can be built using the following commands in the scripts directory:
```
./buildBootloader.py
./buildKernel.py
./buildQemu.py
./buildImage.py
./createCard.py
```

## Compile ROS from source
Once the image is installed, the ROS environment can be created:
```
./installRosMobile.py
```

## Enable file sharing for easier development
This process worked for me, but should probably be verified on a vanilla install.

Install samba (`sudo apt-get install samba`) if not already installed. Edit `/etc/samba/smb.conf`: Uncomment `workgroup = WORKGROUP` and `security = user`. Add the following to the end of the file:
```
[ros]
    comment = ROS Workspace
    path = /home/garrett/ros
    browsable = yes
    guest ok = no
    read only = no
    create mask = 0644
```

Create a samba password for the account: `sudo smbpasswd -a garrett`. Finally, add the mapping to `/etc/samba/smbusers`:
```
garrett="garrett"
```
