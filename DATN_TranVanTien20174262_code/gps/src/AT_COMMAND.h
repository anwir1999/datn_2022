#ifndef AT_COMMAND_H_
#define AT_COMMAND_H_

#include "ht32.h"
#include "ht32_board.h"
#include "ht32_board_config.h"
#include "mqtt.h"
#include "string.h"
#include "stdlib.h"
#include "GPS.h"
#define TIMER_ATC_PERIOD 100
#define BUF_SIZE (1024)

#define DAM_BUX_TX 2048
// Simcom AT functions
typedef enum
{
    SIMCOM_EVEN_OK = 0,
    SIMCOM_EVEN_TIMEOUT,
    SIMCOM_EVEN_ERROR,
    SIMCOM_EVEN_NULL,
} SIMCOM_ResponseEvent_t;

typedef struct
{
    bool Flag_ScanNetwok;
    bool Flag_Cycle_Completed;
    bool Flag_ActiveNetwork;
    bool Flag_DeactiveNetwork;
    bool Flag_Network_Active;
    bool Flag_Network_Check_OK;
    bool Flag_Wait_Exit;
    bool Flag_GPS_Started;
    bool Flag_GPS_Stopped;
    bool Flag_Timer_GPS_Run;
    bool Flag_Device_Ready;
    bool Flag_WifiScan_Request;
    bool Flag_WifiScan_End;
    bool Flag_WifiCell_OK;
    bool Flag_Restart7070G_OK;
    bool Flag_CFUN_0_OK;
    bool Flag_CFUN_1_OK;
    bool Flag_Control_7070G_GPIO_OK;
    bool Flag_SelectNetwork_OK;
    bool Flag_NeedToProcess_GPS;
    bool Flag_MQTT_Stop_OK;
    bool Flag_RebootNetworkModule_OK;
    bool Flag_NeedToRebootNetworkModule;
    // Need adding to reset parameters funtion
    bool Flag_MQTT_Connected;
    bool Flag_MQTT_Sub_OK;
    bool Flag_MQTT_Publish_OK;
    bool Flag_MQTT_SubMessage_Processing;

    uint8_t Task_Selection;
}Device_Flag;


typedef void (*SIMCOM_SendATCallBack_t)(SIMCOM_ResponseEvent_t event, void *ResponseBuffer);

void Simcom_SendATCommand(void);
void Simcom_RetrySendATC(void);
void Simcom_ATC_SendATCommand(const char *Command, char *ExpectResponse, uint32_t Timeout, uint8_t RetryCount, SIMCOM_SendATCallBack_t CallBackFunction);
void Simcom_ATResponse_Callback(SIMCOM_ResponseEvent_t event, void *ResponseBuffer);

typedef struct 
{
    char CMD[DAM_BUX_TX];
    volatile u32 lenCMD;
    char ExpectResponseFromATC[20];
    uint32_t TimeoutATC;
    uint32_t CurrentTimeoutATC;
    uint8_t RetryCountATC;
    SIMCOM_SendATCallBack_t SendATCallBack;
    bool Flag_RX_End;
    char AT_RX_Data[BUF_SIZE];
    uint32_t AT_RX_ArrayIndex;
}ATCommand_t;   
typedef struct
{
	char stCheck[10];
	char stpk[10];
	char tp[10];
	bool st_flag;
	bool st_speaker;
	int time_period;
	char mess_rx[256];
}st_device;
// Common functions

void WaitandExitLoop(volatile bool *Flag);
void UxART1_TxTest(u8 *Ptr, u32 Ptr_len);
void Configure_AT_Command(void);
void ATCommand_RX_Process(void);
int filter_comma_v2(char *respond_data, int begin, int end, char *output);
#endif
