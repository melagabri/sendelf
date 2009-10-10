#include "StdAfx.h"
#include "HbcCompSender.h"

#include "globals.h"

#define WIILOAD_VERSION_MAYOR 0
#define WIILOAD_VERSION_MINOR 5

CHbcCompSender::CHbcCompSender(void)
{
}

CHbcCompSender::~CHbcCompSender(void)
{
	if (compressFile != NULL) {
		free(compressFile);
		compressFile = NULL;
	}
}

void CHbcCompSender::SendHeaders()
{
	char buf[16];
	memcpy(&buf, "HAXX", 4);
	buf[4] = WIILOAD_VERSION_MAYOR;
	buf[5] = WIILOAD_VERSION_MINOR;
	buf[6] = (argumentcount >> 8) & 0xFF;
	buf[7] = argumentcount & 0xFF;

	void *content = GetFileContents();
	compress = (memcmp(content, "PK\x03\x04", 4) == 0) ? false : CompressFile();

	int cSize = EndianFlip(compressedSize);
	int fSize = EndianFlip(GetFilesize());

	if (compress) {
		memcpy((void *) &buf[8], (void *) &cSize , 4);
		memcpy((void *) &buf[12], (void *) &fSize, 4);
	} else {
		memcpy((void *) &buf[8], (void *) &fSize, 4);
		memset((void *) &buf[12], 0, 4);
	}

	Send((const char *) &buf, 16);
}

void CHbcCompSender::SendFile()
{
	if (compress) {
		Send((const char *) compressFile, compressedSize);
	} else {
		Send((const char *) GetFileContents(), GetFilesize());
	}
}

bool CHbcCompSender::CompressFile()
{
	int fsize = GetFilesize();

	compressedSize = (uLongf) ((float) fsize * 1.02);
	compressFile = malloc(compressedSize);

	int res = compress2((Bytef *) compressFile, &compressedSize, (const Bytef *) GetFileContents(), fsize, 6);
	if (res != Z_OK || compressedSize > (uLongf) fsize) {
		compressedSize = 0;
		free(compressFile);
		compressFile = NULL;
		return false;
	}
	return true;
}
