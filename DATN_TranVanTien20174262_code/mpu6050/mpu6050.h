#ifndef MPU6050_H_
#define MPU6050_H_

#include "ht32.h"
#include "ht32_board.h"
#include "ht32_board_config.h"
#include "i2c.h"
#include "ht32_time.h"

extern u32 size;
extern vu32 DelayTime;

void CKCU_Configuration(void);
void GPIO_IN_Configuration(void);
void mpu6050_configINT(void);
/*void Delay(u32 nTime);
void TimingDelay(void);*/
#endif
