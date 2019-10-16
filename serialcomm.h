#include "serialport.h"
#include <pthread.h>
#include <sql.h>
#include <sqlext.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

//VDM Info
#define Message_ID 6
#define Repeat_indicator 2
#define User_ID 30
#define Navigational_status 4
#define Rate_of_turn_ROT 8
#define SOG 10
#define Position accuracy 1
#define Longitude 28
#define Latitude 27
#define COG 12
#define True_heading 9
#define Time_stamp
#define Reserved_for_regional_applications 4
#define Spare 1
#define RAIM_Flag 1
#define Communication_State 19

//
#define SUCCESS 1;
#define FAIL    0;


#define RETURN_SUCCESS 1
#define RETURN_FAIL 0

extern "C" void* connect_thread(void* args);

struct thread_parameter{

	void* serialcomm_adr;
    char  portname[MAX_PATH];
};


class  CSerialComm 
{
private : 
	CSerialPort*    serial;
	
	thread_parameter* thread_p;
	
	bool not_connect[7];



	//char LATITUE[31],RMR[31],LONGITUDE[31],EAST[31],UTC[31],STATUS[31];

	SQLCHAR     LATITUE[30],RMR[30],LONGITUDE[30],EAST[30],UTC[30],STATUS[30];

	SQLCHAR     D[10],E[100],SHIP_NAME[20];

	SQLINTEGER  A,B,C,user_id;  
	SQLFLOAT filed[20];
	SQLFLOAT MMSI_ID;
	
	SQLCHAR ascii[23];

	SQLINTEGER  Total_Number_Of_Sentences,Sentence_number,Sequential_message_identifier;
	
    SQLCHAR     AIS_channel[3];

	pthread_mutex_t m_mutex;
	
public :
	CSerialComm();
	~CSerialComm();
	
	void             find_port();     // 무한 루프 돌려야 할곳
	int				connect(char* _portNum, int bps,CSerialPort*  serialport);
	int				sendCommand(unsigned char pos);
	void			disconnect(CSerialPort*  serialport);
	int				recv(char *buff,CSerialPort*    serialport);
	int             run(char *portname);
	int             connect_port(char* portname,int bps, CSerialPort*  serialport);
	void            recv_portname(char* portname);  //void* serial_type
	void sqlsin();
	int Parse_GLL(const char* sentence);
	int Parse_VDM(const char* sentence);

	void DB_realtime();
	void DB_M1to3();
	void DB_M5();
	bool M1to3(unsigned char* sentence);
	bool VDM_parse(unsigned char* sentence);
	bool M5(unsigned char* sentence);
};