#include "stdafx.h"

#include "SendElf.h"

TCHAR portString[12];
TCHAR ipString[17];
TCHAR filename[260];
TCHAR args[MAX_ARGS_LEN+1];
DWORD protocol;
HWND hMainDialog;

int endian_flip(int value)
{
	return (
		((value & 0xFF000000) >> 24) |
		((value & 0x00FF0000) >>  8) |
		((value & 0x0000FF00) <<  8) |
		((value & 0x000000FF) << 24));
}