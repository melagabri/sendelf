#include "stdafx.h"

#include "SendElf.h"

HKEY softwareKey;


extern HWND hFilename;
extern HWND hPort;
extern HWND hIpaddress;
extern HWND hSendbutton;
extern HWND hProtocol;
extern HWND hArgs;

void load_Settings(void) {
	//Load some defaults
	_tcscpy_s(portString,10,TEXT("4299"));
	ipString[0]=0;
	filename[0]=0;
	args[0]=0;
	protocol=1;

	long lSuccess=RegCreateKeyEx(
		HKEY_CURRENT_USER,
		TEXT("Software\\TCP Loader"),
		0,
		NULL,
		REG_OPTION_NON_VOLATILE,
		KEY_WRITE|KEY_READ,
		NULL,
		&softwareKey,
		NULL
	);

	if(lSuccess!=ERROR_SUCCESS) {
		goto regkeyopenError;
	}

	DWORD bufferlength=sizeof(filename);

	RegQueryValueEx(
		softwareKey,
		TEXT("File"),
		0,
		NULL,
		(LPBYTE)filename,
		&bufferlength
	);

	bufferlength=sizeof(ipString);

	RegQueryValueEx(
		softwareKey,
		TEXT("Ip"),
		0,
		NULL,
		(LPBYTE)ipString,
		&bufferlength
	);

	bufferlength=sizeof(portString);

	RegQueryValueEx(
		softwareKey,
		TEXT("Port"),
		0,
		NULL,
		(LPBYTE)portString,
		&bufferlength
	);

	bufferlength=sizeof(protocol);

	RegQueryValueEx(
		softwareKey,
		TEXT("Protocol"),
		0,
		NULL,
		(LPBYTE)&protocol,
		&bufferlength
	);

	bufferlength=sizeof(args);

	RegQueryValueEx(
		softwareKey,
		TEXT("Args"),
		0,
		NULL,
		(LPBYTE)&args,
		&bufferlength
	);

	goto endofregread;

regkeyopenError:
	//Errors are ignored, they key most likely just didn't exist yet, since we where first
	//MessageBox(hMainDialog,"Could not open the registery key","Aw, crap",0);
endofregread:
	return;
}

void save_Settings(void) {

    GetWindowText(hFilename, filename, MAX_PATH);
	GetWindowText(hIpaddress, (LPWSTR) ipString, sizeof ipString);
	GetWindowText(hPort, portString, sizeof portString);
	GetWindowText(hArgs, args, sizeof args);


	protocol=(DWORD)SendMessage(hProtocol,CB_GETCURSEL,0,0);



	long lSuccess=RegSetValueEx(
		softwareKey,
		TEXT("Port"),
		0,
		REG_SZ,
		(const BYTE*)portString,
		(DWORD)(_tcslen(portString)+1)*sizeof(TCHAR)
	);

	if(lSuccess!=ERROR_SUCCESS) {
		goto regkeywriteError;
	}

	lSuccess=RegSetValueEx(
		softwareKey,
		TEXT("Ip"),
		0,
		REG_SZ,
		(const BYTE*)ipString,
		(DWORD)(_tcslen(ipString)+1)*sizeof(TCHAR)
	);

	if(lSuccess!=ERROR_SUCCESS) {
		goto regkeywriteError;
	}

	lSuccess=RegSetValueEx(
		softwareKey,
		TEXT("File"),
		0,
		REG_SZ,
		(const BYTE*)filename,
		(DWORD)(_tcslen(filename)+1)*sizeof(TCHAR)
	);

	if(lSuccess!=ERROR_SUCCESS) {
		goto regkeywriteError;
	}

	lSuccess=RegSetValueEx(
		softwareKey,
		TEXT("Protocol"),
		0,
		REG_DWORD,
		(const BYTE*)&protocol,
		(DWORD)sizeof(protocol)
	);

	if(lSuccess!=ERROR_SUCCESS) {
		goto regkeywriteError;
	}

	lSuccess=RegSetValueEx(
		softwareKey,
		TEXT("Args"),
		0,
		REG_SZ,
		(const BYTE*)args,
		(DWORD)(_tcslen(args)+1)*sizeof(TCHAR)
	);

	if(lSuccess!=ERROR_SUCCESS) {
		goto regkeywriteError;
	}

	goto endofregwrite;

			
regkeywriteError:
	MessageBox(hMainDialog,TEXT("Could not write to the registery key"),TEXT("Aw, crap"),0);
endofregwrite:
	RegCloseKey(softwareKey);
}