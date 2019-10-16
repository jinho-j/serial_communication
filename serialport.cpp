#include "serialport.h"
CSerialPort::CSerialPort(){}  //printf("serialport ���� \n");

CSerialPort::~CSerialPort(){}  

bool CSerialPort::OpenPort(CString portname)  
{    
	m_hComm = CreateFile(/*L"//./" + */portname,  
		GENERIC_READ | GENERIC_WRITE,  
		0,  
		0,  
		OPEN_EXISTING,  
		0,  
		0);
	
	if(m_hComm == INVALID_HANDLE_VALUE)  
	{  
		return false;  
	}  
	else  
		return true;  
}  

bool CSerialPort::ConfigurePort(DWORD BaudRate, BYTE ByteSize, DWORD fParity,   
	BYTE Parity, BYTE StopBits)  
{  
	if((m_bPortReady = GetCommState(m_hComm, &m_dcb))==0)  
	{  
		printf("\nGetCommState Error\n");
		//"MessageBox(L, L"Error", MB_OK + MB_ICONERROR);  
		CloseHandle(m_hComm);  
		return false;  
	}  

	m_dcb.BaudRate          = BaudRate;  
	m_dcb.ByteSize          = ByteSize;  
	m_dcb.Parity            = Parity ;  
	m_dcb.StopBits          = StopBits;  
	m_dcb.fBinary           = true;  
	m_dcb.fDsrSensitivity   = false;  
	m_dcb.fParity           = fParity;  
	m_dcb.fOutX             = false;  
	m_dcb.fInX              = false;  
	m_dcb.fNull             = false;  
	m_dcb.fAbortOnError     = false;   //����
	m_dcb.fOutxCtsFlow      = false;  
	m_dcb.fOutxDsrFlow      = false;  
	m_dcb.fDtrControl       = DTR_CONTROL_DISABLE;  
	m_dcb.fDsrSensitivity   = false;  
	m_dcb.fRtsControl       = RTS_CONTROL_DISABLE;  
	m_dcb.fOutxCtsFlow      = false;  
	m_dcb.fOutxCtsFlow      = false;  
	m_dcb.fTXContinueOnXoff = true;		 // Continue TX when Xoff sent
	m_dcb.fErrorChar		= false;	 // Enable Err Replacement
	m_dcb.ErrorChar			= 0;		 // Error replacement char
	m_dcb.EofChar			= 0;		 // End of Input character
	m_dcb.EvtChar			= 0;		 // Received Event character


	m_bPortReady = SetCommState(m_hComm, &m_dcb);  

	if(m_bPortReady == 0)  
	{  
		//MessageBox(L"SetCommState Error");  
		printf("SetCommState Error");

		CloseHandle(m_hComm);  
		return false;  
	}  

	return true;  
}  

bool CSerialPort::SetCommunicationTimeouts(DWORD ReadIntervalTimeout,  
	DWORD ReadTotalTimeoutMultiplier, DWORD ReadTotalTimeoutConstant,  
	DWORD WriteTotalTimeoutMultiplier, DWORD WriteTotalTimeoutConstant)  
{  
	if((m_bPortReady = GetCommTimeouts(m_hComm, &m_CommTimeouts)) == 0)  
		return false;  

	m_CommTimeouts.ReadIntervalTimeout          = ReadIntervalTimeout;  
	m_CommTimeouts.ReadTotalTimeoutConstant     = ReadTotalTimeoutConstant;  
	m_CommTimeouts.ReadTotalTimeoutMultiplier   = ReadTotalTimeoutMultiplier;  
	m_CommTimeouts.WriteTotalTimeoutConstant    = WriteTotalTimeoutConstant;  
	m_CommTimeouts.WriteTotalTimeoutMultiplier  = WriteTotalTimeoutMultiplier;  

	m_bPortReady = SetCommTimeouts(m_hComm, &m_CommTimeouts);  

	if(m_bPortReady == 0)  
	{  
		//MessageBox(L"StCommTimeouts function failed",L"Com Port Error",MB_OK+MB_ICONERROR);  
		printf("\nStCommTimeouts function failed\n");
		CloseHandle(m_hComm);  
		return false;  
	}  

	return true;  
}  

bool CSerialPort::WriteByte(BYTE bybyte)  
{  
	//iBytesWritten=0;
	//m_iBytesWritten=0;  
	if(WriteFile(m_hComm, &bybyte, 1, &m_iBytesWritten, NULL) == 0)  
		return false;  
	else  
		return true;  
}  

bool CSerialPort::ReadByte(BYTE &resp)  
{  
	BYTE rx;  
	resp=0;  

	DWORD dwBytesTransferred=0;  

	if(ReadFile(m_hComm, &rx, 1, &dwBytesTransferred, 0))  
	{  
		if(dwBytesTransferred == 1)  
		{  
			resp=rx;  
			return true;  
		}  
	}  

	return false;  
}

bool CSerialPort::ReadByte(char *buff)  
{  

	DWORD dwBytesTransferred=0;  

	if(ReadFile(m_hComm, buff, 4000, &dwBytesTransferred, 0))  
	{  
		if(dwBytesTransferred > 0)  
		{  			 
			buff[dwBytesTransferred] = '\0'; // ����

			ToAscii7bits(buff,dwBytesTransferred);

			return true;  
		}  
	}  

	return false;  
} 

bool CSerialPort::ReadByte(BYTE* &resp, UINT size)  
{  
	DWORD dwBytesTransferred=0;  

	if(ReadFile(m_hComm, resp, size, &dwBytesTransferred, 0))  
	{  
		if(dwBytesTransferred == size)  
			return true;  
	}  

	return false;  
}  

void CSerialPort::ClosePort()  
{  
	CloseHandle(m_hComm);
	
	return;  
}  