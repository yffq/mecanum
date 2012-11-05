avr_controller
==============

## Enable GPIO access
A helper program, run as root, is used to export GPIO pins. CMake tries to set permissions before building the program, so you will need to run `make` twice to get an executable with the right permissions. On the second run, sudo will ask for your password (this is how you know the executable file is configured correctly). If there is a problem, you can try running `gpio_export_rights.sh` directly.

## Enable I2C access
Add your username to the group i2c: `sudo usermod -a -G i2c <username>`
