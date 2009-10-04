#pragma once

#include "resource.h"
#include "stdafx.h"

typedef void (*messageCallback)(LPTSTR);
void guiMessage(LPTSTR message);

bool sendFile();

const unsigned char WIILOAD_VERSION_MAYOR= 0;
const unsigned char WIILOAD_VERSION_MINOR= 3;
const unsigned short MAX_ARGS_LEN=1024;

#include "globals.h"

void load_Settings(void);
void save_Settings(void);

typedef std::basic_string<TCHAR> tstring;