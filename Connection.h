#pragma once

class CConnection
{
public:
	CConnection(void);
	~CConnection(void);

	virtual void Connect() = 0;
	virtual void Send(const char *buffer, int length) = 0;
	virtual void Disconnect() = 0;
};
