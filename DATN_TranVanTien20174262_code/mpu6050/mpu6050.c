
#include "mpu6050.h"

void CKCU_Configuration(void)
{
  CKCU_PeripClockConfig_TypeDef CKCUClock = {{0}};

  HTCFG_INPUT_WAKE_CLK(CKCUClock)  = 1;
  HTCFG_INPUT_KEY1_CLK(CKCUClock)  = 1;
  HTCFG_INPUT_KEY2_CLK(CKCUClock)  = 1;
  CKCUClock.Bit.AFIO       = 1;
  CKCU_PeripClockConfig(CKCUClock, ENABLE);
}
/*********************************************************************************************************//**
  * @brief  Configure the GPIO ports for input.
  * @retval None
  ***********************************************************************************************************/
void GPIO_IN_Configuration(void)
{
  /* Configure WAKEUP, KEY1, KEY2 pins as the input function                                                */
  /* Configure AFIO mode of input pins                                                                      */
  AFIO_GPxConfig(HTCFG_INPUT_WAKE_ID, HTCFG_INPUT_WAKE_AFIO_PIN, AFIO_FUN_GPIO);
  AFIO_GPxConfig(HTCFG_INPUT_KEY1_ID, HTCFG_INPUT_KEY1_AFIO_PIN, AFIO_FUN_GPIO);
  AFIO_GPxConfig(HTCFG_INPUT_KEY2_ID, HTCFG_INPUT_KEY2_AFIO_PIN, AFIO_FUN_GPIO);

  /* Configure GPIO direction of input pins                                                                 */
  GPIO_DirectionConfig(HTCFG_WAKE, HTCFG_INPUT_WAKE_GPIO_PIN, GPIO_DIR_IN);
  GPIO_DirectionConfig(HTCFG_KEY1, HTCFG_INPUT_KEY1_GPIO_PIN, GPIO_DIR_IN);
  GPIO_DirectionConfig(HTCFG_KEY2, HTCFG_INPUT_KEY2_GPIO_PIN, GPIO_DIR_IN);

  /* Configure GPIO pull resistor of input pins                                                             */
  GPIO_PullResistorConfig(HTCFG_WAKE, HTCFG_INPUT_WAKE_GPIO_PIN, GPIO_PR_DOWN);
  GPIO_PullResistorConfig(HTCFG_KEY1, HTCFG_INPUT_KEY1_GPIO_PIN, GPIO_PR_UP);
  GPIO_PullResistorConfig(HTCFG_KEY2, HTCFG_INPUT_KEY2_GPIO_PIN, GPIO_PR_UP);

  GPIO_InputConfig(HTCFG_WAKE, HTCFG_INPUT_WAKE_GPIO_PIN, ENABLE);
  GPIO_InputConfig(HTCFG_KEY1, HTCFG_INPUT_KEY1_GPIO_PIN, ENABLE);
  GPIO_InputConfig(HTCFG_KEY2, HTCFG_INPUT_KEY2_GPIO_PIN, ENABLE);
}


/*void SysTick_Handler(void)
{
  extern void TimingDelay(void);
  TimingDelay();
}
void Delay(u32 nTime)
{
  // Enable the SysTick Counter
  SYSTICK_CounterCmd(SYSTICK_COUNTER_CLEAR);
  SYSTICK_CounterCmd(SYSTICK_COUNTER_ENABLE);

  DelayTime = nTime;

  while(DelayTime != 0);

  // Disable SysTick Counter 
  SYSTICK_CounterCmd(SYSTICK_COUNTER_DISABLE);
  // Clear SysTick Counter 
  SYSTICK_CounterCmd(SYSTICK_COUNTER_CLEAR);
}

void TimingDelay(void)
{
  if(DelayTime != 0)
  {
    DelayTime--;
  }
}*/
void mpu6050_write(u8 subAddress, u8 data)
{
	I2C_Master_Buffer_Tx[0] = subAddress;
	I2C_Master_Buffer_Tx[1] = data;
  I2C_Master_Transmitter( 2);
	Time_Delay(TIME_MS2TICK(10));
	
}
void mpu6050_configINT(void)
{
	
	mpu6050_write(0x6B, 0x00);
  mpu6050_write(SIGNAL_PATH_RESET, 0x07); //Reset all internal signal paths in the MPU-6050 by writing 0x07 to register 0x68;
  mpu6050_write(I2C_SLV0_ADDR, 0x00); //write register 0x37 to select how to use the interrupt pin. For an active high, push-pull signal that stays until register (decimal) 58 is read, write 0x20.
  mpu6050_write(ACCEL_CONFIG, 0x01); //Write register 28 (==0x1C) to set the Digital High Pass Filter, bits 3:0. For example set it to 0x01 for 5Hz. (These 3 bits are grey in the data sheet, but they are used! Leaving them 0 means the filter always outputs 0.)
  mpu6050_write(MOT_THR, 1); //Write the desired Motion threshold to register 0x1F (For example, write decimal 20).  
  mpu6050_write(MOT_DUR, 2); //Set motion detect duration to 1  ms; LSB is 1 ms @ 1 kHz rate  
  mpu6050_write(MOT_DETECT_CTRL, 0x15); //to register 0x69, write the motion detection decrement and a few other settings (for example write 0x15 to set both free-fall and motion decrements to 1 and accelerometer start-up delay to 5ms total by adding 1ms. )   
  mpu6050_write(INT_ENABLE, 0x40); //write register 0x38, bit 6 (0x40), to enable motion detection interrupt.     
	//mpu6050_write(I2C_SLV0_ADDR, 160);
	Time_Delay(TIME_MS2TICK(180));
}
