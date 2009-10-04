#include "stdafx.h"

#include "SendElf.h"

TCHAR portString[12];
TCHAR ipString[17];
TCHAR filename[260];
TCHAR args[MAX_ARGS_LEN+1];
DWORD protocol;
HWND hMainDialog;