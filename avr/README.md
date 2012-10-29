avr
===

## Building and uploading AVR firmware
The commands for building and uploading firmware images are straight-forward CMake:

```
mkdir build
cd build
cmake ..
make
make mecanum-upload # Assuming mecanum is used in generate_arduino_firmware()
```
