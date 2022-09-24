#ifndef GPSRECEI_H_
#define GPSRECEI_H_

#include "ht32.h"
#include "ht32_board.h"
#include "ht32_board_config.h"
#include "string.h"
#include "stdlib.h"
#include "AT_COMMAND.h"

typedef struct 
{
    char GPS_RX_DATA[512];
    int GPS_RX_INDEX;
		bool FLAG_CHECK;
		bool FLAG_START;
		float longitude;
    float latitude;
    float utc_time;
		char ns, ew;
		float speed_k;
    float course_d;
    int date;
	  char msl_units;
		u32 detect;
		int st_mess;
}GPS;

typedef struct
{
	char hour[10];
	int hour_i;
	char minute[10];
	char second[10];
	char time[128];
	int day;
	int month;
	int year;
	char date[128];
}DT;

void UxART_TxTest(u8 *Ptr, u32 Ptr_len);
void GPS_RX_Process(void);
int filter_comma(char *respond_data, int begin, int end, char *output);
void Conver_DateTime(char *datetime, char kind);
#endif
