#include "stdafx.h"

#include "SendElf.h"

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;

WSADATA wsaData;

INT_PTR CALLBACK MainDialog(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

int APIENTRY _tWinMain(
	HINSTANCE hInstance,
	HINSTANCE,
	LPTSTR commandline,
	int
) {
	hInst = hInstance;
	
	union {
		int iSuccess;
		bool bSuccess;
	};

	iSuccess = WSAStartup(MAKEWORD(2,2), &wsaData);
	if(iSuccess!=0) {
		MessageBox(NULL, TEXT("Winsock could not be initalized"), TEXT("Aw crap"), 0);
		return 1;
	}

	load_Settings();
	
	hInstGecko = LoadLibrary(L"ftd2xx.dll");
	
	bool showDialog=true;
	bool doSend=false;

	if(*commandline!=0) {
		//MessageBox(NULL,commandline,0,MB_ICONINFORMATION);
		LPTSTR substring;
		if(_tcsstr(commandline,TEXT("/noshow"))!=0) {
			showDialog=false;
		}

		substring=_tcsstr(commandline,TEXT("/file") );
		//note: quotes are mandatory and the spacing must be correct
		if(substring!=0) {
			LPTSTR namestart=substring+7;
			int namelength;

			namelength=int(_tcsstr(namestart,TEXT("\"") )-namestart);

			_tcsncpy(filename,namestart,namelength);
			filename[namelength]=0;
		}

		substring=_tcsstr(commandline,TEXT("/send"));
		if(substring!=0) {
			doSend=true;
		}
	}

	if(doSend) {
		if((ipString==0 || *ipString==0) && protocol != 3 && protocol != 4) {
			MessageBox(NULL,TEXT("The IP address is not yet set, here, have the GUI"),TEXT("A bit too early"),MB_ICONINFORMATION |MB_OK);
			showDialog=true;
		} else {
			bSuccess=sendFile();
			if(!bSuccess) {
				showDialog=true;
			} else if(!showDialog) {
				Sleep(1000);//hack
			}
		}
	}

	if(showDialog) {
		DialogBoxW(hInst, MAKEINTRESOURCE(IDD_MAINDIALOG), 0, MainDialog);
	}

	FreeLibrary(hInstGecko);

	WSACleanup();

	return 0;
	//return (int) msg.wParam;
}

