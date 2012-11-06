// ITG3200 Register Map
#define ITG3200_WHO_AM_I          0x00 // Who Am I
#define ITG3200_SMPLRT_DIV        0x15 // Sample Rate Divider
#define ITG3200_DLPF_FS           0x16 // DLPF, Full Scale
#define ITG3200_INT_CFG           0x17 // Interrupt Configuration
#define ITG3200_INT_STATUS        0x1A // Interrupt Status
#define ITG3200_TEMP_OUT_H        0x1B // Temperature Data High
#define ITG3200_TEMP_OUT_L        0x1C // Temperature Data Low
#define ITG3200_GYRO_XOUT_H       0x1D // Gyro-X Data High
#define ITG3200_GYRO_XOUT_L       0x1E // Gyro-X Data Low
#define ITG3200_GYRO_YOUT_H       0x1F // Gyro-Y Data High
#define ITG3200_GYRO_YOUT_L       0x20 // Gyro-Y Data Low
#define ITG3200_GYRO_ZOUT_H       0x21 // Gyro-Z Data High
#define ITG3200_GYRO_ZOUT_L       0x22 // Gyro-Z Data Low
#define ITG3200_PWR_MGM           0x3E // Power Management

// DLPF, Full Scale Register Bits
// FS_SEL must be set to 3 for proper operation
// Set DLPF_CFG to 3 for 1kHz Fint and 42 Hz Low Pass Filter
#define ITG3200_DLPF_CFG_0        (1<<0)
#define ITG3200_DLPF_CFG_1        (1<<1)
#define ITG3200_DLPF_CFG_2        (1<<2)
#define ITG3200_DLPF_FS_SEL_0     (1<<3)
#define ITG3200_DLPF_FS_SEL_1     (1<<4)

// Power Management Register Bits
// Recommended to set CLK_SEL to 1,2 or 3 at startup for more stable clock
#define ITG3200_PWR_MGM_CLK_SEL_0 (1<<0)
#define ITG3200_PWR_MGM_CLK_SEL_1 (1<<1)
#define ITG3200_PWR_MGM_CLK_SEL_2 (1<<2)
#define ITG3200_PWR_MGM_STBY_Z    (1<<3)
#define ITG3200_PWR_MGM_STBY_Y    (1<<4)
#define ITG3200_PWR_MGM_STBY_X    (1<<5)
#define ITG3200_PWR_MGM_SLEEP     (1<<6)
#define ITG3200_PWR_MGM_H_RESET   (1<<7)

// Interrupt Configuration Bits
#define ITG3200_INT_CFG_ACTL         (1<<7)
#define ITG3200_INT_CFG_OPEN         (1<<6)
#define ITG3200_INT_CFG_LATCH_INT_EN (1<<5)
#define ITG3200_INT_CFG_INT_ANYRD    (1<<4)
#define ITG3200_INT_CFG_ITG_RDY_EN   (1<<2)
#define ITG3200_INT_CFG_RAW_RDY_EN   (1<<0)
