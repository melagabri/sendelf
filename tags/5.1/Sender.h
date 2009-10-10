#pragma once

#include "Connection.h"

class CSender
{
public:
	CSender();
	~CSender(void);

	void SetConnection(CConnection *conn);
	void SetSource(TCHAR *filename);
	void SetArguments(TCHAR *argv);

	int GetFilesize();
	void * GetFileContents();

	virtual void SendHeaders() = 0;
	virtual void SendFile();
	virtual void SendArguments();

	virtual void Send();
protected:
	void Send(const char *buffer, int length);
	PWCHAR* ParseCommandLineArgs(PWCHAR CmdLine, int* _argc);
	int EndianFlip(int value);

	CConnection *connection;

	int filesize;
	int argumentcount;
	char *arguments;
	int argumentsLength;
	TCHAR *filename;
	HANDLE fileHandle;
	void *fileContent;
};
