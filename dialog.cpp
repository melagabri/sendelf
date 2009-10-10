#include "stdafx.h"

#include "SendElf.h"

HWND hFilename;
HWND hPort;
HWND hIpaddress;
HWND hSendbutton;
HWND hProtocol;
HWND hArgs;
HWND hPort_label;
HWND hIpaddress_label;

HWND hComport;
HWND hComport_label;

void updateSendButton(void);
void updateMode(void);

TCHAR *protocolNames[5]= { TEXT("Svpe (TCP)"), TEXT("HBC (TCP, - 1.0.4)"), TEXT("HBC (TCP, 1.0.5+)"), TEXT("HBC (Gecko, - 1.0.4)"), TEXT("HBC (Gecko, 1.0.5+)") };

// Message handler for diaog box.
INT_PTR CALLBACK MainDialog(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
	UNREFERENCED_PARAMETER(lParam);

	switch (message) {
		case WM_INITDIALOG: {

			hPort=GetDlgItem(hDlg, IDC_PORT);
			hIpaddress=GetDlgItem(hDlg, IDC_IPADDRESS);
			hFilename=GetDlgItem(hDlg, IDC_FILENAME);
			hSendbutton=GetDlgItem(hDlg, IDC_SENDBUTTON);
			hProtocol=GetDlgItem(hDlg, IDC_PROTOCOL);
			hArgs=GetDlgItem(hDlg, IDC_ARGS);

			hIpaddress_label=GetDlgItem(hDlg, IDC_IPADDRESS_LABEL);
			hPort_label=GetDlgItem(hDlg, IDC_PORT_LABEL);
			hComport_label=GetDlgItem(hDlg, IDC_COMPORT_LABEL);

			SetWindowText(hPort, portString);
			SetWindowText(hIpaddress, (LPCWSTR) ipString);
			SetWindowText(hFilename, filename);
			SetWindowText(hArgs, args);

			for(int i = 0; i < (hInstGecko != NULL ? 5 : 3); i++) {
				SendMessage(hProtocol,CB_INSERTSTRING,i,(LPARAM)protocolNames[i]);
			}

			if (protocol < 0 || protocol >= (DWORD) ((hInstGecko != NULL) ? 5 : 3)) {
				protocol = DEFAULT_PROTOCOL;
			}
			SendMessage(hProtocol,CB_SETCURSEL, protocol, 0);
			
			DragAcceptFiles(hDlg,true);
			hMainDialog=hDlg;
			updateSendButton();
			updateMode();

			SendMessage(
				hPort,
				EM_SETLIMITTEXT,
				6,
				0
			); 
			SendMessage(
				hArgs,
				EM_SETLIMITTEXT,
				MAX_ARGS_LEN,
				0
			); 
		} break;

		case WM_DROPFILES: {
			HDROP hDrop = (HDROP)wParam;
			TCHAR filename[260];

			DragQueryFile(
				hDrop,
				0,
				filename,
				sizeof(filename)
			);

			SetWindowText(hFilename, filename);

			DragFinish(hDrop);
			//send button is updated since the controll will send a change message, so no need to do it manualy
		} break;

		case WM_COMMAND:
			if (LOWORD(wParam) == IDC_BROWSEBUTTON) {
				OPENFILENAME ofn;
				TCHAR filename[260];

				ZeroMemory(&ofn, sizeof(ofn));
				ofn.lStructSize = sizeof(ofn);
				ofn.hwndOwner = hDlg;
				ofn.lpstrFile = filename;

				GetWindowText(hFilename, filename, MAX_PATH);

				// Set lpstrFile[0] to '\0' so that GetOpenFileName does not 
				// use the contents of szFile to initialize itself.
				//ofn.lpstrFile[0] = '\0';
				ofn.nMaxFile = sizeof(filename);
				ofn.lpstrFilter = TEXT("DOL, ELF and ZIP files\0*.DOL;*.ELF;*.ZIP\0DOL files \0*.DOL\0ELF files\0*.ELF\0ZIP files\0*.ZIP\0");
				ofn.nFilterIndex = 1;
				ofn.lpstrFileTitle = NULL;
				ofn.nMaxFileTitle = 0;
				ofn.lpstrInitialDir = NULL;
				ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
				ofn.hwndOwner=hDlg;

				// Display the Open dialog box. 
				if (GetOpenFileName(&ofn)==TRUE) {
					SetWindowText(hFilename, ofn.lpstrFile);
				}

				updateSendButton();

			} else if(LOWORD(wParam) == IDC_SENDBUTTON) {

				//TCHAR filename [MAX_PATH+1];
				GetWindowText(hFilename, filename, MAX_PATH);
        
				//char ipString [35];
				GetWindowText(hIpaddress, (LPWSTR) ipString, sizeof ipString);
				//char portString [10];
				GetWindowText(hPort, portString, sizeof portString);
				//char argsString [MAX_ARGS_LEN+1];
				GetWindowText(hArgs, args, sizeof args);

				protocol=(DWORD)SendMessage(hProtocol,CB_GETCURSEL,0,0);

				sendFile();


			} else if(LOWORD(wParam)==IDC_FILENAME && HIWORD(wParam)==EN_CHANGE) {
				updateSendButton();
			} else if(LOWORD(wParam)==IDC_IPADDRESS	&& HIWORD(wParam)==EN_CHANGE) {
				updateSendButton();
			} else if(LOWORD(wParam)==IDC_PORT && HIWORD(wParam)==EN_CHANGE) {
				updateSendButton();
			} else if(LOWORD(wParam)==IDC_PROTOCOL && HIWORD(wParam)==CBN_SELCHANGE) {
				updateMode();
			}
		break;

		case WM_ENDSESSION:
			if(wParam==FALSE) return 0;
			//else, fall thrught

		case WM_CLOSE: {
			
			save_Settings();

			EndDialog(hDlg, LOWORD(wParam));


			hMainDialog=NULL;
			return (INT_PTR)TRUE;
		} break;

		
	}
	return (INT_PTR)FALSE;
}

void updateSendButton(void) {
	TCHAR filename [MAX_PATH+1];
  GetWindowText(hFilename, filename, MAX_PATH);
	bool enable;
	DWORD protocol=(DWORD)SendMessage(hProtocol,CB_GETCURSEL,0,0);

	if (protocol < 0 || protocol > 4) {
		enable = false;
	}
	else if(protocol != 3 && protocol != 4) {
		long ipnumber;
		SendMessage(hIpaddress,IPM_GETADDRESS,0,(LPARAM)&ipnumber);

		TCHAR portString [10];
		GetWindowText(hPort, portString, sizeof portString);

		enable=(
			ipnumber!=0 &&
			_tcscmp(portString,TEXT(""))!=0 &&
			PathFileExists( filename)
		);
	} else {
		enable=PathFileExists(filename) && true;
	}

	EnableWindow(hSendbutton,enable);
}

void updateMode(void) {
	DWORD protocol=(DWORD)SendMessage(hProtocol,CB_GETCURSEL,0,0);
	EnableWindow(hArgs,protocol!=0);
	

	EnableWindow(hPort,protocol < 3);
	EnableWindow(hIpaddress,protocol < 3);
	EnableWindow(hPort_label,protocol < 3);
	EnableWindow(hIpaddress_label,protocol < 3);


}

void guiMessage(LPTSTR message) {
	MessageBox(hMainDialog,message,TEXT("Aw, crap"),0);
}