#include "StdAfx.h"
#include "Sender.h"

CSender::CSender() : fileHandle(NULL), filename(NULL), fileContent(NULL), 
					 argumentcount(0), arguments(NULL), filesize(-1), connection(NULL)
{
}

CSender::~CSender(void)
{
	if (arguments != NULL) {
		delete[] arguments;
	}
	if (fileHandle != NULL) {
		CloseHandle(fileHandle);
	}
	if (fileContent != NULL) {
		free(fileContent);
	}
}

void CSender::SetConnection(CConnection *conn)
{
	connection = conn;
}

void CSender::SetSource(TCHAR *filename)
{
	this->filename = filename;

	// Check existance...
	fileHandle = CreateFile(filename, FILE_READ_DATA | FILE_READ_ATTRIBUTES, 
		FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL);

	if(fileHandle == INVALID_HANDLE_VALUE) {
		throw TEXT("CreateFile failed, are you sure that the file exists and is readable?");
	}
}

void CSender::SetArguments(TCHAR *argv)
{
	// First argument is the filename
	TCHAR *name = wcsrchr(filename, '\\') + 1;
	int nameLength = wcslen(name);

	LPWSTR *argsArr = CommandLineToArgvW(argv, &argumentcount);
	argumentsLength = (int) ((wcslen(argv) + 1) + nameLength + 1);
	arguments = new char[argumentsLength];
	char *argsPos = arguments + nameLength;

	memset(arguments, 0, argumentsLength);

	WideCharToMultiByte(CP_OEMCP, 0, name, -1, arguments, argumentsLength, NULL, NULL);

	for(int i = 1; i < argumentcount; i++) {
		int length = WideCharToMultiByte(CP_OEMCP, 0, argsArr[i], -1, NULL, 0, NULL, NULL);
		WideCharToMultiByte(CP_OEMCP, 0, argsArr[i], -1, argsPos, length, NULL, NULL);
		argsPos += strlen(argsPos) + 1;
	}
	LocalFree(argsArr);
}

int CSender::GetFilesize()
{
	if (filesize == -1) {
		DWORD high, low = GetFileSize(fileHandle, &high);
		filesize = high << 16 | low;
	}
	return filesize;
}

void * CSender::GetFileContents()
{
	if (fileContent == NULL) {
		DWORD bytesRead;
		int size = GetFilesize();
		fileContent = malloc(size);
		ReadFile(fileHandle, fileContent, size, &bytesRead, NULL);
	}
	return fileContent;
}

void CSender::SendFile()
{
	Send((const char *) GetFileContents(), GetFilesize());
}

void CSender::SendArguments()
{
	Send(arguments, argumentsLength);
}

void CSender::Send()
{
	connection -> Connect();

	try
	{
		SendHeaders();
		SendFile();
		SendArguments();

		if (connection != NULL) {
			connection->Disconnect();
		}
	}
	catch(...) {
		if (connection != NULL) {
			connection->Disconnect();
		}
		throw;
	}
}

void CSender::Send(const char *buffer, int length)
{
	connection -> Send(buffer, length);
}