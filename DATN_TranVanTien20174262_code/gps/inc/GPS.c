
#include "GPS.h"

extern ATCommand_t SIMCOM_ATCommand;
extern GPS gps_t;
extern DT DateTime; 
extern char gps_rx[256];
extern u32 n;
int lat_current;
int lon_current;
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
extern uc8  *gURTx0_Ptr;
extern u32 gURTx0_Length;
extern u8 Time[128];
extern u8 Day[128];
extern u8 latitude[50];
extern u8 longitude[50];
extern u8 sog[50];
extern DT DateTime; 
extern int count_filter, lim, start, finish ,i;
extern u8 count_mess;
void UxART_TxTest(u8 *Ptr, u32 Ptr_len)
{
	char data[128];
	u32 i;
	for(i = 0; i < Ptr_len; i++)
	{
		data[i] = *Ptr++;
	}
  gURTx0_Ptr = (u8 *)data;
  gURTx0_Length = Ptr_len ;

  while (gURTx0_Length != 0)      // Latest byte move to UxART shift register, but the transmission may be on going.
	{
		while (USART_GetFIFOStatus(HTCFG_UART0_PORT, USART_FIFO_TX) != 8)
    {
      USART_SendData(HTCFG_UART0_PORT, *gURTx0_Ptr++);
      gURTx0_Length--;
      if (gURTx0_Length == 0)
      {
        break;
      }
    }
	}
}


void GPS_RX_Process(void)
{

    // Check error
    if(strstr((const char*)gps_t.GPS_RX_DATA, "$GNRMC"))
    {
				memset(gps_rx, 0, strlen(gps_rx));
				strcpy((char *)gps_rx, (char *)gps_t.GPS_RX_DATA);
				filter_comma((char *)gps_rx, 1, 2, (char *)Time);
				filter_comma((char *)gps_rx, 3, 4, (char *)latitude);
				filter_comma((char *)gps_rx, 5, 6, (char *)longitude);
				filter_comma((char *)gps_rx, 7, 8, (char *)sog);
				filter_comma((char *)gps_rx, 9, 10, (char *)Day);
			
				Conver_DateTime((char *)Time, 'T');
				sprintf((char *)Time, "%s:%s:%s", DateTime.hour, DateTime.minute, DateTime.second);
				gps_t.latitude = atof((const char *)latitude);
				lat_current = (int )gps_t.latitude/100;
				gps_t.latitude = lat_current + ((gps_t.latitude - lat_current*100)/60);
			
				gps_t.longitude = atof((const char *)longitude);
				lon_current = (int )gps_t.longitude/100;
				gps_t.longitude = lon_current + ((gps_t.longitude - lon_current*100)/60);
				gps_t.speed_k = atof((const char *)sog);
				Conver_DateTime((char *)Day, 'D');
				sprintf((char *)Day, "%d/%d/%d", DateTime.day, DateTime.month, DateTime.year );
				if(count_mess == 1)
				{
					gps_t.st_mess = 0;
				}
				else if(count_mess == 2)
				{
					gps_t.st_mess = 1;
				}
				sprintf(gps_rx, "{\"status\": \"%d\",\"latht\": \"%f\",\"lonht\": \"%f\",\"speedht\": \"%0.2f\",\"timeht\": \"%s\",\"dateht\": \"%s\", \"detectht\": \"%d\"}",gps_t.st_mess, gps_t.latitude, gps_t.longitude, gps_t.speed_k,Time,Day, gps_t.detect);	
				if(gps_t.latitude != 0 && gps_t.longitude != 0)
				{
					gps_t.FLAG_CHECK = TRUE;
					if(n == 0)
					{
						n = 1;
					}
				}
				//gps_t.FLAG_CHECK = TRUE;
				memset(gps_t.GPS_RX_DATA, 0, strlen(gps_t.GPS_RX_DATA));
    }

    // Reset AT command buffer
		
    memset((char *)gps_t.GPS_RX_DATA, 0, sizeof(gps_t.GPS_RX_DATA));
    gps_t.GPS_RX_INDEX = 0;
}

void HTCFG_UART0_IRQHandler(void)
{
  bool bIsRxDataReady = FALSE;

  /* Timeout: Clear Timeout Flag                                                                            */
  if (USART_GetFlagStatus(HTCFG_UART0_PORT, USART_FLAG_TOUT))
  {
    USART_ClearFlag(HTCFG_UART0_PORT, USART_FLAG_TOUT);
    bIsRxDataReady = TRUE;
    DBG_IO1_HI(); // Note: The width of the latest Rx byte Stop bit and IO HI is the timeout timming.
  }

  /* Rx FIFO level is reached                                                                               */
  if (USART_GetFlagStatus(HTCFG_UART0_PORT, USART_FLAG_RXDR))
  {
    bIsRxDataReady = TRUE;
    DBG_IO1_LO();
  }

  /* Rx: Move data from USART FIFO to buffer                                                                */
  if (bIsRxDataReady == TRUE)
  {
    while (USART_GetFIFOStatus(HTCFG_UART0_PORT, USART_FIFO_RX))
    {
			
			gps_t.GPS_RX_DATA[gps_t.GPS_RX_INDEX++] = USART_ReceiveData(HTCFG_UART0_PORT);
			if(gps_t.GPS_RX_DATA[gps_t.GPS_RX_INDEX-1] == 10){
				//gURRx_Length++;
				gps_t.GPS_RX_DATA[gps_t.GPS_RX_INDEX] = '\0';
				GPS_RX_Process();
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

int filter_comma(char *respond_data, int begin, int end, char *output)
{
    memset(output, 0, strlen(output));
		count_filter = 0;
		lim = 0;
		start = 0;
		finish = 0;
    for (i = 0; i < strlen(respond_data); i++)
    {
        if ( respond_data[i] == ',')
        {
            count_filter ++;
            if (count_filter == begin)          start = i+1;
            if (count_filter == end)            finish = i;
        }

    }
    lim = finish - start;
    for (i = 0; i < lim; i++){
        output[i] = respond_data[start];
        start ++;
    }
    output[i] = 0;
    return 0;
}
void Conver_DateTime(char *datetime, char kind)
{
	char cv[50];
	if(kind == 'D')
	{
			strncpy(cv, datetime + 0, 2);
			DateTime.day = atoi(cv);
			strncpy(cv, datetime + 2, 2);
			DateTime.month = atoi(cv);
			strncpy(cv, datetime + 4, 2);
			DateTime.year = atoi(cv) + 2000;
	}
	else
	{
			strncpy(DateTime.hour, datetime + 0, 2);
			DateTime.hour_i = atoi(DateTime.hour) + 7;
			if(DateTime.hour_i >= 24)
			{
				DateTime.hour_i -= 24;
			}
			sprintf((char *)DateTime.hour	,"%d",DateTime.hour_i );
			if(DateTime.hour_i >= 0 && DateTime.hour_i < 10)
			{
				sprintf((char *)DateTime.hour	,"0%d",DateTime.hour_i );
			}
			strncpy(DateTime.minute, datetime + 2, 2);
			//DateTime.minute = atoi(minute_ch);
			strncpy(DateTime.second, datetime + 4, 2);
			//DateTime.second = atoi(second_ch);
	}
}
