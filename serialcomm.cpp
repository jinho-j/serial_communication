#include "serialcomm.h"




CSerialComm::CSerialComm(){
	pthread_mutex_init(&m_mutex, NULL);}
CSerialComm::~CSerialComm(){}


int CSerialComm::Parse_VDM(const char* sentence){


	char * sentence_pointer;

	char * next_stence_pointer;

	int comp_length;

	char temp[100],temp_sql[102];

	int token_n = 0;

	char buff[100],space[3]="''";

	//aditional
	bool test=false;


	if(strstr(sentence,"VDM")!=NULL)
	{
		strcpy(buff,sentence);

		sentence_pointer = strstr(buff, ",");


		while (token_n != 5) {

			//GLL = new GPS_GLL;

			sentence_pointer = strstr(sentence_pointer, ",") + 1;

			//aditional
			if(*sentence_pointer == ',')
				test = !test;

			next_stence_pointer = strstr(sentence_pointer, ",") + 1;

			comp_length = next_stence_pointer - sentence_pointer;

			memset(temp, 0, 100);

			//input information to temp 
			if (comp_length > 0)

				memcpy(temp, sentence_pointer, comp_length - 1);


			else

				memcpy(temp, sentence_pointer, buff + strlen(buff) - sentence_pointer);

			// input information       
			if (token_n == 0) {
				//GLL->Latitue = atof(temp);
				if(test)
					A=-1;
				else{

					A=atoi(temp);
					
				}
				// printf("A :  %d\n", A);
			}
			else if (token_n == 1) {
				if(test)
					B=-1;
				else{

					B=atoi(temp);
					
				}
				// printf("B :  %d\n",B);
			}
			else if (token_n == 2) {
				if(test)
					C=-1;
				else
					C=atoi(temp);

				//printf("C :  %s\n", C);

			}
			else if (token_n == 3) {
				if(test)
					memcpy(D,space,3);
				else{
					sprintf(temp_sql,"'%S'",temp);
					memcpy(D,temp_sql,12);
				}
				//printf("D :  %s\n", D);

			}

			else if (token_n == 4) {
				if(test)
					memcpy(E,space,3);
				else{
					memcpy(E,temp,100);  
				}
				// printf("E :  %s\n",E);
			}

			token_n++;

			if(test)
				test = !test;

		}
	}
	else
		return FAIL;

	//printf("%d\n",strlen((char*)E));
	if(strlen((char*)E)==28)
		return SUCCESS;
	 
	if(strlen((char*)E)==60)
		return SUCCESS;
	 
		
	


}


void CSerialComm::find_port(){
	int com_num;
	char portname[MAX_PATH];

	serial = new CSerialPort(); 

	if(serial == NULL){
		printf("SerialPort initialization failed.");
		return;
	}
	int t=0;
	while(1){//t!=20

		for (com_num=0; com_num<6; com_num++){

			if(not_connect[com_num]){
				sprintf (portname, "COM%d", com_num+3);	
				//printf(" %s\n" , portname);   
				if(connect(portname, 1200,serial)){   
					not_connect[com_num]=false;
					printf("FIND PORT ! : %s\n" , portname);
					disconnect(serial);
					if(!run(portname))
						printf("Create Thread FAIL !! \n");
				}
			}
		}
		//Sleep(1000);
		//t++;
	}
	delete serial;
}



int CSerialComm::run(char *portname)
{   
	thread_p = new thread_parameter;	

	pthread_t tid;

	thread_p->serialcomm_adr = (void*)this;

	memcpy(thread_p->portname,portname,sizeof(portname)+1);


	if(pthread_create(	&tid,
		NULL,
		connect_thread,
		(void*)thread_p) != 0){
			return RETURN_FAIL;
	}
	else {

		return RETURN_SUCCESS;

	}
}



extern "C" void* connect_thread(void* args)
{
	CSerialComm * serialcomm;
	thread_parameter *thr_p=(thread_parameter*)args;



	serialcomm = (CSerialComm*)thr_p->serialcomm_adr;   
	serialcomm->recv_portname(thr_p->portname); //args


	return NULL;
}


void CSerialComm::recv_portname(char* portname)//void* serial_type
{   
	CSerialPort*   serial_th;
	serial_th = new CSerialPort();  //serialport 클래스 개인화


	char buff[4000];
	int bps = 1200;

	//aditional 11.30
	bool connect_done=false;

	for(int j=0;j<=5;j++){
		memset(buff,NULL,sizeof(buff));
		if(connect(portname,bps,serial_th))

			recv(buff,serial_th);
		//printf(" %s 의 %d에서의  recv(buff) : %s\n",portname,bps,buff);

		//GPS
		if(strstr(buff,"GP") != NULL){
			printf("======Conncet GPS PORT======\n");
			disconnect(serial_th);
			connect_done= !connect_done;
			if(!connect_port(portname,bps,serial_th)){
				printf("======DISConncet GPS [%s]======\n",portname); break; }        
		}

		//AIS
		else if(strstr(buff,"AI") != NULL){
			printf("======Conncet AIS PORT======\n");
			disconnect(serial_th);
			connect_done= !connect_done;
			if(!connect_port(portname,bps,serial_th)){
				printf("======DISConncet AIS [%s]======\n",portname);break; }
		}
		bps *= 2;
		disconnect(serial_th);

	}
	if(!connect_done)
		printf("======NOT Declare Sentence======\n");

	not_connect[atoi(portname+3)-3]=true;

}

int CSerialComm::connect_port(char* portname,int bps,CSerialPort*  serialport)  // 수신 부분 
{
	char recv_buff[4000];  //ReadFile(m_hComm, buff, 4000, &dwBytesTransferred, 0) 의  3번째 인자와 같은 크기로 권고
	char *token;
	char Distig[] = "\n";
	char tag[128] = "\s:tt456*7e\\"; // tag_block
	char send_buff[1024];
	const char * sentence_pointer;




	while(connect(portname,bps,serialport)){

		pthread_mutex_lock(&m_mutex);
		memset(recv_buff,NULL,sizeof(recv_buff));
		//delete thread_p;  //필요없을것 같아서 지움      # 오류 생길수도 ! 
		recv(recv_buff,serialport);

		// printf("[%s]\n%s\n",portname,recv_buff); 


		token = strtok(recv_buff,Distig);

		while (token != NULL){   
			strcpy(send_buff,token);                     
			//strcat(send_buff,token); 
			//printf("[%s]\n%s\n",portname,send_buff);
			//api->sendSentence(2,send_buff,sizeof(send_buff));
			//sentence_pointer = strstr(send_buff,"GLL");

			/* if(Parse_GLL(token))
			sqlsin();*/


			if(Parse_VDM(token))
			{
				//printf("%s\n",E);

				VDM_parse(E);
				

					
			} 


			token = strtok (NULL,Distig);
			memset(send_buff,0,sizeof(send_buff));

		}

		pthread_mutex_unlock(&m_mutex);

		disconnect(serialport);   
	}

	delete serialport;

	return RETURN_FAIL;
}






int CSerialComm::connect(char* portNum, int bps,CSerialPort*  serialport ) ///
{     

	if(!serialport->OpenPort(portNum))///
		return RETURN_FAIL;

	if(!serialport->ConfigurePort(bps, 8, FALSE, NOPARITY, ONESTOPBIT))
		return RETURN_FAIL; 
	if(!serialport->SetCommunicationTimeouts(150, 0, bps, 0, 1)) // 수정   첫번째 인자 수도 중요!
		return RETURN_FAIL;

	return RETURN_SUCCESS;
}


int CSerialComm::sendCommand(unsigned char pos)
{
	if(serial->WriteByte(pos))
		return RETURN_SUCCESS;
	else
		return RETURN_FAIL;
}

void CSerialComm::disconnect(CSerialPort* serialport) //
{    	

	serialport->ClosePort();  //
}

int CSerialComm::recv(char* buff,CSerialPort*  serialport)
{
	if(serialport->ReadByte(buff)) //
		return RETURN_SUCCESS;
	else
		return RETURN_FAIL;
}


void CSerialComm::sqlsin()
{
	SQLHENV hEnv;
	SQLHDBC hDbc;
	SQLHSTMT hStmt;
	SQLRETURN ret = SQL_SUCCESS; // SQL 리턴값
	SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &hEnv);
	SQLSetEnvAttr(hEnv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, SQL_IS_INTEGER);
	SQLAllocHandle(SQL_HANDLE_DBC, hEnv, &hDbc); // 데이터베이스 핸들
	ret = SQLConnect(hDbc, (SQLCHAR *)"CYBERMARINE", SQL_NTS, (SQLCHAR *) "sa", SQL_NTS, (SQLCHAR *)"20170405", SQL_NTS); // DNS,ID,PW 데이터베이스 로그인
	ret = SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt);
	char query[1024]; // 질의을 담을 버퍼

	if((ret !=SQL_SUCCESS) && (ret != SQL_SUCCESS_WITH_INFO)) { printf("CONNECTION FAIL\n"); }
	else { printf("[ DATABSE CONNECTION "); }

	sprintf_s(query, 1000,"INSERT INTO GPSINFO1(LATITUE,RMR,LONGITUDE,EAST,UTC,STATUS)" "VALUES('%s','%s','%s','%s','%s','%s')",LATITUE,RMR,LONGITUDE,EAST,UTC,STATUS);
	SQLExecDirect(hStmt, (SQLCHAR *) query, SQL_NTS);

	if((SQLExecDirect(hStmt, (SQLCHAR *) query, SQL_NTS)) == 0)
	{ printf("AND STORED SUCCESS ]\n"); }
	else{ printf("BUT STORED FAIL ]\n"); }

	if(hStmt)SQLFreeHandle(SQL_HANDLE_STMT,hStmt);
	if(hDbc)SQLDisconnect(hDbc);
	if(hDbc)SQLFreeHandle(SQL_HANDLE_DBC, hDbc);
	if(hDbc)SQLFreeHandle(SQL_HANDLE_ENV, hDbc);
}

void CSerialComm::DB_M1to3()
{
	SQLHENV hEnv;
	SQLHDBC hDbc;
	SQLHSTMT hStmt;
	SQLRETURN ret = SQL_SUCCESS; // SQL 리턴값
	SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &hEnv);
	SQLSetEnvAttr(hEnv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, SQL_IS_INTEGER);
	SQLAllocHandle(SQL_HANDLE_DBC, hEnv, &hDbc); // 데이터베이스 핸들
	ret = SQLConnect(hDbc, (SQLCHAR *)"CYBERMARINE", SQL_NTS, (SQLCHAR *) "sa", SQL_NTS, (SQLCHAR *)"20170405", SQL_NTS); // DNS,ID,PW 데이터베이스 로그인
	ret = SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt);
	char query[1024]; // 질의을 담을 버퍼
	char time_char[25];
	if((ret !=SQL_SUCCESS) && (ret != SQL_SUCCESS_WITH_INFO)) { printf("CONNECTION FAIL\n"); }
	else { printf("[ DATABSE CONNECTION "); }

	time_t t = time(NULL);
  struct tm tm = *localtime(&t);
 
  
  sprintf(time_char,"'%d-%d-%d %d:%d:%d'",tm.tm_year+1900, tm.tm_mon+1, tm.tm_mday,tm.tm_hour, tm.tm_min, tm.tm_sec);
	sprintf_s(query, 1000,"INSERT INTO AISDB02(Total_Number_Of_Sentences,Sentence_number,Sequential_message_identifier,AIS_channel,Message_ID,Repeat_Indicator,MMSI_NUMBER,Navigational_Status,ROT,SOG,Position_accuracy,Longitude,Latitude,COG,True_heading,Time_stamp,Regional_Application,Spare,RAIM_Flag,COM_UTC_Direct,COM_Frame,COM_UTC_Bit1,COM_UTC_Bit2,COM_Submessage,time_char)"
		"VALUES(%ld,%ld,%ld,%s,%lf,%lf,%d,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%s)",
		A,B,C,D,filed[0],filed[1],(int)filed[2],filed[3],filed[4],filed[5],filed[6],filed[7],filed[8],filed[9],filed[10],filed[11],
		filed[12],filed[13],filed[14],filed[15],filed[16],filed[17],filed[18],filed[19],time_char);
	
	//SQLExecDirect(hStmt, (SQLCHAR *) query, SQL_NTS);
	if((SQLExecDirect(hStmt, (SQLCHAR *) query, SQL_NTS)) == 0)
	{ printf("==DBAIS02==AND STORED SUCCESS ]\n"); }
	else{ printf("BUT STORED FAIL ]\n"); }

	if(hStmt)SQLFreeHandle(SQL_HANDLE_STMT,hStmt);
	if(hDbc)SQLDisconnect(hDbc);
	if(hDbc)SQLFreeHandle(SQL_HANDLE_DBC, hDbc);
	if(hDbc)SQLFreeHandle(SQL_HANDLE_ENV, hDbc);
}


void CSerialComm::DB_M5()
{
	SQLHENV hEnv;
	SQLHDBC hDbc;
	SQLHSTMT hStmt;
	SQLRETURN ret = SQL_SUCCESS; // SQL 리턴값
	SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &hEnv);
	SQLSetEnvAttr(hEnv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, SQL_IS_INTEGER);
	SQLAllocHandle(SQL_HANDLE_DBC, hEnv, &hDbc); // 데이터베이스 핸들
	ret = SQLConnect(hDbc, (SQLCHAR *)"CYBERMARINE", SQL_NTS, (SQLCHAR *) "sa", SQL_NTS, (SQLCHAR *)"20170405", SQL_NTS); // DNS,ID,PW 데이터베이스 로그인
	ret = SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt);
	char query[1024]; // 질의을 담을 버퍼

	if((ret !=SQL_SUCCESS) && (ret != SQL_SUCCESS_WITH_INFO)) { printf("CONNECTION FAIL\n"); }
	else { printf("[ DATABSE CONNECTION "); }

	sprintf_s(query, 1000,"UPDATE NAME03 SET MMSI_NUMBER=%d,SHIP_NAME=%s WHERE MMSI_NUMBER=%d",
		user_id,ascii,user_id);
	//SQLExecDirect(hStmt, (SQLCHAR *) query, SQL_NTS);
	if((SQLExecDirect(hStmt, (SQLCHAR *) query, SQL_NTS)) == 0)
	{ ; }
	else
	{
		
		sprintf_s(query, 1000,"INSERT INTO NAME03(MMSI_NUMBER,SHIP_NAME)" 
		"VALUES(%d,%s)",user_id,ascii);
		SQLExecDirect(hStmt, (SQLCHAR *) query, SQL_NTS); 
	}

	if(hStmt)SQLFreeHandle(SQL_HANDLE_STMT,hStmt);
	if(hDbc)SQLDisconnect(hDbc);
	if(hDbc)SQLFreeHandle(SQL_HANDLE_DBC, hDbc);
	if(hDbc)SQLFreeHandle(SQL_HANDLE_ENV, hDbc);
}






bool  CSerialComm::M1to3(unsigned char* sentence)
{			

	//printf("1to3");
	unsigned char *st= sentence; ///////////////////////////////+1
	bool bit[168];
	int n, i=0,bit_ad=0,ret;////////////////////
	int filed_bit[20] = { 6,2,30,4,8,10,1,28,27,12,9,6,4,1,1,2,3,5,7,2};//////////////////
	// double filed[20];


	// string  in to  6 bit row
	while (st[i] != '\0') {

		st[i] += 40;
		if (st[i] > 128)
			st[i] += 32;
		else
			st[i] += 40;

		n = 5;
		while (n != -1) {
			bit[bit_ad] = (st[i] >> n) & 1;
			n--;
			bit_ad++;
		}
		i++;      
	}

	// filed parse
	i = 0; n = 0; bit_ad = 0;
	while (n != 20) {//////////////////////////
		ret = 0; bit_ad += filed_bit[n];
		while (i != bit_ad) {
			if (bit[i])
				ret += pow((double)2, bit_ad - i -1);
			// printf("%d", bit[i]);
			i++;

		}
		// printf("\n");
		filed[n] = ret;

		n++;
	}




	n = 0;
	if(filed[4]>128)
	{
		filed[4]--;
		filed[4]=(int)filed[4]^255;
		filed[4]=(-1)*(filed[4]/4.733)*(filed[4]/4.733);
	}
	else if(filed[4]==128)
	{
		filed[4]=-128;
	}
	else 
	{

		filed[4]=(filed[4]/4.733)*(filed[4]/4.733);
	}

	if(filed[7]>134217728)
	{
		filed[7]--;
		filed[7]=(int)filed[7]^268435455;
		filed[7]=(-1)*filed[7]/600000;
	}
	else 
		filed[7]=filed[7]/600000;


	if(filed[8]>67108864)
	{
		
		filed[8]--;
		filed[8]=(int)filed[8]^134217727;
		filed[8]=(-1)*filed[8]/600000;
	}
	else 
		filed[8]=filed[8]/600000;


	filed[5]=filed[5]*0.1;
	filed[9]=filed[9]*0.1;

	/*
	printf("-------------------------------------------------유의미성------------------------------------------- \n");
	printf("Message ID : %f\n",filed[0]); //Message types 의 타입을 말해준다 예를들어 위치보고 메시지인지 안전관련 메시지인지 등등~
	printf("Repeat indicator : %f\n",filed[1]);//메시지가 반복 된 횟수를 나타 내기 위해 repeater에서 사용합니다
	printf("User ID : %f\n",filed[2]); //MMSI number  즉 어떤배인지 설명해주는거 배or SAR항공기인지
	printf("Navigational status : %f\n",filed[3]);//운항상태 즉 정박중인지 항해중인지
	printf("ROT[AIS] : %.1f degree/minute \n",filed[4]=(filed[4]/4.733)*(filed[4]/4.733));//선박 의 선회율 즉 선박이 돌고있는 속도를 나타냅니다. 배가 분당도로 돌고있는 비율을 나타냅니다.
	printf("SOG : %.1f knots\n",filed[5]=filed[5]*0.1); //배의 속도를 나타낸다
	printf("Position accuracy : %f\n",filed[6]); //1 = 하이 (<10m; DGNSS 수신기의 차동 모드) 0 = 로우 (> 10m; 예를 들어 GNSS 수신기 또는 다른 전자 위치 고정 장치의 자율 모드); 기본값 = 0
	printf("Longitude : %f degrees 5minute\n",filed[7]=filed[7]/600000);//경도/////////// 5minute는 왜그런지 잘 모르겟다
	printf("Latitude : %f degrees 5minute\n",filed[8]=filed[8]/600000);//위도///////////5minute는 왜그런지 잘 모르겟다
	printf("COG : %.1f degrees true\n",filed[9]=filed[9]*0.1);// 지구 표면과 관련하여 두 지점 사이의 선박 진행의 실제 방향입니다. 
	printf("True heading : %f degrees true\n",filed[10]);//선박이가리키는 방향을 나타내는 네비게이션에 사용되는 용어입니다.실제로 운항하는 방향 일 수도 있고 아닐 수도 있습니다.코스 또는 트랙으로 알려져 있습니다. 
	printf("Time stamp : %f second past the minute\n",filed[11]);//UTC 보고가 발생됬을때의 시간
	printf("Reserved for regional applications : %f\n",filed[12]);//관할 지역 당국의 정의를 위해 예약 됨. 지역 응용 프로그램에 사용되지 않는 경우 0으로 설정해야합니다. 지역별 응용 프로그램에서는 0을 사용하지 않아야합니다.
	printf("Spare : %f\n",filed[13]);//Not used. Should be set to zero 
	printf("RAIM-Flag : %f\n",filed[14]);//전자 위치 고정 장치의 RAI 플래그; 0 = 사용 중이 아닌 RAIM = 기본값; 1 = 사용중인 RAIM
	*/
	//=============================================================
	// printf("Communication State : \n"); //communication state 4등분
	// printf("1. UTC Direct:");

	/*
	for(int j=149;j<151;j++) 
	printf("%d",bit[j]);
	printf("\n");
	printf("2. Frames:");
	for(int j=151; j<154;j++)
	printf("%d",bit[j]);
	printf("\n");
	*/
	int a=0,b=0;
	for(int j=154;j<159;j++)//앞에 5bit 10진수
	{
		if (bit[j])
			a += pow((double)2, 158 - j);
	}
	for(int j=159;j<166;j++)//뒤에 7bit 10진수
	{
		if (bit[j])
			b += pow((double)2, 165 - j );
	}

	//printf("3.%d:%d UTC\n",a,b);
	// printf("4.Sub-Message %d",filed[19]);
	//=========================================================
	DB_M1to3();
	DB_realtime();
	return SUCCESS;

}
bool  CSerialComm::M5(unsigned char* sentence)
{
	
	// unsigned char st[100] = "56Sej44000037?33KB06ltpF222222222222221@4pC5336g07PUDhCP0000"; ///////////////////////////////+1
	unsigned char *st= sentence;
	bool bit[600];
	int n, i = 0, bit_ad = 0, ret;////////////////////
	char temp_ascii[21];

	// string  in to  6 bit row
	while (st[i] != '\0') {
		st[i] += 40;
		if (st[i] > 128)
			st[i] += 32;
		else
			st[i] += 40;
		n = 5;
		while (n != -1) {
			bit[bit_ad] = (st[i] >> n) & 1;
			n--;
			bit_ad++;
		}
		i++;
	}

	//User_id
	ret = 0; bit_ad = 38; i = 8;
	while (i != bit_ad) {
		if (bit[i])
			ret += pow((double)2, bit_ad - i - 1);
		i++;
	}
	user_id = ret;


	//vessel_name
	n = 0; bit_ad = 112;
	while (n != 20) {    
		i = 0; ret = 0;
		while (i != 6) {
			if (bit[bit_ad])
				ret += pow((double)2, 5 - i);
			i++;  bit_ad++;
		}
		if (ret < 32)
			temp_ascii[n] = ret + 64;
		else
			temp_ascii[n] = ret;
		n++;

		
	}
	temp_ascii[20] = '\0';

	
	sprintf((char*)ascii,"'%s'",temp_ascii);
	
	DB_M5();
	return SUCCESS;

}



bool CSerialComm::VDM_parse(unsigned char* sentence)
{
	unsigned char *st= sentence; ///////////////////////////////+1




	//printf("%c\n",*sentence);
	if(*sentence=='1')
		return M1to3(sentence);
	else if(*sentence=='2')
		return M1to3(sentence);
	else if(*sentence=='3')
		return M1to3(sentence);
	else if(*sentence=='5')
		return M5(sentence);
	else
		return FAIL;


}


void CSerialComm::DB_realtime()
{
	
	SQLHENV hEnv;
	SQLHDBC hDbc;
	SQLHSTMT hStmt;
	SQLRETURN ret = SQL_SUCCESS; // SQL 리턴값
	SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &hEnv);
	SQLSetEnvAttr(hEnv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, SQL_IS_INTEGER);
	SQLAllocHandle(SQL_HANDLE_DBC, hEnv, &hDbc); // 데이터베이스 핸들
	ret = SQLConnect(hDbc, (SQLCHAR *)"CYBERMARINE", SQL_NTS, (SQLCHAR *) "sa", SQL_NTS, (SQLCHAR *)"20170405", SQL_NTS); // DNS,ID,PW 데이터베이스 로그인
	ret = SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt);
	char query[1024]; // 질의을 담을 버퍼
	//char temp[12];
	//itoa((int)filed[2],temp,10);
	//printf("변한거 : %s\n", temp);
	if((ret !=SQL_SUCCESS) && (ret != SQL_SUCCESS_WITH_INFO)) { printf("CONNECTION FAIL\n"); }
	else { printf("[ DATABSE CONNECTION "); }

	// 업데이트문에  SET  필드마다  ',' 붙여야함
	sprintf_s(query, 1000,"UPDATE REALTIME_VDM\nSET Latitude=%lf,Longitude=%lf, MMSI_NUMBER=%d, SOG=%lf\nWHERE MMSI_NUMBER=%d",filed[8],filed[7],(int)filed[2],filed[5],(int)filed[2]);
	//SQLExecDirect(hStmt, (SQLCHAR *) query, SQL_NTS);
	if((SQLExecDirect(hStmt, (SQLCHAR *) query, SQL_NTS)) == 0)
		;
	//printf("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@REALTIME_VDM 두번째@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n");

	else{
		
		sprintf_s(query, 1000,"INSERT INTO REALTIME_VDM(Latitude,Longitude,MMSI_NUMBER,SOG) VALUES(%lf,%lf,%d,%lf)",filed[8],filed[7],(int)filed[2],filed[5]);
	
	if((SQLExecDirect(hStmt, (SQLCHAR *) query, SQL_NTS)) == 0)
	            ;
		//printf("=============================REALTIME_VDM 처음들어옴============================================\n");

	else 
		;
		//printf("GGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGG\n");
	
	}
	if(filed[8]<=35.0681||filed[7]>=129.12632)
	{
		sprintf_s(query, 1000,"DELETE FROM REALTIME_VDM WHERE MMSI_NUMBER=%d",(int)filed[2]);
	
	if((SQLExecDirect(hStmt, (SQLCHAR *) query, SQL_NTS)) == 0)
	printf("현재상태 항구에서 벗어남\n");
	
	}
	if(hStmt)SQLFreeHandle(SQL_HANDLE_STMT,hStmt);
	if(hDbc)SQLDisconnect(hDbc);
	if(hDbc)SQLFreeHandle(SQL_HANDLE_DBC, hDbc);
	if(hDbc)SQLFreeHandle(SQL_HANDLE_ENV, hDbc);

	/*
	(Total_Number_Of_Sentences,Sentence_number,Sequential_message_identifier,AIS_channel,Message_ID,Repeat_Indicator,MMSI_NUMBER,Navigational_Status,ROT,SOG,Position_accuracy,Longitude,Latitude,COG,True_heading,Time_stamp,Regional_Application,Spare,RAIM_Flag,COM_UTC_Direct,COM_Frame,COM_UTC_Bit1,COM_UTC_Bit2,COM_Submessage)" 
		"VALUES(%ld,%ld,%ld,%s,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf)",
		A,B,C,D,filed[0],filed[1],filed[2],filed[3],filed[4],filed[5],filed[6],filed[7],filed[8],filed[9],filed[10],filed[11],
		filed[12],filed[13],filed[14],filed[15],filed[16],filed[17],filed[18],filed[19]);
	*/
}



/*
void CSerialComm::DB_realtime()
{
	SQLHENV hEnv;
	SQLHDBC hDbc;
	SQLHSTMT hStmt;
	SQLRETURN ret = SQL_SUCCESS; // SQL 리턴값
	SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &hEnv);
	SQLSetEnvAttr(hEnv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, SQL_IS_INTEGER);
	SQLAllocHandle(SQL_HANDLE_DBC, hEnv, &hDbc); // 데이터베이스 핸들
	ret = SQLConnect(hDbc, (SQLCHAR *)"CYBERMARINE", SQL_NTS, (SQLCHAR *) "sa", SQL_NTS, (SQLCHAR *)"20170405", SQL_NTS); // DNS,ID,PW 데이터베이스 로그인
	ret = SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt);
	char query[1024]; // 질의을 담을 버퍼
	//char temp[12];
	//itoa((int)filed[2],temp,10);
	//printf("변한거 : %s\n", temp);
	if((ret !=SQL_SUCCESS) && (ret != SQL_SUCCESS_WITH_INFO)) { printf("CONNECTION FAIL\n"); }
	else { printf("[ DATABSE CONNECTION "); }


	sprintf_s(query, 1000,"UPDATE REALTIMEAISDB" 
		"\nSET  Total_Number_Of_Sentences = %ld,"
		"Sentence_number = %ld, "
		"Sequential_message_identifier=%ld,"
		"AIS_channel=%s,"
		"Message_ID=%lf,"
		"Repeat_Indicator=%lf,"
		"MMSI_NUMBER=%lf,"
		"Navigational_Status=%lf,"
		"ROT=%lf,"
		"SOG=%lf,"
		"Position_accuracy=%lf,"
		"Longitude=%lf,"
		"Latitude=%lf,"
		"COG=%lf,"
		"True_heading=%lf,"
		"Time_stamp=%lf,"
		"Regional_Application=%lf,"
		"Spare=%lf,"
		"RAIM_Flag=%lf,"
		"COM_UTC_Direct=%lf,"
		"COM_Frame=%lf,"
		"COM_UTC_Bit1=%lf,"
		"COM_UTC_Bit2=%lf,"
		"COM_Submessage=%lf"
		"\nWHERE MMSI_NUMBER=%lf",
		A,B,C,D,filed[0],filed[1],filed[2],filed[3],filed[4],filed[5],filed[6],filed[7],filed[8],filed[9],filed[10],filed[11],
		filed[12],filed[13],filed[14],filed[15],filed[16],filed[17],filed[18],filed[19],filed[2]);
	SQLExecDirect(hStmt, (SQLCHAR *) query, SQL_NTS);
	if((SQLExecDirect(hStmt, (SQLCHAR *) query, SQL_NTS)) == 0)
	{ printf("AND STORED SUCCESS ]\n"); }
	else{
		
		sprintf_s(query, 1000,"INSERT INTO REALTIMEAISDB(Total_Number_Of_Sentences,Sentence_number,Sequential_message_identifier,AIS_channel,Message_ID,Repeat_Indicator,MMSI_NUMBER,Navigational_Status,ROT,SOG,Position_accuracy,Longitude,Latitude,COG,True_heading,Time_stamp,Regional_Application,Spare,RAIM_Flag,COM_UTC_Direct,COM_Frame,COM_UTC_Bit1,COM_UTC_Bit2,COM_Submessage)"
		"VALUES(%ld,%ld,%ld,%s,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf)",
		A,B,C,D,filed[0],filed[1],filed[2],filed[3],filed[4],filed[5],filed[6],filed[7],filed[8],filed[9],filed[10],filed[11],
		filed[12],filed[13],filed[14],filed[15],filed[16],filed[17],filed[18],filed[19]);
	SQLExecDirect(hStmt, (SQLCHAR *) query, SQL_NTS);
	if((SQLExecDirect(hStmt, (SQLCHAR *) query, SQL_NTS)) == 0)
	{ printf("야!\n"); }
	else printf("지\n");
	
	}

	if(hStmt)SQLFreeHandle(SQL_HANDLE_STMT,hStmt);
	if(hDbc)SQLDisconnect(hDbc);
	if(hDbc)SQLFreeHandle(SQL_HANDLE_DBC, hDbc);
	if(hDbc)SQLFreeHandle(SQL_HANDLE_ENV, hDbc);

	/*
	(Total_Number_Of_Sentences,Sentence_number,Sequential_message_identifier,AIS_channel,Message_ID,Repeat_Indicator,MMSI_NUMBER,Navigational_Status,ROT,SOG,Position_accuracy,Longitude,Latitude,COG,True_heading,Time_stamp,Regional_Application,Spare,RAIM_Flag,COM_UTC_Direct,COM_Frame,COM_UTC_Bit1,COM_UTC_Bit2,COM_Submessage)" 
		"VALUES(%ld,%ld,%ld,%s,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf)",
		A,B,C,D,filed[0],filed[1],filed[2],filed[3],filed[4],filed[5],filed[6],filed[7],filed[8],filed[9],filed[10],filed[11],
		filed[12],filed[13],filed[14],filed[15],filed[16],filed[17],filed[18],filed[19]);
	
}*/
