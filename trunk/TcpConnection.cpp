#include "StdAfx.h"
#include "TcpConnection.h"

CTcpConnection::CTcpConnection(void) : sock(INVALID_SOCKET)
{
}

void CTcpConnection::SetDestination(char *address, unsigned short port)
{
	this->address = address;
	this->port = port;
}

void CTcpConnection::Connect()
{
	struct sockaddr_in target;
	
	sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock == INVALID_SOCKET) {
		throw TEXT("socket() failed, something seriously wrong is going on");
	}

	memset(&target, 0, sizeof(SOCKADDR_IN));
	target.sin_family = AF_INET;
	target.sin_addr.S_un.S_addr = inet_addr(address);
	target.sin_port = htons(port);

	if (connect(sock, (struct sockaddr *) &target, sizeof(target)) == SOCKET_ERROR) {
		throw TEXT("connect() failed! You sure that's the right ip address?");
	}
}

void CTcpConnection::Send(const char *buffer, int length)
{
	send(sock, buffer, length, 0);
}

void CTcpConnection::Disconnect()
{
	if (sock != INVALID_SOCKET) {
		closesocket(sock);
		sock = INVALID_SOCKET;
	}
}
