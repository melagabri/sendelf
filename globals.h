const unsigned char WIILOAD_VERSION_MAYOR= 0;
const unsigned char WIILOAD_VERSION_MINOR= 3;
const unsigned short MAX_ARGS_LEN=1024;

extern TCHAR portString[12];
extern TCHAR ipString[17];
extern TCHAR filename[260];
extern TCHAR args[MAX_ARGS_LEN+1];
extern DWORD protocol;

extern HWND hMainDialog;

int endian_flip(int value);
PWCHAR* ParseCommandLineArgs(PWCHAR CmdLine, int* _argc);
