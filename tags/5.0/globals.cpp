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

PWCHAR* ParseCommandLineArgs(PWCHAR CmdLine, int* _argc)
{
    PWCHAR* argv;
    PWCHAR  _argv;
    ULONG   len;
    ULONG   argc;
    WCHAR   a;
    ULONG   i, j;

    BOOLEAN  in_QM;
    BOOLEAN  in_TEXT;
    BOOLEAN  in_SPACE;

    len = wcslen(CmdLine);
    i = ((len+2)/2)*sizeof(PVOID) + sizeof(PVOID);

    argv = (PWCHAR*)GlobalAlloc(GMEM_FIXED,
        i + (len+2)*sizeof(WCHAR));

    _argv = (PWCHAR)(((PUCHAR)argv)+i);

    argc = 0;
    argv[argc] = _argv;
    in_QM = FALSE;
    in_TEXT = FALSE;
    in_SPACE = TRUE;
    i = 0;
    j = 0;

	while((a = CmdLine[i]) != 0) {
        if(in_QM) {
            if(a == '\"') {
                in_QM = FALSE;
            } else {
                _argv[j] = a;
                j++;
            }
        } else {
            switch(a) {
            case '\"':
                in_QM = TRUE;
                in_TEXT = TRUE;
                if(in_SPACE) {
                    argv[argc] = _argv+j;
                    argc++;
                }
                in_SPACE = FALSE;
                break;
            case ' ':
            case '\t':
            case '\n':
            case '\r':
                if(in_TEXT) {
                    _argv[j] = '\0';
                    j++;
                }
                in_TEXT = FALSE;
                in_SPACE = TRUE;
                break;
            default:
                in_TEXT = TRUE;
                if(in_SPACE) {
                    argv[argc] = _argv+j;
                    argc++;
                }
                _argv[j] = a;
                j++;
                in_SPACE = FALSE;
                break;
            }
        }
        i++;
    }
    _argv[j] = '\0';
    argv[argc] = NULL;

    (*_argc) = argc;
    return argv;
}