#include "StdAfx.h"
#include "SvpeSender.h"

#include "globals.h"

CSvpeSender::CSvpeSender(void)
{
}

CSvpeSender::~CSvpeSender(void)
{
}

void CSvpeSender::SendHeaders()
{
	char buf[4];
	int fSize = EndianFlip(GetFilesize());
	memcpy(&buf, (void *) &fSize, 4);

	Send((const char *) &buf, 4);
}

void CSvpeSender::SendArguments()
{
}