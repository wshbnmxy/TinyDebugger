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

wtint32_t kr_getModuleNameSub(wtwchar_t *szNtName, wtwchar_t *szName, wtuint32_t nSize)
{
        wtwchar_t  szBuffer[_MAX_PATH] = L" :";
        wtwchar_t  szNtBuffer[_MAX_PATH] = L"";
        wtuint32_t nNtBufferLen;
        wtuint32_t nError = 0;

        for (szBuffer[0] = L'A'; szBuffer[0] <= L'Z'; szBuffer[0]++) {
                if (QueryDosDeviceW(szBuffer, szNtBuffer, _MAX_PATH) > 0) {

                        nNtBufferLen = wcslen(szNtBuffer);
                        if (wcsncmp(szNtBuffer, szNtName, nNtBufferLen) == 0 && szNtName[nNtBufferLen] == L'\\') {
                                _snwprintf(szName, nSize, L"%s%s", szBuffer, &szNtName[nNtBufferLen]);
                                return wtsuccess;
                        }
                        
                } else {
                        nError = GetLastError();
                        if (nError == ERROR_FILE_NOT_FOUND) {
                                continue;
                        }
                        return wtfailure;
                }
        }
        return wtfailure;
}

wtint32_t kr_getModuleName(wtpvoid_t pFile, wtwchar_t *szName, wtuint32_t nSize) {

        wtint32_t nRet = 0;
        wtwchar_t szBufferNtName[_MAX_PATH * 2];

        // param check
        if (pFile == NULL || szName == NULL || nSize == 0) {
                return wtfailure;
        }

        ZeroMemory(szName, sizeof(wtwchar_t) * nSize);
        
        // get mapped file name
        if (!GetMappedFileNameW(GetCurrentProcess(), pFile, szBufferNtName, _MAX_PATH * 2)) {
                return wtfailure;
        }

        // get full path
        return kr_getModuleNameSub(szBufferNtName, szName, nSize);
}