/*********************************************************************************************************//**
 * @file    USART/Interrupt_FIFO/main.c
 * @version $Rev:: 5073         $
 * @date    $Date:: 2020-11-08 #$
 * @brief   Main program.
 *************************************************************************************************************
 * @attention
 *
 * Firmware Disclaimer Information
 *
 * 1. The customer hereby acknowledges and agrees that the program technical documentation, including the
 *    code, which is supplied by Holtek Semiconductor Inc., (hereinafter referred to as "HOLTEK") is the
 *    proprietary and confidential intellectual property of HOLTEK, and is protected by copyright law and
 *    other intellectual property laws.
 *
 * 2. The customer hereby acknowledges and agrees that the program technical documentation, including the
 *    code, is confidential information belonging to HOLTEK, and must not be disclosed to any third parties
 *    other than HOLTEK and the customer.
 *
 * 3. The program technical documentation, including the code, is provided "as is" and for customer reference
 *    only. After delivery by HOLTEK, the customer shall use the program technical documentation, including
 *    the code, at their own risk. HOLTEK disclaims any expressed, implied or statutory warranties, including
 *    the warranties of merchantability, satisfactory quality and fitness for a particular purpose.
 *
 * <h2><center>Copyright (C) Holtek Semiconductor Inc. All rights reserved</center></h2>
 ************************************************************************************************************/

/* Includes ------------------------------------------------------------------------------------------------*/
#include "ht32.h"
#include "ht32_board.h"
#include "ht32_board_config.h"
#include "AT_COMMAND.h"
#include "mqtt.h"
#include "GPS.h"
#include "i2c.h"
#include "mpu6050.h"
#include "string.h"
#include "stdlib.h"
/** @addtogroup HT32_Series_Peripheral_Examples HT32 Peripheral Examples
  * @{
  */

/** @addtogroup USART_Examples USART
  * @{
  */

/** @addtogroup Interrupt_FIFO
  * @{
  */
#define LOWPOWER_MODE_WFI                        (1)

/* Private function prototypes -----------------------------------------------------------------------------*/
void Config_GPIO(void);
void UxART_Configuration(void);
void UxART1_Configuration(void);
void RTC_Configuration(void);
void EXTI_Configuration(void);
void deepsleep(u32 deeptime);
void LowPower_Enter(void);
/* Global variables ----------------------------------------------------------------------------------------*/

//khai bao bien at
volatile SIMCOM_ResponseEvent_t AT_RX_event;
volatile Device_Flag D_Flag_t;
ATCommand_t SIMCOM_ATCommand;
uc8  *gURTx1_Ptr;
vu32 gURTx1_Length;
vu32 TimePeriod;
u8 charTimePeriod;
vu32 utick;
u32 count_NW;
st_device st_device_t;
int j;
//khai bao bien mqtt
volatile AT_FLAG AT_Flag_t;
volatile HT32_FLAG HT32_FLAG_T;
MQTTdataType_t MQTTdataType;
u8 TX_Trans[128];
bool Flag_CheckEnd_AT = false;
u32 count_restart;

//khai bao bien gps
GPS gps_t;
DT DateTime; 
uc8  *gURTx0_Ptr;
u32 gURTx0_Length = 0;
u8 Time[128];
u8 Day[128];
u8 latitude[50];
u8 longitude[50];
u8 sog[50];
char gps_rx[256];
DT DateTime; 
int count_filter, lim, start, finish ,i;

// khai bao i2c mpu6050

u8 I2C_Master_Buffer_Tx[BufferSize];
u8 I2C_Master_Buffer_Rx[BufferSize];
u8 Tx_Index;/* Private constants ---------------------------------------------------------------------------------------*/
vu8 Rx_Index;/* Private function prototypes -----------------------------------------------------------------------------*/
u32 size;
u32 spk_count = 0;
/* Private variables ---------------------------------------------------------------------------------------*/
uc8 gHelloString[] = "AT+CEREG?\r\n";
uc8 topic_pub[] = "messages/bbaadd3d-0a6d-43b3-85db-db46381d2914/attributets";
uc8 topic_sub[] = "messages/bbaadd3d-0a6d-43b3-85db-db46381d2914/ht32";
uc8 data[] = "helloPC";
u32 time1;
u8 count;
u8 count_reset;
u32 n;
FlagStatus TmpStatus = RESET;
bool CB_CD;
u8 count_mess=1;
/* Global functions ----------------------------------------------------------------------------------------*/
/*********************************************************************************************************//**
  * @brief  Main program.
  * @retval None
  ***********************************************************************************************************/

bool check_cd()
{
	int a = 0, b = 0;
	u32 time_detect = 0, time2 = 0, time3 = 0;
	u32 count_detect = 0;
	bool check = FALSE;
	while(!check)
	{
		if(a == 0)
		{
			time2 = utick;
			time3 = utick;
			a = 1;
		}
  		TmpStatus = GPIO_ReadInBit(HT_GPIOC, GPIO_PIN_10);
		if(TmpStatus)
		{
			while(TmpStatus)
			{
				TmpStatus = GPIO_ReadInBit(HT_GPIOC, GPIO_PIN_10);
			}
			count_detect++;
		}
		if(utick - time2 > 3000 && a == 1)
		{
			if(count_detect > 4)
			{
				gps_t.detect = count_detect;
				CB_CD = TRUE;
				check = TRUE;
				HT32F_DVB_LEDOff(HT_LED2);
			}
			else
			{
				break;
			}
		}
		if(utick - time3 >= 500)
		{
			HT32F_DVB_LEDToggle(HT_LED2);
			time3 = utick;
		}

	}
	return check;
} 
int main(void)
 {
  HT32F_DVB_LEDInit(HT_LED1);
  HT32F_DVB_LEDInit(HT_LED2);
	
	Config_GPIO();
	Time_Init();
  UxART_Configuration();
	UxART1_Configuration();
	RTC_Configuration();
	EXTI_Configuration();
	Configure_AT_Command();
	CKCU_Configuration();
	GPIO_IN_Configuration();
  I2C_Configuration();
	mpu6050_configINT();
	HT32F_DVB_LEDOn(HT_LED1);
	AT_TURNON:
	 // bat module gps
	 if(st_device_t.st_flag == true && !st_device_t.st_speaker )
	 {
		 	 if(check_cd())
			{
				count_mess = 2;
			}
			else
			{
				if(count_mess == 0)
				{
					LowPower_Enter();
					goto AT_TURNON;
				}

			}
	 }

	GPIO_SetOutBits(HT_GPIOA, GPIO_PIN_8); 
	
	// check AT command
	AT_SimCom:
	Simcom_ATC_SendATCommand("AT+CPOF\r\n", "OK", 1000, 0, NULL);
	Time_Delay(TIME_MS2TICK(3000));
	CB_CD = FALSE;
	GPIO_SetOutBits(HT_GPIOC, GPIO_PIN_13);
	Time_Delay(TIME_MS2TICK(500));	
	GPIO_ClearOutBits(HT_GPIOC, GPIO_PIN_13);
	HT32_FLAG_T.HT32_Reset = FALSE;
	while(!HT32_FLAG_T.HT32_Reset)
	{
		if(count_reset == 0)
		{
			time1 = utick;
			count_reset = 1;
		}
		if(utick - time1 > 25000 )
		{
			break;
		}
	}
	
	if(!HT32_FLAG_T.HT32_Reset)
	{
		HT32_FLAG_T.HT32_Reset = TRUE;
		count_reset = 0;
		Simcom_ATC_SendATCommand("AT+CPOF\r\n", "OK", 1000, 0, NULL);
		goto AT_SimCom;
	}
	
  Simcom_ATC_SendATCommand((const char *)gHelloString, "OK", 7000, 3, Simcom_ATResponse_Callback);
	AT_CHECK:
	AT_Flag_t.AT_FLAG_CHECK = Check_ATC_wait("MQTT check AT command");
	if(AT_Flag_t.AT_FLAG_CHECK != 0)
	{
		if(AT_Flag_t.AT_FLAG_CHECK != 1)
		{
			goto AT_CHECK;
		}
		else
		{
			count_restart++;
		}
	}
	else
	{
		count_restart = 0;
		if(count_NW == 1)
		{
			count_NW = 0;
		}
		else
		{
			Time_Delay(TIME_MS2TICK(7000));	
			Simcom_ATC_SendATCommand((const char *)gHelloString, "OK", 7000, 3, Simcom_ATResponse_Callback);
			goto AT_CHECK;
		}
	}
	if(count_restart == 1)
	{
		Simcom_ATC_SendATCommand("AT+CPOF\r\n", "OK", 1000, 0, NULL);
		Time_Delay(TIME_MS2TICK(200));
		count_restart = 0;
		goto AT_SimCom;
	}
	// connect mqtt
	
	if(MQTT_Connect_Start())
	{
		
		goto AT_SimCom;
	}
	Time_Delay(TIME_MS2TICK(100));
	MQTT_SubTopic((char *)topic_sub, 0);
	Time_Delay(TIME_MS2TICK(1000));
	if(st_device_t.st_flag == true)
	{
		TimePeriod = st_device_t.time_period;
	}
	else 
	{
		TimePeriod = 20;
	}
	count = 0;
	NVIC_EnableIRQ(HTCFG_UART0_IRQn);
	USART_RxCmd(HTCFG_UART0_PORT, ENABLE);
  while (1)
	{
		// transmit data to server
		if(gps_t.FLAG_CHECK == TRUE &&  count_mess > 0) // flag receive gps
		{
			count_mess--;
			gps_t.FLAG_CHECK = FALSE;
			NVIC_DisableIRQ(HTCFG_UART0_IRQn);
			USART_RxCmd(HTCFG_UART0_PORT, DISABLE);
			GPIO_ClearOutBits(HT_GPIOA, GPIO_PIN_8); // tat module gpss
			MQTT_PubDataTopic((char *)topic_pub, (char *)gps_rx, strlen((char *)gps_rx),0, 0 );
			Flag_CheckEnd_AT = true;
			Time_Delay(TIME_MS2TICK(100));	
		}
		Time_Delay(TIME_MS2TICK(100));	
		if(gps_t.FLAG_CHECK == FALSE )
		{
			if(count == 0)
			{
				time1 = utick;
				count = 1;
			}
			if(utick - time1 > 45000)
			{
				Flag_CheckEnd_AT = true;
			}
		}
		// disconnect va deepsleep chu ky TimePeriod 1 lan
		Time_Delay(TIME_MS2TICK(100));	
		if((Flag_CheckEnd_AT == true || st_device_t.st_flag == false) && !spk_count)
		{
			MQTT_Disconnect_Start();
			Flag_CheckEnd_AT =false;
			memset((char *)gps_t.GPS_RX_DATA, 0, sizeof(gps_t.GPS_RX_DATA)); // xoa buffer gps khi turn off, de phong du lieu chua truyen xong
			gps_t.GPS_RX_INDEX = 0;
			gps_t.detect = 0;
			HT32F_DVB_LEDOff(HT_LED1);
			Simcom_ATC_SendATCommand("AT+CPOF\r\n", "OK", 1000, 0, NULL);
			deepsleep(TimePeriod);	
			Time_Delay(TIME_MS2TICK(500));
			if(st_device_t.st_flag && !st_device_t.st_speaker )
			{
				LowPower_Enter();		
			}
			goto AT_TURNON;
		}
		if(st_device_t.st_speaker == true)
			{
				spk_count++;
				for(j = 0; j < 3; j++)
				{
					if(st_device_t.st_speaker == false)
					{
						break;
					}
					Time_Delay(TIME_MS2TICK(100));
					GPIO_SetOutBits(HT_GPIOD, GPIO_PIN_1);
					Time_Delay(TIME_MS2TICK(100));
					GPIO_ClearOutBits(HT_GPIOD, GPIO_PIN_1);

				}
				for(j = 0; j < 3; j++)
				{
					if(st_device_t.st_speaker == false)
					{
						break;
					}
					Time_Delay(TIME_MS2TICK(300));
					GPIO_SetOutBits(HT_GPIOD, GPIO_PIN_1);
					Time_Delay(TIME_MS2TICK(300));
					GPIO_ClearOutBits(HT_GPIOD, GPIO_PIN_1);
				}
				for(j = 0; j < 3; j++)
				{
					if(st_device_t.st_speaker == false)
					{
						break;
					}
					Time_Delay(TIME_MS2TICK(100));
					GPIO_SetOutBits(HT_GPIOD, GPIO_PIN_1);
					Time_Delay(TIME_MS2TICK(100));
					GPIO_ClearOutBits(HT_GPIOD, GPIO_PIN_1);
				}
				if(spk_count > 10 || st_device_t.st_speaker == false)
				{
					spk_count = 0;
				}
			}
		}
  }
void Config_GPIO(void){
	
	// Bat clock cho ngoai vi GPIO va day PA
	CKCU_PeripClockConfig_TypeDef CKCU = {{0}};
	CKCU.Bit.AFIO = 1;
	CKCU.Bit.PD = 1;
	CKCU.Bit.PB = 1;
	CKCU.Bit.PC = 1;
	CKCU_PeripClockConfig(CKCU, ENABLE);
	
	AFIO_GPxConfig(GPIO_PD, AFIO_PIN_8, AFIO_FUN_GPIO);
	AFIO_GPxConfig(GPIO_PA, AFIO_PIN_8, AFIO_FUN_GPIO);
	AFIO_GPxConfig(GPIO_PD, AFIO_PIN_1, AFIO_FUN_GPIO);
	AFIO_GPxConfig(GPIO_PB, AFIO_PIN_8, AFIO_FUN_GPIO);
	AFIO_GPxConfig(GPIO_PB, AFIO_PIN_7, AFIO_FUN_GPIO);
	AFIO_GPxConfig(GPIO_PC, AFIO_PIN_13, AFIO_FUN_GPIO);
	
	//day tro cho GPIO
	GPIO_PullResistorConfig(HT_GPIOD, GPIO_PIN_8, GPIO_PR_UP);
	GPIO_PullResistorConfig(HT_GPIOA, GPIO_PIN_8, GPIO_PR_UP);
	GPIO_PullResistorConfig(HT_GPIOD, GPIO_PIN_1, GPIO_PR_UP);
	GPIO_PullResistorConfig(HT_GPIOB, GPIO_PIN_8, GPIO_PR_UP);
	GPIO_PullResistorConfig(HT_GPIOB, GPIO_PIN_7, GPIO_PR_UP);
	GPIO_PullResistorConfig(HT_GPIOC, GPIO_PIN_13, GPIO_PR_UP);
	// config out/in
	
	GPIO_DirectionConfig(HT_GPIOD, GPIO_PIN_8, GPIO_DIR_OUT);
	GPIO_DirectionConfig(HT_GPIOA, GPIO_PIN_8, GPIO_DIR_OUT);
	GPIO_DirectionConfig(HT_GPIOD, GPIO_PIN_1, GPIO_DIR_OUT);
	GPIO_DirectionConfig(HT_GPIOB, GPIO_PIN_8, GPIO_DIR_OUT);
	GPIO_DirectionConfig(HT_GPIOB, GPIO_PIN_7, GPIO_DIR_OUT);
	GPIO_DirectionConfig(HT_GPIOC, GPIO_PIN_13, GPIO_DIR_OUT);
	// set default value
	GPIO_WriteOutBits(HT_GPIOD, GPIO_PIN_8, RESET);
	GPIO_WriteOutBits(HT_GPIOA, GPIO_PIN_8, RESET);
	GPIO_WriteOutBits(HT_GPIOD, GPIO_PIN_1, RESET);
	GPIO_WriteOutBits(HT_GPIOB, GPIO_PIN_8, RESET);
	GPIO_WriteOutBits(HT_GPIOB, GPIO_PIN_7, RESET);
	GPIO_WriteOutBits(HT_GPIOC, GPIO_PIN_13, RESET);
	
}
/*************************************************************************************************************
  * @brief  Configure the UxART
  * @retval None
  ***********************************************************************************************************/
void UxART_Configuration(void)
  {
  #if 0 // Use following function to configure the IP clock speed.
  // The UxART IP clock speed must be faster 16x then the baudrate.
  CKCU_SetPeripPrescaler(CKCU_PCLK_UxARTn, CKCU_APBCLKPRE_DIV2);
  #endif

  { /* Enable peripheral clock of AFIO, UxART                                                               */
    CKCU_PeripClockConfig_TypeDef CKCUClock = {{0}};
    CKCUClock.Bit.AFIO                   = 1;
    CKCUClock.Bit.HTCFG_UART0_RX_GPIO_CLK = 1;
    CKCUClock.Bit.HTCFG_UART0_IPN         = 1;
    CKCU_PeripClockConfig(CKCUClock, ENABLE);
  }

  /* Turn on UxART Rx internal pull up resistor to prevent unknow state                                     */
  GPIO_PullResistorConfig(HTCFG_UART0_RX_GPIO_PORT, HTCFG_UART0_RX_GPIO_PIN, GPIO_PR_UP);

  /* Config AFIO mode as UxART function.                                                                    */
  AFIO_GPxConfig(HTCFG_UART0_TX_GPIO_ID, HTCFG_UART0_TX_AFIO_PIN, AFIO_FUN_USART_UART);
  AFIO_GPxConfig(HTCFG_UART0_RX_GPIO_ID, HTCFG_UART0_RX_AFIO_PIN, AFIO_FUN_USART_UART);

  {
    /* UxART configured as follow:
          - BaudRate = 115200 baud
          - Word Length = 8 Bits
          - One Stop Bit
          - None parity bit
    */

    /* !!! NOTICE !!!
       Notice that the local variable (structure) did not have an initial value.
       Please confirm that there are no missing members in the parameter settings below in this function.
    */
    USART_InitTypeDef USART_InitStructure = {0};
    USART_InitStructure.USART_BaudRate = 9600;
    USART_InitStructure.USART_WordLength = USART_WORDLENGTH_8B;
    USART_InitStructure.USART_StopBits = USART_STOPBITS_1;
    USART_InitStructure.USART_Parity = USART_PARITY_NO;
    USART_InitStructure.USART_Mode = USART_MODE_NORMAL;
    USART_Init(HTCFG_UART0_PORT, &USART_InitStructure);
  }

  /* Enable UxART interrupt of NVIC                                                                         */
  NVIC_DisableIRQ(HTCFG_UART0_IRQn);

  /* Enable UxART Rx interrupt                                                                              */
  USART_IntConfig(HTCFG_UART0_PORT, USART_INT_RXDR, ENABLE);

  /* Enable UxART Timeout interrupt                                                                         */
  USART_IntConfig(HTCFG_UART0_PORT, USART_INT_TOUT, ENABLE);

  /* Seting Tx/Rx FIFO Level                                                                                */
  USART_RXTLConfig(HTCFG_UART0_PORT, USART_RXTL_04);
  USART_TXTLConfig(HTCFG_UART0_PORT, USART_TXTL_02);

  /*Seting TimeOut Counter                                                                                  */
  USART_SetTimeOutValue(HTCFG_UART0_PORT, 0x7F);

  /* Enable UxART Tx and Rx function                                                                        */
  USART_TxCmd(HTCFG_UART0_PORT, ENABLE);
  USART_RxCmd(HTCFG_UART0_PORT, DISABLE);
}
void UxART1_Configuration(void)
{
  #if 0 // Use following function to configure the IP clock speed.
  // The UxART IP clock speed must be faster 16x then the baudrate.
  CKCU_SetPeripPrescaler(CKCU_PCLK_UxARTn, CKCU_APBCLKPRE_DIV2);
  #endif

  { /* Enable peripheral clock of AFIO, UxART                                                               */
    CKCU_PeripClockConfig_TypeDef CKCUClock = {{0}};
    CKCUClock.Bit.AFIO                   = 1;
    CKCUClock.Bit.HTCFG_UART_RX_GPIO_CLK = 1;
    CKCUClock.Bit.HTCFG_UART_IPN         = 1;
    CKCU_PeripClockConfig(CKCUClock, ENABLE);
  }

  /* Turn on UxART Rx internal pull up resistor to prevent unknow state                                     */
  GPIO_PullResistorConfig(HTCFG_UART_RX_GPIO_PORT, HTCFG_UART_RX_GPIO_PIN, GPIO_PR_UP);

  /* Config AFIO mode as UxART function.                                                                    */
  AFIO_GPxConfig(HTCFG_UART_TX_GPIO_ID, HTCFG_UART_TX_AFIO_PIN, AFIO_FUN_USART_UART);
  AFIO_GPxConfig(HTCFG_UART_RX_GPIO_ID, HTCFG_UART_RX_AFIO_PIN, AFIO_FUN_USART_UART);

  {
    /* UxART configured as follow:
          - BaudRate = 115200 baud
          - Word Length = 8 Bits
          - One Stop Bit
          - None parity bit
    */

    /* !!! NOTICE !!!
       Notice that the local variable (structure) did not have an initial value.
       Please confirm that there are no missing members in the parameter settings below in this function.
    */
    USART_InitTypeDef USART_InitStructure = {0};
    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_WordLength = USART_WORDLENGTH_8B;
    USART_InitStructure.USART_StopBits = USART_STOPBITS_1;
    USART_InitStructure.USART_Parity = USART_PARITY_NO;
    USART_InitStructure.USART_Mode = USART_MODE_NORMAL;
    USART_Init(HTCFG_UART_PORT, &USART_InitStructure);
  }

  /* Enable UxART interrupt of NVIC                                                                         */
  NVIC_EnableIRQ(HTCFG_UART_IRQn);

  /* Enable UxART Rx interrupt                                                                              */
  USART_IntConfig(HTCFG_UART_PORT, USART_INT_RXDR, ENABLE);

  /* Enable UxART Timeout interrupt                                                                         */
  USART_IntConfig(HTCFG_UART_PORT, USART_INT_TOUT, ENABLE);

  /* Seting Tx/Rx FIFO Level                                                                                */
  USART_RXTLConfig(HTCFG_UART_PORT, USART_RXTL_04);
  USART_TXTLConfig(HTCFG_UART_PORT, USART_TXTL_02);

  /*Seting TimeOut Counter                                                                                  */
  USART_SetTimeOutValue(HTCFG_UART_PORT, 0x7F);

  /* Enable UxART Tx and Rx function                                                                        */
  USART_TxCmd(HTCFG_UART_PORT, ENABLE);
  USART_RxCmd(HTCFG_UART_PORT, ENABLE);
}

void EXTI_Configuration(void)
{
  /* Enable the AFIO and button GPIO Clock                                                                  */
  CKCU_PeripClockConfig_TypeDef CKCUClock = {{0}};
  CKCUClock.Bit.AFIO         = 1;
  CKCUClock.Bit.EXTI         = 1;
  CKCU_PeripClockConfig(CKCUClock, ENABLE);

  /* Configure Button pin as input floating                                                                 */
  AFIO_GPxConfig(GPIO_PC, AFIO_PIN_10, AFIO_FUN_GPIO);
	GPIO_DirectionConfig(HT_GPIOC, GPIO_PIN_10, GPIO_DIR_IN);
  GPIO_PullResistorConfig(HT_GPIOC, GPIO_PIN_10, GPIO_PR_UP);
  GPIO_InputConfig(HT_GPIOC, GPIO_PIN_10, ENABLE);


  /* Connect Button EXTI Channel to Button GPIO Pin                                                         */
  AFIO_EXTISourceConfig(AFIO_EXTI_CH_10, AFIO_ESS_PC);

  /* Enable and set EXTI Event Wakeup interrupt to the lowest priority                                      */
  //NVIC_SetPriority(EVWUP_IRQn, 0xF);
  NVIC_EnableIRQ(EVWUP_IRQn);
}

/*********************************************************************************************************//**
  * @brief  Configures RTC clock source and prescaler.
  * @retval None
  * @details The RTC configuration as following:
  *   - Check the backup domain(RTC & PWRCU) is ready for access.
  *   - Reset Backup Domain.
  *   - Enable the LSE OSC and wait till LSE is ready.
  *   - Select the RTC Clock Source as LSE.
  *   - Set the RTC time base to 1 second.
  *   - Set Compare value.
  *   - Enable the RTC.
  *   - Enable the RTC Compare Match wakeup event.
  *
  ***********************************************************************************************************/
void RTC_Configuration(void)
{
#if (EXAMPLE_RTC_LSE == 1)
  CKCU_PeripClockConfig_TypeDef CKCUClock = {{0}};
  CKCUClock.Bit.BKP          = 1;
  CKCU_PeripClockConfig(CKCUClock, ENABLE);

  #if (LIBCFG_NO_PWRCU_TEST_REG == 1)
  #else
  /* Check the backup domain(RTC & PWRCU) is ready for access                                               */
  if (PWRCU_CheckReadyAccessed() != PWRCU_OK)
  {
    while (1);
  }
  #endif

  /* Reset Backup Domain                                                                                    */
  PWRCU_DeInit();

  /* Enable the LSE OSC                                                                                     */
  RTC_LSESMConfig(RTC_LSESM_NORMAL);
  RTC_LSECmd(ENABLE);
  /* Wait till LSE is ready                                                                                 */
  while (CKCU_GetClockReadyStatus(CKCU_FLAG_LSERDY) == RESET);

  /* Select the RTC Clock Source as LSE                                                                     */
  RTC_ClockSourceConfig(RTC_SRC_LSE);

  /* Set the RTC time base to 1s                                                                            */
  RTC_SetPrescaler(RTC_RPRE_32768);

  /* Set Compare value                                                                                      */
  RTC_SetCompare(0xFFFFFFFF);

  /* Enable the RTC                                                                                         */
  RTC_Cmd(ENABLE);

  
#endif
}

void deepsleep(u32 deeptime)
{
		#if (EXAMPLE_RTC_LSE == 1)
	{
		u32 wRtcCounterTmp = 0;
		/* Enable the RTC Compare Match wakeup event                                                              */
		EXTI_WakeupEventIntConfig(ENABLE);
		RTC_WakeupConfig(RTC_WAKEUP_CM, ENABLE);
		/* Wait till RTC count occurs                                                                         */
		wRtcCounterTmp = RTC_GetCounter();
		while (RTC_GetCounter() == wRtcCounterTmp);
	
		/* Compare Match in 3 second                                                                          */
		RTC_SetCompare(RTC_GetCounter()+ deeptime);
	}
	#endif
	PWRCU_DeepSleep1(PWRCU_SLEEP_ENTRY_WFI);
}

void LowPower_Enter(void)
{
  EXTI_ClearWakeupFlag(EXTI_CHANNEL_10);

  #if (LOWPOWER_MODE_WFI == 1)
  EXTI_WakeupEventIntConfig(ENABLE);
	EXTI_WakeupEventConfig(EXTI_CHANNEL_10, EXTI_WAKEUP_HIGH_LEVEL, ENABLE);
  PWRCU_DeepSleep1(PWRCU_SLEEP_ENTRY_WFI); // WFI: wakeup by interrupt only
  #else
  PWRCU_DeepSleep1(PWRCU_SLEEP_ENTRY_WFE); // WFE: wakeup by interrupt or EXTI event
  #endif
}
#if (HT32_LIB_DEBUG == 1)
/*********************************************************************************************************//**
  * @brief  Report both the error name of the source file and the source line number.
  * @param  filename: pointer to the source file name.
  * @param  uline: error line source number.
  * @retval None
  ***********************************************************************************************************/
void assert_error(u8* filename, u32 uline)
{
  /*
     This function is called by IP library that the invalid parameters has been passed to the library API.
     Debug message can be added here.
     Example: printf("Parameter Error: file %s on line %d\r\n", filename, uline);
  */

  while (1)
  {
  }
}
#endif


/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */
