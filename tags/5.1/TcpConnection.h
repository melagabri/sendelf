#pragma once
#include "connection.h"

class CTcpConnection :
	public CConnection
{
public:
	CTcpConnection(void);

	void SetDestination(char *address, unsigned short port);
	void Connect();
	void Send(const char *buffer, int length);
	void Disconnect();
private:
	SOCKET sock;
	char *address;
	unsigned short port;
};
