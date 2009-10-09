#pragma once

#include "Sender.h"

class CHbcSender : public CSender
{
public:
	CHbcSender(void);
	~CHbcSender(void);

	void SendHeaders();
};
