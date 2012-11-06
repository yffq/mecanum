// ADXL345 Register Map
#define	ADXL345_DEVID             0x00   // Device ID Register
#define ADXL345_THRESH_TAP        0x1D   // Tap Threshold
#define ADXL345_OFSX              0x1E   // X-axis offset
#define ADXL345_OFSY              0x1F   // Y-axis offset
#define ADXL345_OFSZ              0x20   // Z-axis offset
#define ADXL345_DUR               0x21   // Tap Duration
#define ADXL345_Latent            0x22   // Tap latency
#define ADXL345_Window            0x23   // Tap window
#define ADXL345_THRESH_ACT        0x24   // Activity Threshold
#define ADXL345_THRESH_INACT      0x25   // Inactivity Threshold
#define ADXL345_TIME_INACT        0x26   // Inactivity Time
#define ADXL345_ACT_INACT_CTL     0x27   // Axis enable control for activity and inactivity detection
#define ADXL345_THRESH_FF         0x28   // Free-fall threshold
#define ADXL345_TIME_FF           0x29   // Free-fall time
#define ADXL345_TAP_AXES          0x2A   // Axis control for tap/double tap
#define ADXL345_ACT_TAP_STATUS    0x2B   // Source of tap/double tap
#define ADXL345_BW_RATE           0x2C   // Data rate and power mode control
#define ADXL345_POWER_CTL         0x2D   // Power Control Register
#define ADXL345_INT_ENABLE        0x2E   // Interrupt Enable Control
#define ADXL345_INT_MAP           0x2F   // Interrupt Mapping Control
#define ADXL345_INT_SOURCE        0x30   // Source of interrupts
#define ADXL345_DATA_FORMAT       0x31   // Data format control
#define ADXL345_DATAX0            0x32   // X-Axis Data 0
#define ADXL345_DATAX1            0x33   // X-Axis Data 1
#define ADXL345_DATAY0            0x34   // Y-Axis Data 0
#define ADXL345_DATAY1            0x35   // Y-Axis Data 1
#define ADXL345_DATAZ0            0x36   // Z-Axis Data 0
#define ADXL345_DATAZ1            0x37   // Z-Axis Data 1
#define ADXL345_FIFO_CTL          0x38   // FIFO control
#define ADXL345_FIFO_STATUS       0x39   // FIFO status

// Data rate control bits
#define ADXL345_DATA_RATE_6_25_HZ 0b0110
#define ADXL345_DATA_RATE_12_5_HZ 0b0111
#define ADXL345_DATA_RATE_25_HZ   0b1000
#define ADXL345_DATA_RATE_50_HZ   0b1001
#define ADXL345_DATA_RATE_100_HZ  0b1010
#define ADXL345_DATA_RATE_200_HZ  0b1011
#define ADXL345_DATA_RATE_400_HZ  0b1100
#define ADXL345_DATA_RATE_800_HZ  0b1101
#define ADXL345_DATA_RATE_1600_HZ 0b1110
#define ADXL345_DATA_RATE_3200_HZ 0b1111

// Power Control Register Bits
#define ADXL345_WU_0              (1<<0)   // Wake Up Mode - Bit 0
#define ADXL345_WU_1              (1<<1)   // Wake Up mode - Bit 1
#define ADXL345_SLEEP             (1<<2)   // Sleep Mode
#define ADXL345_MEASURE           (1<<3)   // Measurement Mode
#define ADXL345_AUTO_SLP          (1<<4)   // Auto Sleep Mode bit
#define ADXL345_LINK              (1<<5)   // Link bit

// Interrupt Enable/Interrupt Map/Interrupt Source Register Bits
#define ADXL345_OVERRUN           (1<<0)
#define ADXL345_WATERMARK         (1<<1)
#define ADXL345_FREE_FALL         (1<<2)
#define ADXL345_INACTIVITY        (1<<3)
#define ADXL345_ACTIVITY          (1<<4)
#define ADXL345_DOUBLE_TAP        (1<<5)
#define ADXL345_SINGLE_TAP        (1<<6)
#define ADXL345_DATA_READY        (1<<7)

// Interrupt masks, used in interrupt map INT_MAP
#define ADXL345_INTERRUPT1        0x00
#define ADXL345_INTERRUPT2        0xFF

// Data Format Bits
#define ADXL345_RANGE_2G          (0<<0)
#define ADXL345_RANGE_4G          (1<<0)
#define ADXL345_RANGE_8G          (1<<1)
#define ADXL345_RANGE_16G         (1<<0)|(1<<1)
#define ADXL345_JUSTIFY           (1<<2)
#define ADXL345_FULL_RES          (1<<3)

#define ADXL345_INT_INVERT        (1<<5)
#define ADXL345_SPI               (1<<6)
#define ADXL345_SELF_TEST         (1<<7)

// FIFO Control Bits (ADXL345_FIFO_CTL)
#define ADXL345_BYPASS            (0<<6)
#define ADXL345_FIFO              (1<<6)
#define ADXL345_STREAM            (1<<7)
#define ADXL345_TRIGGER           (1<<6)|(1<<7)
