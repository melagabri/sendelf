#include "StdAfx.h"
#include "ComConnection.h"

#define FTDI_PACKET_SIZE 3968

CComConnection::CComConnection(void) : gecko(INVALID_HANDLE_VALUE)
{
	hInst = LoadLibrary(L"ftd2xx.dll");
	if (hInst == NULL) {
		throw TEXT("Cannot load the required dll's for USB Gecko.");
	}

	// Find the functions
	FT_Open = (Func_FT_Open) GetProcAddress(hInst, "FT_Open");
	FT_GetComPortNumber = (Func_FT_GetComPortNumber) GetProcAddress(hInst, "FT_GetComPortNumber");
	FT_Close = (Func_FT_Close) GetProcAddress(hInst, "FT_Close");
}

CComConnection::~CComConnection(void)
{
	if (gecko != INVALID_HANDLE_VALUE) {
		CloseHandle(gecko);
	}

	if (hInst != NULL) {
		FreeLibrary(hInst);
	}
}

void CComConnection::Connect()
{
	HANDLE ftHandle;
	ULONG res = FT_Open(0, &ftHandle);
	if (res != 0) {
		throw TEXT("Cannot start driver needed for USB Gecko");
	}

	LONG portnumber;
	res = FT_GetComPortNumber(ftHandle, &portnumber);
	FT_Close(ftHandle);

	if (res != 0) {
		throw TEXT("Cannot find correct COM portnumber");
	}

	TCHAR comport[5];
	wsprintf(comport, L"COM%d", portnumber);
	
	gecko = CreateFile(comport, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (gecko == INVALID_HANDLE_VALUE) {
		throw TEXT("Cannot open port");
	}
	
	DCB dcb;
	GetCommState(gecko, &dcb);
	dcb.BaudRate = 115200;
	dcb.ByteSize = 8;
	dcb.Parity = NOPARITY;
	dcb.StopBits = ONESTOPBIT;
	SetCommState(gecko, &dcb);

	COMMTIMEOUTS timeouts;
	GetCommTimeouts(gecko, &timeouts);
	memset(&timeouts, 0, sizeof(COMMTIMEOUTS));
	timeouts.ReadIntervalTimeout = MAXDWORD;

	if (!SetCommTimeouts(gecko, &timeouts)) {
		throw TEXT("Cannot set timeouts on port.");
	}
	if (!SetCommMask(gecko, 0)) {
		throw TEXT("Cannot set event mask on port.");
	}
	PurgeComm(gecko, PURGE_RXCLEAR | PURGE_TXCLEAR | PURGE_TXABORT | PURGE_RXABORT);
}

void CComConnection::Send(const char *buffer, int length)
{
	DWORD res;

	int left = length;

	while (left) {
		int chunk = left;
		if (chunk > FTDI_PACKET_SIZE)
			chunk = FTDI_PACKET_SIZE;
		if (!WriteFile(gecko, buffer, chunk, &res, NULL)) {
			throw TEXT("Error while writing to USB Gecko");
		}
		left -= res;
		buffer += res;
	}
}

void CComConnection::Disconnect()
{
	if (gecko != INVALID_HANDLE_VALUE) {
		CloseHandle(gecko);
		gecko = INVALID_HANDLE_VALUE;
	}
}
