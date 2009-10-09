#pragma once

#include "resource.h"
#include "stdafx.h"

typedef void (*messageCallback)(LPTSTR);
void guiMessage(LPTSTR message);

bool sendFile();

#include "globals.h"

void load_Settings(void);
void save_Settings(void);

typedef std::basic_string<TCHAR> tstring;