#pragma once
#include "connection.h"

typedef ULONG (__stdcall *Func_FT_Open) (int deviceNumber, HANDLE *handle);
typedef ULONG (__stdcall *Func_FT_GetComPortNumber) (HANDLE handle, LPLONG portnumber);
typedef ULONG (__stdcall *Func_FT_Close) (HANDLE handle);

class CComConnection :
	public CConnection
{
public:
	CComConnection(void);
	~CComConnection(void);

	void Connect();
	void Send(const char *buffer, int length);
	void Disconnect();
private:
	HANDLE gecko;

	Func_FT_Open FT_Open;
	Func_FT_GetComPortNumber FT_GetComPortNumber;
	Func_FT_Close FT_Close;
};
