#ifndef I2C1_H_
#define I2C1_H_

#include "ht32.h"
#include "ht32_board.h"
#include "ht32_board_config.h"

#define SIGNAL_PATH_RESET 0x68
#define I2C_SLV0_ADDR 0x37
#define ACCEL_CONFIG 0x1C
#define MOT_THR 0x1F // Motion detection threshold bits [7:0]
#define MOT_DUR 0x20 // Duration counter threshold for motion interrupt generation, 1 kHz rate, LSB = 1 ms
#define MOT_DETECT_CTRL 0x69
#define INT_ENABLE 0x38
#define WHO_AM_I_MPU6050 0x75 // Should return 0x68
#define INT_STATUS 0x3A
//when nothing connected to AD0 than address is 0x68
#define ADO 0
#if ADO
#define MPU6050_ADDRESS 0x69 // Device address when ADO = 1
#else
#define MPU6050_ADDRESS 0x68 // Device address when ADO = 0
#endif

#define I2C_MASTER_ADDRESS     0x0A
#define I2C_SLAVE_ADDRESS      0x68 
#define BufferSize             2
#define ClockSpeed             1000000

extern u8 I2C_Master_Buffer_Tx[BufferSize];
extern u8 I2C_Master_Buffer_Rx[BufferSize];
extern u8 Tx_Index;
extern vu8 Rx_Index;
void I2C_Configuration(void);
void I2C_Master_Transmitter( u32 size);
void I2C_Master_Receiver( u32 size);
#endif
