#include <stdio.h>
#include "serialcomm.h"


char* input(){
char* serial_type;
	
	serial_type = new char[50];
	printf("input : <PORT NAME> <bps>\n");
	
	
	scanf("%[^\n]s", serial_type);
	    printf("¿©±â : %s \n",serial_type);
		return serial_type;
}

int main()
{   
	int i = 0;
	CSerialComm* serialComm;
	serialComm  = new CSerialComm();
	
	
		serialComm->find_port();
		
    }

// STEP 1. SerialPort Connect
	// STEP 2. Send Command
	
	// STEP 3. SerialPort disConnect
	//serialComm.disconnect();

	/*printf("\nend connect\n");
		return 0;*/




