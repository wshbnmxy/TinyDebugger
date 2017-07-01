#ifdef _DEBUG
#define CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif // _DEBUG

#include <krcommon.h>
#include <krinfo.h>

#include <Windows.h>

BOOL WINAPI DllMain(HANDLE hModule, DWORD dwReason, LPVOID lpReserved) {
        switch (dwReason)
        {
        case DLL_PROCESS_ATTACH:
                break;
        case DLL_PROCESS_DETACH:
                kr_uninitDebugInfo();
#ifdef _DEBUG
                _CrtDumpMemoryLeaks();
#endif // _DEBUG
                break;
        default:
                break;
        }
        return TRUE;
}