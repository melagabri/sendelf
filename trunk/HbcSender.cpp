#include "StdAfx.h"
#include "HbcSender.h"

#include "globals.h"

#define WIILOAD_VERSION_MAYOR 0
#define WIILOAD_VERSION_MINOR 4

CHbcSender::CHbcSender(void)
{
}

CHbcSender::~CHbcSender(void)
{
}

void CHbcSender::SendHeaders()
{
	char buf[12];
	memcpy(&buf, "HAXX", 4);
	buf[4] = WIILOAD_VERSION_MAYOR;
	buf[5] = WIILOAD_VERSION_MINOR;
	buf[6] = (argumentcount >> 8) & 0xFF;
	buf[7] = argumentcount & 0xFF;

	int fSize = EndianFlip(GetFilesize());
	memcpy((void *) &buf[8], (void *) &fSize, 4);

	Send((const char *) &buf, 12);
}
