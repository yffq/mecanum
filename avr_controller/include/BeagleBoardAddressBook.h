#ifndef BEAGLEBOARDADDRESSBOOK_H
#define BEAGLEBOARDADDRESSBOOK_H

// GPIO pins
#define EXPANSION3       139 // BUTTON_GREEN
#define EXPANSION4       144
#define EXPANSION5       138 // BUTTON_YELLOW
#define EXPANSION6       146 // THUMBWHEEL2
#define EXPANSION7       137 // THUMBWHEEL4
#define EXPANSION8       143 // BUTTON_RED
#define EXPANSION9       136 // ARDUINO_BRIDGE5
#define EXPANSION10      145 // ARDUINO_BRIDGE2
#define EXPANSION11      135
#define EXPANSION12      158 // ARDUINO_BRIDGE6
#define EXPANSION13      134
#define EXPANSION14      162 // ARDUINO_BRIDGE1
#define EXPANSION15      133
#define EXPANSION16      161 // ARDUINO_BRIDGE3
#define EXPANSION17      132
#define EXPANSION18      159 // ARDUINO_BRIDGE4
#define EXPANSION19      131 // THUMBWHEEL1
#define EXPANSION20      156 // ARDUINO_RESSET
#define EXPANSION21      130 // IMU_INT0
#define EXPANSION22      157 // IMU_INT1
#define EXPANSION23      183 // IMU_SDA
#define EXPANSION24      168 // IMU_SCL
#define LED_USR0         149
#define LED_USR1         150
#define BUTTON_USR       7

// Arduino bridge
#define ARDUINO_BRIDGE1  EXPANSION14
#define ARDUINO_BRIDGE2  EXPANSION10
#define ARDUINO_BRIDGE3  EXPANSION16
#define ARDUINO_BRIDGE4  EXPANSION18
#define ARDUINO_BRIDGE5  EXPANSION9
#define ARDUINO_BRIDGE6  EXPANSION12
#define ARDUINO_RESSET   EXPANSION20

// Buttons
#define BUTTON_GREEN     EXPANSION3
#define BUTTON_YELLOW    EXPANSION5
#define BUTTON_RED       EXPANSION8
#define THUMBWHEEL1      EXPANSION19
#define THUMBWHEEL2      EXPANSION6
#define THUMBWHEEL4      EXPANSION7

// IMU interrupts
#define IMU_INT0         EXPANSION21
#define IMU_INT1         EXPANSION22
//#define IMU_SDA          EXPANSION23
//#define IMU_SCL          EXPANSION24

#endif // BEAGLEBOARDADDRESSBOOK_H
