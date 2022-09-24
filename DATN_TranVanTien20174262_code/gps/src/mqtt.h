#ifndef MQTT_H_
#define MQTT_H_

#include "ht32.h"
#include "ht32_board.h"
#include "ht32_board_config.h"
#include "ht32_time.h"
#include "AT_COMMAND.h"
#include "string.h"
#include "stdlib.h"

extern bool Flag_Check_AT;
extern bool Flag_CheckEnd_AT;
extern u8 TX_Trans[128];
typedef struct
{
	u32 AT_FLAG_CHECK;
	u32 AT_FLAG_START;
	u32 AT_FLAG_ACCCLIENT;
	u32 AT_FLAG_CONNECT;
	u32 AT_FLAG_DISS;
	u32 AT_FLAG_RELES;
	u32 AT_FLAG_STPSERVICE;
	u32 AT_FLAG_INPUB;
	u32 AT_FLAG_PUBTOP;
	u32 AT_FLAG_INPDATA;
	u32 AT_FLAG_PUBDATA;
	u32 AT_FLAG_PUBMES;
	u32 AT_FLAG_INSUB;
	u32 AT_FLAG_SUBTOP;
	u32 AT_FLAG_SUBMES;
}AT_FLAG;

typedef struct
{
	bool HT32_SendData;
	bool HT32_Connect;
	bool HT32_Reset;
	bool HT32_Detect;
}HT32_FLAG;

typedef struct
{
    /* data */
    char dataRec[1024];
    char dataTrans[1024];
    char SubTopic[1024];
    char PubTopic[1024];
    int dataLen;
    int qos;
    int retain;
} MQTTdataType_t;

bool MQTT_Connect_Start(void);
void MQTT_Disconnect_Start(void);
void MQTT_PubDataTopic(char *topic, char *data, int datalen, int qos, int retain);
void MQTT_SubTopic(char *topic, int qos);
u32 Check_ATC_wait(char *notifi_rx);
#endif
