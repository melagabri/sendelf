#pragma once

#include "Sender.h"

class CSvpeSender : public CSender
{
public:
	CSvpeSender(void);
	~CSvpeSender(void);

	void SendHeaders();
	void SendArguments();
};
