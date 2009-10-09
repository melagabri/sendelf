#include "stdafx.h"
#include "SendElf.h"

#include "Sender.h"
#include "SvpeSender.h"
#include "HbcSender.h"
#include "HbcCompSender.h"
#include "TcpConnection.h"
#include "ComConnection.h"

bool sendFile() {
	CConnection *connection = NULL;
	CSender *sender = NULL;
	bool bSuccess = true;
	char *ipAddr = NULL;
	
	try
	{
		switch(protocol)
		{
			case 0:
				sender = new CSvpeSender();
				connection = new CTcpConnection();
				break;
			case 1:
				sender = new CHbcSender();
				connection = new CTcpConnection();
				break;
			case 2:
				sender = new CHbcCompSender();
				connection = new CTcpConnection();
				break;
			case 3:
				sender = new CHbcSender();
				connection = new CComConnection();
				break;
			case 4:
				sender = new CHbcCompSender();
				connection = new CComConnection();
				break;
		}
		sender->SetConnection(connection);

		// Set the file
		sender->SetSource((TCHAR *) &filename);

		if (protocol != 3 && protocol != 4) {
			// Set the destination
			int ipLength = WideCharToMultiByte(CP_OEMCP, 0, ipString, -1, NULL, 0, NULL, NULL);
			ipAddr = new char[ipLength + 1];
			memset(ipAddr, 0, ipLength + 1);
			WideCharToMultiByte(CP_OEMCP, 0, ipString, -1, ipAddr, ipLength, NULL, NULL);
			
			unsigned short ipPort = (unsigned short) _tstoi(portString);
			((CTcpConnection *) connection)->SetDestination(ipAddr, ipPort);
		}
		sender->SetArguments(args);
		sender->Send();
	}
	catch(LPTSTR ex)
	{
		guiMessage(ex);
		bSuccess = false;
	}
	if (sender != NULL) {
		delete sender;
	}
	if (connection != NULL) {
		delete connection;
	}
	if (ipAddr != NULL) {
		delete[] ipAddr;
	}
	return bSuccess;
}
