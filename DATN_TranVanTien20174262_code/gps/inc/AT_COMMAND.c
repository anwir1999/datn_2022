
#include "AT_COMMAND.h"

extern volatile SIMCOM_ResponseEvent_t AT_RX_event;
extern volatile Device_Flag D_Flag_t;
extern ATCommand_t SIMCOM_ATCommand;
extern uc8  *gURTx1_Ptr;
extern vu32 gURTx1_Length;
extern volatile HT32_FLAG HT32_FLAG_T;
extern vu32 TimePeriod;
extern u8 charTimePeriod[128];
extern vu32 utick;
extern u32 count_NW;
extern st_device st_device_t;
int count_filter_v2, lim_v2, start_v2, finish_v2 ,i_v2;
#define DEBUG_IO          (0)
#if (DEBUG_IO == 1)
  #define DBG_IO1_LO()    HT32F_DVB_LEDOn(HT_LED1)
  #define DBG_IO1_HI()    HT32F_DVB_LEDOff(HT_LED1)
  #define DBG_IO2_LO()    HT32F_DVB_LEDOn(HT_LED2)
  #define DBG_IO2_HI()    HT32F_DVB_LEDOff(HT_LED2)
#else
  #define DBG_IO1_LO(...)
  #define DBG_IO1_HI(...)
  #define DBG_IO2_LO(...)
  #define DBG_IO2_HI(...)
#endif

void Simcom_SendATCommand()
{
    //UARTprintf("Send: %s", SIMCOM_ATCommand.CMD);
    UxART1_TxTest((unsigned char *) SIMCOM_ATCommand.CMD, SIMCOM_ATCommand.lenCMD);
}
//-----------------------------------------------------------------------------------------//
void Simcom_ATC_SendATCommand(const char *Command, char *ExpectResponse, uint32_t Timeout, uint8_t RetryCount, SIMCOM_SendATCallBack_t CallBackFunction)
{
		//u8 tx[128];
    //UARTprintf("Send: %s\r", Command);
		memset(SIMCOM_ATCommand.CMD, 0, sizeof(SIMCOM_ATCommand.CMD));
    strcpy(SIMCOM_ATCommand.CMD, Command);
		//sprintf((char *)tx, "len: %d\r\n", strlen(Command));
		//UxART1_TxTest((u8 *)tx, strlen((char *)tx));
    SIMCOM_ATCommand.lenCMD = strlen(Command);
    strcpy(SIMCOM_ATCommand.ExpectResponseFromATC, ExpectResponse);
    SIMCOM_ATCommand.RetryCountATC = RetryCount;
    SIMCOM_ATCommand.SendATCallBack = CallBackFunction;
    SIMCOM_ATCommand.TimeoutATC = Timeout;
    SIMCOM_ATCommand.CurrentTimeoutATC = 0;
    AT_RX_event = SIMCOM_EVEN_NULL;
	
		Simcom_SendATCommand();

}
//-----------------------------------------------------------------------------------------//
void Simcom_RetrySendATC()
{
    Simcom_SendATCommand();
}
//-----------------------------------------------------------------------------------------//
void Simcom_ATResponse_Callback(SIMCOM_ResponseEvent_t event, void *ResponseBuffer)
{
    AT_RX_event = event;
    if(event == SIMCOM_EVEN_OK)
    {
        //UARTprintf("SIMCOM_EVENT_OK\r\n");
        D_Flag_t.Flag_Wait_Exit = true;
        D_Flag_t.Flag_Device_Ready = true;
    }
    else if(event == SIMCOM_EVEN_TIMEOUT)
    {
        //UARTprintf("SIMCOM_EVENT_TIMEOUT\r\n");
        D_Flag_t.Flag_Wait_Exit = true;
    }
    else if(event == SIMCOM_EVEN_ERROR)
    {
        //UARTprintf("SIMCOM_EVENT_ERROR\r\n");
        D_Flag_t.Flag_Wait_Exit = true;
    }
}

void WaitandExitLoop(volatile bool *Flag)
{
    while(1)
    {
        if(*Flag == true)
        {
            *Flag = false;
            break;
        }
        //SysCtlDelay(SysCtlClockGet()/10);
        //vTaskDelay(50 / RTOS_TICK_PERIOD_MS);
    }
	}

/*********************************************************************************************************//**
  * @brief  UxART Tx Test.
  * @retval None
  ***********************************************************************************************************/

void UxART1_TxTest(u8 *Ptr, u32 Ptr_len)
{
	u8 data[128];
	u32 i;
	for(i = 0; i < Ptr_len; i++)
	{
		data[i] = *Ptr++;
	}
  gURTx1_Ptr = (u8 *)data;
  gURTx1_Length = Ptr_len ;
  //USART_IntConfig(HTCFG_UART0_PORT, USART_INT_TXDE | USART_INT_TXC, ENABLE);

  while (gURTx1_Length != 0)      // Latest byte move to UxART shift register, but the transmission may be on going.
	{
		while (USART_GetFIFOStatus(HTCFG_UART_PORT, USART_FIFO_TX) != 8)
    {
      USART_SendData(HTCFG_UART_PORT, *gURTx1_Ptr++);
      gURTx1_Length--;
      if (gURTx1_Length == 0)
      {
        //USART_IntConfig(HTCFG_UART0_PORT, USART_INT_TXDE, DISABLE);
        break;
      }
    }
	}
  //while (gIsTxFinished == FALSE); // Set by TXC interrupt, transmission is finished.
}

void Configure_AT_Command(void)
{
    memset(SIMCOM_ATCommand.AT_RX_Data, 0, sizeof(SIMCOM_ATCommand.AT_RX_Data));

    SysTick_Config(SystemCoreClock/8);
}

void SysTick_Handler(void)
{
	utick += 100;
	if(SIMCOM_ATCommand.TimeoutATC > 0 && SIMCOM_ATCommand.CurrentTimeoutATC < SIMCOM_ATCommand.TimeoutATC){
		SIMCOM_ATCommand.CurrentTimeoutATC += TIMER_ATC_PERIOD;
		if(SIMCOM_ATCommand.CurrentTimeoutATC >= SIMCOM_ATCommand.TimeoutATC){
			SIMCOM_ATCommand.CurrentTimeoutATC -= SIMCOM_ATCommand.TimeoutATC;
			if(SIMCOM_ATCommand.RetryCountATC > 0){
				Simcom_RetrySendATC();
				SIMCOM_ATCommand.RetryCountATC--;
			}
			else{
				SIMCOM_ATCommand.TimeoutATC = 0;
				if(SIMCOM_ATCommand.SendATCallBack != NULL)
					{
						SIMCOM_ATCommand.SendATCallBack(SIMCOM_EVEN_TIMEOUT, "@@@");
						SIMCOM_ATCommand.SendATCallBack = NULL;
          }
			}
		}
	}
}

void ATCommand_RX_Process()
{
    // Return event OK
    if(SIMCOM_ATCommand.ExpectResponseFromATC[0] != 0 && strstr((const char*)SIMCOM_ATCommand.AT_RX_Data, SIMCOM_ATCommand.ExpectResponseFromATC))
    {
       SIMCOM_ATCommand.ExpectResponseFromATC[0] = 0;
       if(SIMCOM_ATCommand.SendATCallBack != NULL)
       {
           SIMCOM_ATCommand.TimeoutATC = 0;
           SIMCOM_ATCommand.SendATCallBack(SIMCOM_EVEN_OK, SIMCOM_ATCommand.AT_RX_Data);
       }
    }

    // Check error
    if(strstr((const char*)SIMCOM_ATCommand.AT_RX_Data, "ERROR"))
    {
       if(SIMCOM_ATCommand.SendATCallBack != NULL)
       {
           SIMCOM_ATCommand.SendATCallBack(SIMCOM_EVEN_ERROR, SIMCOM_ATCommand.AT_RX_Data);
       }
    }
		// Check error
    if(strstr((const char*)SIMCOM_ATCommand.AT_RX_Data, "\"stCheck\""))
		{
			memset(st_device_t.mess_rx, 0, strlen(st_device_t.mess_rx));
			strcpy((char *)st_device_t.mess_rx,(char *)SIMCOM_ATCommand.AT_RX_Data);
			filter_comma_v2((char *)st_device_t.mess_rx, 3, 4, (char *)st_device_t.stCheck);
			if(strstr((const char *)st_device_t.stCheck, "off"))
			{
				st_device_t.st_flag = false;
			}
			else if(strstr((const char *)st_device_t.stCheck, "on"))
			{
				st_device_t.st_flag = true;
				filter_comma_v2((char *)st_device_t.mess_rx, 7, 8, (char *)st_device_t.stpk);
				if(strstr((const char *)st_device_t.stpk, "off")||strstr((const char *)st_device_t.stpk, "null"))
				{
					st_device_t.st_speaker = false;
				}
				else if(strstr((const char *)st_device_t.stpk, "on"))
				{
					st_device_t.st_speaker = true;
				}
				filter_comma_v2((char *)st_device_t.mess_rx, 11, 12, (char *)st_device_t.tp);
				st_device_t.time_period = atoi((const char *)st_device_t.tp);
			}
			
    }
		if(strstr((const char*)SIMCOM_ATCommand.AT_RX_Data, "PB DONE"))
		{
			HT32_FLAG_T.HT32_Reset = TRUE;
		}
		if(strstr((const char*)SIMCOM_ATCommand.AT_RX_Data, "+CEREG:"))
		{
			count_NW = (int)SIMCOM_ATCommand.AT_RX_Data[10] - '0';
		}
    // Reset AT command buffer
    memset(SIMCOM_ATCommand.AT_RX_Data, 0, sizeof(SIMCOM_ATCommand.AT_RX_Data));
    SIMCOM_ATCommand.AT_RX_ArrayIndex = 0;
}

void HTCFG_UART_IRQHandler(void)
{
  bool bIsRxDataReady = FALSE;

  /* Timeout: Clear Timeout Flag                                                                            */
  if (USART_GetFlagStatus(HTCFG_UART_PORT, USART_FLAG_TOUT))
  {
    USART_ClearFlag(HTCFG_UART_PORT, USART_FLAG_TOUT);
    bIsRxDataReady = TRUE;
    DBG_IO1_HI(); // Note: The width of the latest Rx byte Stop bit and IO HI is the timeout timming.
  }

  /* Rx FIFO level is reached                                                                               */
  if (USART_GetFlagStatus(HTCFG_UART_PORT, USART_FLAG_RXDR))
  {
    bIsRxDataReady = TRUE;
    DBG_IO1_LO();
  }

  /* Rx: Move data from USART FIFO to buffer                                                                */
  if (bIsRxDataReady == TRUE)
  {
    while (USART_GetFIFOStatus(HTCFG_UART_PORT, USART_FIFO_RX))
    {
      SIMCOM_ATCommand.AT_RX_Data[SIMCOM_ATCommand.AT_RX_ArrayIndex++] = USART_ReceiveData(HTCFG_UART_PORT);
			if(SIMCOM_ATCommand.AT_RX_Data[SIMCOM_ATCommand.AT_RX_ArrayIndex-1] == '\n'){
				//gURRx_Length++;
				SIMCOM_ATCommand.AT_RX_Data[SIMCOM_ATCommand.AT_RX_ArrayIndex] = '\0';
				SIMCOM_ATCommand.Flag_RX_End = true;
				ATCommand_RX_Process();
			}
      #if 0
      if (SIMCOM_ATCommand.AT_RX_ArrayIndex == 128)
      {
        while (1) {}; // Rx Buffer full
      }
      #endif
    }
  }
}
int filter_comma_v2(char *respond_data, int begin, int end, char *output)
{
    memset(output, 0, strlen(output));
		count_filter_v2 = 0;
	lim_v2 = 0;
	start_v2 = 0;
	finish_v2 = 0;
	 i_v2 = 0;
    for (i_v2 = 0; i_v2 < strlen(respond_data); i_v2++)
    {
        if ( respond_data[i_v2] == '\"')
        {
            count_filter_v2 ++;
            if (count_filter_v2 == begin)          start_v2 = i_v2+1;
            if (count_filter_v2 == end)            finish_v2 = i_v2;
        }

    }
    lim_v2 = finish_v2 - start_v2;
    for (i_v2 = 0; i_v2 < lim_v2; i_v2++){
        output[i_v2] = respond_data[start_v2];
        start_v2 ++;
    }
    output[i_v2] = 0;
    return 0;
}
