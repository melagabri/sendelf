#include "stdafx.h"
#include "SendElf.h"

void CreateBuffer(char *b, unsigned char protocol, unsigned int fileSize, short args_length) ;
bool TCP_SendFile(
	unsigned long ipInt,
	unsigned short portInt,
	LPTSTR filename,
	bool standalone,
	messageCallback message,
	char *args=NULL, unsigned short argslength=0) ;
INT_PTR CALLBACK MainDialog(HWND, UINT, WPARAM, LPARAM) ;

bool sendFile() {
	bool standalone= protocol==0;


	unsigned long ipInt;
	unsigned short portInt=(unsigned short)_tstoi(portString);

#ifdef UNICODE

	int	cfilename_length=(unsigned short)WideCharToMultiByte(
		CP_OEMCP,
		0,
		filename,
		-1,
		NULL,
		0,
		NULL,
		NULL
	);

	char *cfilename =new char[cfilename_length];

	WideCharToMultiByte(
		CP_OEMCP,
		0,
		filename,
		-1,
		cfilename,
		cfilename_length+1,
		NULL,
		NULL
	);

	int cip_length=(unsigned short)WideCharToMultiByte(
		CP_OEMCP,
		0,
		ipString,
		-1,
		NULL,
		0,
		NULL,
		NULL
	);

	char *cip =new char[cip_length];

	WideCharToMultiByte(
		CP_OEMCP,
		0,
		ipString,
		-1,
		cip,
		cip_length+1,
		NULL,
		NULL
	);

	ipInt=inet_addr(cip);

	delete[] cip;



	char *nameonly=strrchr(cfilename,'\\')+1;
	unsigned int nameonly_lenght=(unsigned int)strlen(nameonly)+1;

	int numargs;

	LPWSTR *argsArr=CommandLineToArgvW(args,&numargs);
	int t=(int)((wcslen(args)+1)+(numargs+1)+nameonly_lenght+1);
	char *argsBytes=new char[t];

	char *argsPos=argsBytes+nameonly_lenght;
	strcpy(argsBytes,nameonly);
	for(int i=0;i<numargs;i++) {

		int carg_length=(unsigned short)WideCharToMultiByte(
			CP_OEMCP,
			0,
			argsArr[i],
			-1,
			NULL,
			0,
			NULL,
			NULL
		);

		char *carg =new char[carg_length];

		WideCharToMultiByte(
			CP_OEMCP,
			0,
			argsArr[i],
			-1,
			carg,
			carg_length,
			NULL,
			NULL
		);


		strcpy(argsPos,carg);
		argsPos+=carg_length;
		*argsPos=0;
		argsPos+=1;

		delete[] carg;
	}
	*argsPos=0;

	unsigned short args_length=(unsigned short)(argsPos-argsBytes);

	LocalFree(argsArr);


	bool bSuccess;
	bSuccess=TCP_SendFile(ipInt,portInt,filename,standalone,guiMessage,argsBytes,args_length);

	delete[] argsBytes;
	delete[] cfilename;

	return bSuccess;
#else
#error nonunicode todo
#endif
}

bool TCP_SendFile(
	unsigned long ipInt,
	unsigned short portInt,
	LPTSTR filename,
	bool standalone,
	messageCallback message,
	char *args,unsigned short args_length
) {

	union {
		int isuccess;
		BOOL bSuccess;
	};

	HANDLE fileHandle = CreateFile(
		filename,
		FILE_READ_DATA | FILE_READ_ATTRIBUTES,
		FILE_SHARE_READ,
		NULL,
		OPEN_EXISTING,
		FILE_FLAG_SEQUENTIAL_SCAN,
		NULL
	);

	if(fileHandle == INVALID_HANDLE_VALUE) {
		message(TEXT("CreateFile failed, are you sure that the file exists and is readable?"));
		goto TCP_createfileError;
	}

	int fileSize = GetFileSize(fileHandle,NULL);
	if(INVALID_FILE_SIZE == fileSize) {
		message(TEXT("GetFileSize failed, are you sure that the chosen file exists and is readable?"));
		goto TCP_getfilesizeError;
	}

	SOCKET socketHandle = socket(
		AF_INET,//ip4
		SOCK_STREAM,//tcp
		IPPROTO_TCP//tcp again
	);

	if(socketHandle == INVALID_SOCKET) {
		message(TEXT("socket() failed, something seriously wrong is going on"));
		goto TCP_socketError;
	}

	struct sockaddr_in socketAddr;
	memset(&socketAddr,0,sizeof(socketAddr));

	socketAddr.sin_family=AF_INET;
	socketAddr.sin_addr.S_un.S_addr=ipInt;
	socketAddr.sin_port=htons(portInt);

	isuccess = connect( socketHandle, (const sockaddr *)&socketAddr, sizeof(socketAddr) );

	if (isuccess==SOCKET_ERROR) {
		message(TEXT("connect() failed! You sure that's the right ip address?"));
		goto TCP_connectError;
	}

	char head[12];
	CreateBuffer(head,standalone,fileSize,args_length);

	TRANSMIT_FILE_BUFFERS transferBuffers;

	transferBuffers.Head=&head;
	transferBuffers.HeadLength=(standalone?4:12);
	transferBuffers.Tail=args;
	transferBuffers.TailLength=args_length;

	bSuccess = TransmitFile(
		socketHandle,
		fileHandle,
		fileSize,
		0,
		NULL,
		&transferBuffers,
		TF_DISCONNECT //| TF_USE_DEFAULT_WORKER 
	);

	if(!bSuccess) {
		message(TEXT("TransmitFile failed!"));
		goto TCP_transferError;
	}

	CloseHandle(fileHandle);

	goto TCP_sendend;

TCP_transferError:
				
TCP_connectError:
	closesocket(socketHandle);
TCP_socketError:

TCP_getfilesizeError:
	CloseHandle(fileHandle);

TCP_createfileError:
	return false;
TCP_sendend:
	return true;
}


bool Gecko_SendFile (
	unsigned char /*portnumber*/,
	LPTSTR filename,
	messageCallback message,
	char /* *args*/,unsigned short args_length
) {

	BOOL succes;

	HANDLE fileHandle = CreateFile(
		filename,
		FILE_READ_DATA | FILE_READ_ATTRIBUTES,
		FILE_SHARE_READ,
		NULL,
		OPEN_EXISTING,
		FILE_FLAG_SEQUENTIAL_SCAN,
		NULL
	);

	if(fileHandle == INVALID_HANDLE_VALUE) {
		message(TEXT("CreateFile failed, are you sure that the file exists and is readable?"));
		goto GECKO_createfileError;
	}

	int fileSize = GetFileSize(fileHandle,NULL);
	if(INVALID_FILE_SIZE == fileSize) {
		message(TEXT("GetFileSize failed, are you sure that the chosen file exists and is readable?"));
		goto GECKO_getfilesizeError;
	}

	char head[12];

	CreateBuffer(head,2,fileSize,args_length);

	//if(!WriteFile(handle_gecko,head,12,NULL,NULL)) {
	//	message(TEXT("Writing the header to the gecko failed, are you using the right com port?"));
	//	goto GECKO_writeError;
	//}

	char* readBuffer=new char[fileSize];

	succes=ReadFile(
		fileHandle,
		readBuffer,
		fileSize,
		NULL,
		NULL
	);

	if(!succes) {
		message(TEXT("readfile failed, are you sure that the file exists and is readable?"));
		delete readBuffer;
		goto GECKO_readError;
	}

	//succes=WriteFile(handle_gecko,readBuffer,12,NULL,NULL);

	delete readBuffer;

	if(!succes) {
		message(TEXT("Writing the file to the gecko failed, are you using the right com port?"));
		goto GECKO_writeError;
	}

	if(args_length>0) {
		//succes=WriteFile(handle_gecko,args,args_length,NULL,NULL);
		if(!succes) {
			message(TEXT("Writing the args to the gecko failed, are you using the right com port?"));
			goto GECKO_writeError;
		}
	}

	CloseHandle(fileHandle);

	goto GECKO_EndSend;

GECKO_writeError:
GECKO_readError:

GECKO_getfilesizeError:
	CloseHandle(fileHandle);
GECKO_createfileError:

	return false;
GECKO_EndSend:
	return true;
}

void CreateBuffer(char *b, unsigned char protocol, unsigned int fileSize, short args_length) {

	//Do an endian flip
	int outSize = (
		((fileSize & 0xFF000000) >> 24) |
		((fileSize & 0x00FF0000) >>  8) |
		((fileSize & 0x0000FF00) <<  8) |
		((fileSize & 0x000000FF) << 24)
	);
	//int outSize=fileSize;

	b[0]='H';
	b[1]='A';
	b[2]=b[3]='X';

	b[4]=WIILOAD_VERSION_MAYOR;
	b[5]=WIILOAD_VERSION_MINOR;

	b[6]=(char)(args_length>>8)&0xFF;
	b[7]=(char)args_length&0xFF;	

	memcpy(b+(protocol!=0?0:8),&outSize,4);
}