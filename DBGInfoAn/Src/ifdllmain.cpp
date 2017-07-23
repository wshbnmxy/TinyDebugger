#include <ifcommon.h>
#include <ifinfo.h>
#include <Windows.h>

BOOL WINAPI DllMain(HANDLE hModule, DWORD dwReason, LPVOID lpReserved) {
        switch (dwReason)
        {
        case DLL_PROCESS_ATTACH:
                break;
        case DLL_PROCESS_DETACH:
                break;
        default:
                break;
        }
        return TRUE;
}