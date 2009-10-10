#pragma once

#include "Sender.h"
#include "include/zlib.h"

class CHbcCompSender : public CSender
{
public:
	CHbcCompSender(void);
	~CHbcCompSender(void);

	void SendHeaders();
	void SendFile();
protected:
	bool CompressFile();

	bool compress;
	uLongf compressedSize;
	void *compressFile;
};
