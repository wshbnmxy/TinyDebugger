#include <krutil.h>
#include <krerror.h>
#include <krinfo.h>

#include <setjmp.h>
#include <psapi.h>

wtpvoid_t kr_wstrdup(const wtwchar_t *src)
{
        wtwchar_t *dest = NULL;

        if (src == NULL) {
                return dest;
        }

        dest = malloc(wcslen(src) * sizeof(wtwchar_t));
        if (dest == NULL) {
                return dest;
        }

        return wcscpy(dest, src);
}

wtpvoid_t kr_malloc(const wtuint32_t len)
{
        wtpvoid_t target = NULL;
        
        if (len == 0) {
                return target;
        }

        target = malloc(len);
        if (target == NULL) {
                return target;
        }

        return ZeroMemory(target, len);
}

wtvoid_t kr_free(wtpvoid_t *mem)
{
        if (mem != NULL && *mem != NULL) {
                free(*mem);
                *mem = NULL;
        }
}

wtint32_t kr_readMemory(HANDLE hProcess, const wtpvoid_t nBase, const wtuint32_t nSize, wtpvoid_t pOut)
{
        wtuint32_t nReadSize = 0;

        if (hProcess == INVALID_HANDLE_VALUE || nBase == NULL || nSize == 0 || pOut == NULL) {
                return wtfailure;
        }

        if (!ReadProcessMemory(hProcess, nBase, pOut, nSize, &nReadSize)) {
                return wtfailure;
        }

        return nSize == nReadSize ? wtsuccess : wtfailure;
}

wtint32_t kr_writeMemory(HANDLE hProcess, const wtpvoid_t nBase, const wtuint32_t nSize, const wtpvoid_t pIn)
{
        wtuint32_t nWriteSize = 0;

        if (hProcess == INVALID_HANDLE_VALUE || nBase == NULL || nSize == 0 || pIn == NULL) {
                return wtfailure;
        }

        if (!WriteProcessMemory(hProcess, nBase, pIn, nSize, &nWriteSize)) {
                return wtfailure;
        }

        return nSize == nWriteSize ? wtsuccess : wtfailure;
}

wtint32_t kr_readRegisters(HANDLE hThread, LPCONTEXT pRegisters)
{
        if (hThread == INVALID_HANDLE_VALUE || pRegisters == NULL) {
                return wtfailure;
        }

        if (!GetThreadContext(hThread, pRegisters)) {
                return wtfailure;
        }

        return wtsuccess;
}

wtint32_t kr_writeRegisters(HANDLE hThread, const LPCONTEXT pRegisters)
{
        if (hThread == INVALID_HANDLE_VALUE || pRegisters == NULL) {
                return wtfailure;
        }

        if (!SetThreadContext(hThread, pRegisters)) {
                return wtfailure;
        }

        return wtsuccess;
}