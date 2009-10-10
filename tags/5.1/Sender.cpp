#include "StdAfx.h"
#include "Sender.h"
#include "globals.h"

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

	LPWSTR *argsArr = ParseCommandLineArgs(argv, &argumentcount);
	argumentsLength = (int) ((wcslen(argv) + 1) + nameLength + 1);
	arguments = new char[argumentsLength];
	char *argsPos = arguments + nameLength + 1;

	memset(arguments, 0, argumentsLength);

	WideCharToMultiByte(CP_OEMCP, 0, name, -1, arguments, argumentsLength, NULL, NULL);

	for(int i = 0; i < argumentcount; i++) {
		int length = WideCharToMultiByte(CP_OEMCP, 0, argsArr[i], -1, NULL, 0, NULL, NULL);
		WideCharToMultiByte(CP_OEMCP, 0, argsArr[i], -1, argsPos, length, NULL, NULL);
		argsPos += strlen(argsPos) + 1;
	}
	delete[] argsArr;
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

int CSender::EndianFlip(int value)
{
	return (
		((value & 0xFF000000) >> 24) |
		((value & 0x00FF0000) >>  8) |
		((value & 0x0000FF00) <<  8) |
		((value & 0x000000FF) << 24));
}

PWCHAR* CSender::ParseCommandLineArgs(PWCHAR CmdLine, int* _argc)
{
    PWCHAR* argv;
    PWCHAR  _argv;
    ULONG   len;
    ULONG   argc;
    WCHAR   a;
    ULONG   i, j;

    BOOLEAN  in_QM;
    BOOLEAN  in_TEXT;
    BOOLEAN  in_SPACE;

    len = wcslen(CmdLine);
    i = ((len+2)/2)*sizeof(PVOID) + sizeof(PVOID);

    argv = new PWCHAR[i + (len+2)*sizeof(WCHAR)];

    _argv = (PWCHAR)(((PUCHAR)argv)+i);

    argc = 0;
    argv[argc] = _argv;
    in_QM = FALSE;
    in_TEXT = FALSE;
    in_SPACE = TRUE;
    i = 0;
    j = 0;

	while((a = CmdLine[i]) != 0) {
        if(in_QM) {
            if(a == '\"') {
                in_QM = FALSE;
            } else {
                _argv[j] = a;
                j++;
            }
        } else {
            switch(a) {
            case '\"':
                in_QM = TRUE;
                in_TEXT = TRUE;
                if(in_SPACE) {
                    argv[argc] = _argv+j;
                    argc++;
                }
                in_SPACE = FALSE;
                break;
            case ' ':
            case '\t':
            case '\n':
            case '\r':
                if(in_TEXT) {
                    _argv[j] = '\0';
                    j++;
                }
                in_TEXT = FALSE;
                in_SPACE = TRUE;
                break;
            default:
                in_TEXT = TRUE;
                if(in_SPACE) {
                    argv[argc] = _argv+j;
                    argc++;
                }
                _argv[j] = a;
                j++;
                in_SPACE = FALSE;
                break;
            }
        }
        i++;
    }
    _argv[j] = '\0';
    argv[argc] = NULL;

    (*_argc) = argc;
    return argv;
}