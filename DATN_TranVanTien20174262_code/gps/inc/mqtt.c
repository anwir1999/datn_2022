

#include "mqtt.h"

extern volatile SIMCOM_ResponseEvent_t AT_RX_event;
extern volatile Device_Flag D_Flag_t;
extern volatile AT_FLAG AT_Flag_t;
extern volatile HT32_FLAG HT32_FLAG_T;
extern MQTTdataType_t MQTTdataType;
extern u32 count_restart;
bool MQTT_Connect_Start(void)
{
		Time_Delay(TIME_MS2TICK(500));
    Simcom_ATC_SendATCommand("AT+CMQTTSTART\r\n", "OK", 1000, 2, Simcom_ATResponse_Callback);
		AT_START:
		AT_Flag_t.AT_FLAG_START = Check_ATC_wait("MQTT start service");
		if(AT_Flag_t.AT_FLAG_START != 0)
		{
			if(AT_Flag_t.AT_FLAG_START != 1)
			{
				goto AT_START;
			}
			else
			{
				count_restart++;
			}
		}
		else
		{
			count_restart = 0;
		} 
		Time_Delay(TIME_MS2TICK(500));
		Simcom_ATC_SendATCommand("AT+CMQTTACCQ=1,\"chungnt\"\r\n", "OK", 1000, 2, Simcom_ATResponse_Callback); 
		AT_ACCCLIENT:
		AT_Flag_t.AT_FLAG_ACCCLIENT = Check_ATC_wait("MQTT acquire a client");
		if(AT_Flag_t.AT_FLAG_ACCCLIENT != 0)
		{
			if(AT_Flag_t.AT_FLAG_ACCCLIENT != 1)
			{
				goto AT_ACCCLIENT;
			}
			else
			{
				count_restart++;
			}
		}
		else
		{
			count_restart = 0;
		}
		if(count_restart != 0)
		{
			count_restart = 0;
			return TRUE;
		}
		Time_Delay(TIME_MS2TICK(500));
		Simcom_ATC_SendATCommand("AT+CMQTTCONNECT=1,\"tcp://116.101.122.190:1883\",60,1,\"zVRIwH1nviuadaNjRBFjU9CULYZr1r1l\",\"R0BeRO26eUCd8vLM4R4GWGe5W2i6Jxj8\"\r\n", "OK", 5000, 5, Simcom_ATResponse_Callback);
		//Simcom_ATC_SendATCommand("+CMQTTCONNECT=1,\"tcp://116.101.122.190:1883\",60,1,\"batky\",\"4uIHJBMIJXJBIT98RaHWFmXoHh5bBYVN\"\r\n", "OK", 3000, 3, Simcom_ATResponse_Callback);
		//AT+CMQTTCONNECT=1,"tcp://116.101.122.190:1883",60,1,"d829afac-2a21-46f4-af67-3adb93233a57","R0BeRO26eUCd8vLM4R4GWGe5W2i6Jxj8"
		AT_CONNECT:
		AT_Flag_t.AT_FLAG_CONNECT = Check_ATC_wait("MQTT connect service");
		if(AT_Flag_t.AT_FLAG_CONNECT != 0)
		{
			goto AT_CONNECT;
		}
		else if (AT_Flag_t.AT_FLAG_CONNECT == 0)
		{
			HT32_FLAG_T.HT32_Connect = TRUE;
		}
		return FALSE;
}

void MQTT_Disconnect_Start(void)
{
		Simcom_ATC_SendATCommand("AT+CMQTTDISC=1,60\r\n", "OK", 500, 4, Simcom_ATResponse_Callback);
		AT_DISS:
		AT_Flag_t.AT_FLAG_DISS = Check_ATC_wait("MQTT disconnect server");
		if(AT_Flag_t.AT_FLAG_DISS != 0)
		{
			goto AT_DISS;
		}
		
    if(Flag_CheckEnd_AT == true)
    {
				//Count_Connect = 0;
        
				Simcom_ATC_SendATCommand("AT+CMQTTREL=1\r\n", "OK", 3000, 3, Simcom_ATResponse_Callback);
				AT_RELES:
				AT_Flag_t.AT_FLAG_RELES = Check_ATC_wait("MQTT release the client");
				if(AT_Flag_t.AT_FLAG_RELES != 0)
				{
					goto AT_RELES;
				}
				Simcom_ATC_SendATCommand("AT+CMQTTSTOP\r\n", "OK", 3000, 3, Simcom_ATResponse_Callback);
				AT_STPSERVICE:
				AT_Flag_t.AT_FLAG_STPSERVICE = Check_ATC_wait("MQTT stop service ");
				if(AT_Flag_t.AT_FLAG_STPSERVICE != 0)
				{
					goto AT_STPSERVICE;
				}
    }
}

void MQTT_PubDataTopic(char *topic, char *data, int datalen, int qos, int retain)
{
    char buf[256];
    int len;
    len = sprintf(buf, "AT+CMQTTTOPIC=1,%d\r\n", strlen(topic));
    buf[len] = 0;
    MQTTdataType.dataLen = datalen;
    MQTTdataType.qos = qos;
    MQTTdataType.retain = retain;
    sprintf(MQTTdataType.dataTrans, "%s\r\n", data);
    MQTTdataType.dataTrans[datalen+2] = 0;
    sprintf(MQTTdataType.PubTopic, "%s\r\n", topic);
    MQTTdataType.PubTopic[strlen(topic)+2] = 0;
    Simcom_ATC_SendATCommand(buf, ">", 1000, 3, Simcom_ATResponse_Callback);
		AT_INPUB:
		AT_Flag_t.AT_FLAG_INPUB = Check_ATC_wait("MQTT input topic public");
		if(AT_Flag_t.AT_FLAG_INPUB != 0)
		{
			goto AT_INPUB;
		}
		sprintf((char *) TX_Trans, "Pub topic: %s", MQTTdataType.PubTopic);
		//UxART1_TxTest((u8 *)TX_Trans, strlen((char *)TX_Trans));
		Simcom_ATC_SendATCommand(MQTTdataType.PubTopic, "OK", 10000, 3, Simcom_ATResponse_Callback);
		//UxART1_TxTest((u8 *)MQTTdataType.PubTopic, strlen((char *)MQTTdataType.PubTopic));
		AT_PUBTOP:
		AT_Flag_t.AT_FLAG_PUBTOP = Check_ATC_wait("MQTT topic name");
		if(AT_Flag_t.AT_FLAG_PUBTOP != 0)
		{
			goto AT_PUBTOP;
		}
		sprintf(buf, "AT+CMQTTPAYLOAD=1,%d\r\n", MQTTdataType.dataLen);
    Simcom_ATC_SendATCommand(buf, ">", 2000, 3, Simcom_ATResponse_Callback);
		sprintf((char *)buf, "MQTT input topic OK\r\n");
		//UxART1_TxTest((u8 *)buf, strlen((char *)buf));
		AT_INPDATA:
		AT_Flag_t.AT_FLAG_INPDATA = Check_ATC_wait("MQTT input data");
		if(AT_Flag_t.AT_FLAG_INPDATA != 0)
		{
			goto AT_INPDATA;
		}
		//publish message name
		//sprintf((char *) TX_Trans, "Pub data: %s \r\n", MQTTdataType.dataTrans);
		//UxART1_TxTest((u8 *)TX_Trans, strlen((char *)TX_Trans));
	  Simcom_ATC_SendATCommand(MQTTdataType.dataTrans, "OK", 2000, 3, Simcom_ATResponse_Callback);
		//UxART1_TxTest((u8 *)MQTTdataType.dataTrans, strlen((char *)MQTTdataType.dataTrans));
		Time_Delay(TIME_MS2TICK(500));
		AT_PUBDATA:
		AT_Flag_t.AT_FLAG_PUBDATA = Check_ATC_wait("MQTT publish message name");
		if(AT_Flag_t.AT_FLAG_PUBDATA != 0)
		{
			goto AT_PUBDATA;
		}
		//Publish a message to server
		sprintf(buf, "AT+CMQTTPUB=1,%d,%d,%d\r\n", MQTTdataType.qos, 60, MQTTdataType.retain);
		//UxART1_TxTest((u8 *)buf, strlen((char *)buf));
		Simcom_ATC_SendATCommand(buf, "OK", 2000, 3, Simcom_ATResponse_Callback);
		AT_PUBMES:
		AT_Flag_t.AT_FLAG_PUBMES = Check_ATC_wait("MQTT publish a message to server");
		if(AT_Flag_t.AT_FLAG_PUBMES != 0)
		{
			goto AT_PUBMES;
		}
		
}

void MQTT_SubTopic(char *topic, int qos)
{
	char buff[256];
	MQTTdataType.qos = qos;
	Time_Delay(TIME_MS2TICK(1000));
	sprintf(buff, "AT+CMQTTSUBTOPIC=1,%d,%d\r\n", strlen(topic), MQTTdataType.qos);
	sprintf(MQTTdataType.SubTopic, "%s\r\n", topic);
	MQTTdataType.SubTopic[strlen(topic)+2] = 0;
	// input topic subscribe
	if(HT32_FLAG_T.HT32_Connect == TRUE)
	{
		HT32_FLAG_T.HT32_Connect = FALSE;
		Simcom_ATC_SendATCommand(buff, ">", 3000, 5, Simcom_ATResponse_Callback);
		AT_INSUB:
		AT_Flag_t.AT_FLAG_INSUB = Check_ATC_wait("MQTT input topic subscribe");
		if(AT_Flag_t.AT_FLAG_INSUB != 0)
		{
			goto AT_INSUB;
		}
		
		//subscribe topic
		Simcom_ATC_SendATCommand(MQTTdataType.SubTopic, "OK", 5000, 3, Simcom_ATResponse_Callback);
		AT_SUBTOP:
		AT_Flag_t.AT_FLAG_SUBTOP = Check_ATC_wait("MQTT subscribe topic name");
		if(AT_Flag_t.AT_FLAG_SUBTOP != 0)
		{
			goto AT_SUBTOP;
		}
		
		//Subscribe a message to server
		Simcom_ATC_SendATCommand("AT+CMQTTSUB=1\r\n", "OK", 1000, 3, Simcom_ATResponse_Callback);
		AT_SUBMES:
		AT_Flag_t.AT_FLAG_SUBMES = Check_ATC_wait("MQTT subscribe a message to server");
		if(AT_Flag_t.AT_FLAG_SUBMES != 0)
		{
			goto AT_SUBMES;
		}
	}
}
u32 Check_ATC_wait(char *notifi_rx)
{
	while(1)
	{
		if(D_Flag_t.Flag_Wait_Exit == true)
		{
			D_Flag_t.Flag_Wait_Exit = false;
			if (AT_RX_event == SIMCOM_EVEN_TIMEOUT || AT_RX_event == SIMCOM_EVEN_ERROR)
			{
				if (AT_RX_event == SIMCOM_EVEN_TIMEOUT)
				{
					//sprintf((char *)TX_Trans, "%s failed (TIMEOUT)\r\n", notifi_rx);
					//UxART1_TxTest((u8 *)TX_Trans, strlen((char *)TX_Trans));
					return 1;
				}
				else
				{
					//sprintf((char *)TX_Trans, "%s failed (ERROR)\r\n", notifi_rx);
					return 2;
					//UxART1_TxTest((u8 *)TX_Trans, strlen((char *)TX_Trans));
				}
			}
			else if (AT_RX_event == SIMCOM_EVEN_OK)
			{
				//sprintf((char *)TX_Trans, "%s OK\r\n", notifi_rx);
				//UxART1_TxTest((u8 *)TX_Trans, strlen((char *)TX_Trans));
				Time_Delay(TIME_MS2TICK(100));
				return 0;
			}
		}
	}
}
