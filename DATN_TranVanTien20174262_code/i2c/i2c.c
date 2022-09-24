
#include "i2c.h"

/*********************************************************************************************************//**
  * @brief  Configure the I2C.
  * @retval None
  ***********************************************************************************************************/

void I2C_Configuration(void)
{
  { /* Enable peripheral clock                                                                              */
    CKCU_PeripClockConfig_TypeDef CKCUClock = {{0}};
    HTCFG_I2C_MASTER_CLK(CKCUClock) = 1;
    CKCUClock.Bit.AFIO              = 1;
    CKCU_PeripClockConfig(CKCUClock, ENABLE);
  }

  /* Configure GPIO to I2C mode for Master                                                                  */
  AFIO_GPxConfig(HTCFG_I2C_MASTER_SCL_GPIO_ID, HTCFG_I2C_MASTER_SCL_AFIO_PIN, AFIO_FUN_I2C);
  AFIO_GPxConfig(HTCFG_I2C_MASTER_SDA_GPIO_ID, HTCFG_I2C_MASTER_SDA_AFIO_PIN, AFIO_FUN_I2C);
	

  { /* I2C Master configuration                                                                             */

    /* !!! NOTICE !!!
       Notice that the local variable (structure) did not have an initial value.
       Please confirm that there are no missing members in the parameter settings below in this function.
    */
    I2C_InitTypeDef  I2C_InitStructure;

    I2C_InitStructure.I2C_GeneralCall = DISABLE;
    I2C_InitStructure.I2C_AddressingMode = I2C_ADDRESSING_7BIT;
    I2C_InitStructure.I2C_Acknowledge = DISABLE;
    I2C_InitStructure.I2C_OwnAddress = I2C_MASTER_ADDRESS;
    I2C_InitStructure.I2C_Speed = ClockSpeed;
    I2C_InitStructure.I2C_SpeedOffset = 0;
    I2C_Init(HTCFG_I2C_MASTER_PORT, &I2C_InitStructure);
  }

  /* Enable I2C                                                                                             */
  I2C_Cmd(HTCFG_I2C_MASTER_PORT, ENABLE);
}

/*********************************************************************************************************//**
  * @brief  I2C master transfers data to another device.
  * @retval None
  ***********************************************************************************************************/
void I2C_Master_Transmitter( u32 size)
{
	u8 Tx_Index = 0;
  /*--------------------------------------------------------------------------------------------------------*/
  /* Master Transmitter                                                                                     */
  /*--------------------------------------------------------------------------------------------------------*/
  /* Send I2C START & I2C slave address for write                                                           */
  I2C_TargetAddressConfig(HTCFG_I2C_MASTER_PORT, I2C_SLAVE_ADDRESS, I2C_MASTER_WRITE);

  /* Check on Master Transmitter STA condition and clear it                                                 */
  while (!I2C_CheckStatus(HTCFG_I2C_MASTER_PORT, I2C_MASTER_SEND_START));

  /* Check on Master Transmitter ADRS condition and clear it                                                */
  while (!I2C_CheckStatus(HTCFG_I2C_MASTER_PORT, I2C_MASTER_TRANSMITTER_MODE));
  /* Send data                                                                                              */
  while (Tx_Index < size)
  {
    /* Check on Master Transmitter TXDE condition                                                           */
    while (!I2C_CheckStatus(HTCFG_I2C_MASTER_PORT, I2C_MASTER_TX_EMPTY));
    /* Master Send I2C data                                                                                 */
    I2C_SendData(HTCFG_I2C_MASTER_PORT, I2C_Master_Buffer_Tx[Tx_Index ++]);
  }
  /* Send I2C STOP condition                                                                                */
  I2C_GenerateSTOP(HTCFG_I2C_MASTER_PORT);
  /*wait for BUSBUSY become idle                                                                            */
  while (I2C_ReadRegister(HTCFG_I2C_MASTER_PORT, I2C_REGISTER_SR)&0x80000);
}

/*********************************************************************************************************//**
  * @brief  I2C master receives data from another device.
  * @retval None
  ***********************************************************************************************************/
void I2C_Master_Receiver( u32 size)
{
  /*--------------------------------------------------------------------------------------------------------*/
  /* Master Receiver                                                                                        */
  /*--------------------------------------------------------------------------------------------------------*/
  /* Send I2C START & I2C slave address for read                                                            */
	u8 Rx_Index = 0;
  I2C_TargetAddressConfig(HTCFG_I2C_MASTER_PORT, I2C_SLAVE_ADDRESS, I2C_MASTER_READ);

  /* Check on Master Transmitter STA condition and clear it                                                 */
  while (!I2C_CheckStatus(HTCFG_I2C_MASTER_PORT, I2C_MASTER_SEND_START));

  /* Check on Master Transmitter ADRS condition and clear it                                                */
  while (!I2C_CheckStatus(HTCFG_I2C_MASTER_PORT, I2C_MASTER_RECEIVER_MODE));

  I2C_AckCmd(HTCFG_I2C_MASTER_PORT, ENABLE);
  /* Receive data                                                                                           */
  while (Rx_Index < size)
  {

    /* Check on Slave Receiver RXDNE condition                                                              */
    while (!I2C_CheckStatus(HTCFG_I2C_MASTER_PORT, I2C_MASTER_RX_NOT_EMPTY));
    /* Store received data on I2C1                                                                          */
    I2C_Master_Buffer_Rx[Rx_Index ++] = I2C_ReceiveData(HTCFG_I2C_MASTER_PORT);
    if (Rx_Index == (BufferSize-1))
    {
      I2C_AckCmd(HTCFG_I2C_MASTER_PORT, DISABLE);
    }
  }
  /* Send I2C STOP condition                                                                                */
  I2C_GenerateSTOP(HTCFG_I2C_MASTER_PORT);
  /*wait for BUSBUSY become idle                                                                            */
  while (I2C_ReadRegister(HTCFG_I2C_MASTER_PORT, I2C_REGISTER_SR)&0x80000);

}
