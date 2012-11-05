avr_controller
==============

## Enable GPIO access
On the first build, sudo will ask for your password. This is to create a small helper program that can export GPIO pins. If this program is not created, run `build.sh` in the gpio_export directory.

## Enable I2C access
Add your username to the group i2c: `sudo usermod -a -G i2c <username>`
