#include <krutil.h>
#include <krerror.h>
#include <krinfo.h>

#include <setjmp.h>
#include <psapi.h>

static jmp_buf kr_util_jmppoint;

wtint32_t kr_getModuleNameSub(wtwchar *szNtName, wtwchar *szName, wtuint32_t nSize) {

        wtwchar    szBuffer[_MAX_PATH] = L" :";
        wtwchar    szNtBuffer[_MAX_PATH] = L"";
        wtuint32_t nNtBufferLen;
        wtint32_t  nRet = 0;

        for (szBuffer[0] = L'A'; szBuffer[0] <= L'Z'; szBuffer[0]++) {
                if (QueryDosDeviceW(szBuffer, szNtBuffer, _MAX_PATH) > 0) {

                        nNtBufferLen = wcslen(szNtBuffer);
                        if (wcsncmp(szNtBuffer, szNtName, nNtBufferLen) == 0 && szNtName[nNtBufferLen] == L'\\') {
                                _snwprintf(szName, nSize, L"%s%s", szBuffer, &szNtName[nNtBufferLen]);
                                break;
                        }
                        
                } else {
                        nRet = kr_errno = GetLastError();
                        if (kr_errno == ERROR_INSUFFICIENT_BUFFER) {
                                nRet = kr_errno = WT_MEMORY_SMALL_ERROR;
                        } else if (kr_errno = ERROR_FILE_NOT_FOUND) {
                                nRet = kr_errno = 0;
                                continue;
                        } else {
                                nRet = WT_NOT_EXPECT_ERROR;
                        }
                        return nRet;
                }
        }
        return nRet;
}

wtint32_t kr_getModuleName(wtpvoid pFile, wtwchar *szName, wtuint32_t nSize) {

        wtint32_t nRet = 0;
        wtwchar   szBufferNtName[_MAX_PATH * 2];

        kr_errno = 0;

        // param check
        if (pFile == NULL || szName == NULL || nSize == 0) {
                nRet = kr_errno = WT_PARAM_ERROR;
                return nRet;
        }

        ZeroMemory(szName, sizeof(wtwchar) * nSize);
        
        // get mapped file name
        if (!GetMappedFileNameW(GetCurrentProcess(), pFile, szBufferNtName, _MAX_PATH * 2)) {
                nRet = WT_NOT_EXPECT_ERROR;
                kr_errno = GetLastError();
                return nRet;
        }

        // get full path
        return kr_getModuleNameSub(szBufferNtName, szName, nSize);
}

wtint32_t kr_analyzeFile(wtpvoid pFile, krModuleInfo *pModule) {
        
        PIMAGE_DOS_HEADER     pDosHeader;
        PIMAGE_NT_HEADERS     pNtHeaders;
        PIMAGE_SECTION_HEADER pSectionHeader;
        
        pDosHeader = pFile;

        // check MZ flag
        if (pDosHeader->e_magic != IMAGE_DOS_SIGNATURE) {
                longjmp(kr_util_jmppoint, WT_INVAILD_PEFILE);
        }
        
        pNtHeaders = (wtvoid *)((wtchar *)pFile + pDosHeader->e_lfanew);

        // check PE00 flag
        if (pNtHeaders->Signature != IMAGE_NT_SIGNATURE) {
                longjmp(kr_util_jmppoint, WT_INVAILD_PEFILE);
        }
        
        // check machine
        if (pNtHeaders->FileHeader.Machine != IMAGE_FILE_MACHINE_I386) {
                longjmp(kr_util_jmppoint, WT_INVAILD_PEFILE);
        }

        pModule->m_imageInfo.m_endVA = (wtchar *)pModule->m_imageInfo.m_startVA
                                                + pNtHeaders->OptionalHeader.SizeOfImage;
        
        for (wtuint32_t i = 0 ; i < pNtHeaders->FileHeader.NumberOfSections; i++) {
                pSectionHeader = (wtvoid *)((wtchar *)pNtHeaders + 
                                        + sizeof(IMAGE_NT_HEADERS) + (i * sizeof(IMAGE_SECTION_HEADER)));
        }
        
        return 0;
}

wtint32_t kr_analyzeModule(HANDLE hFile, wtpvoid nBase) {
        
        HANDLE    hMap  = NULL;
        wtpvoid   pFile = NULL;
        wtint32_t nRet;
        wtwchar   szName[_MAX_PATH];
        wtpvoid   pModule = NULL;
        
        nRet = kr_errno = 0;
        // parm check
        if (hFile == INVALID_HANDLE_VALUE) {
                nRet = kr_errno = WT_PARAM_ERROR;
                goto l_ret;
        }
        
        // map file
        hMap = CreateFileMappingW(hFile, NULL, PAGE_READONLY, 0, 0, NULL);
        if (hMap == NULL) {
                nRet = WT_NOT_EXPECT_ERROR;
                kr_errno = GetLastError();
                goto l_ret;
        }
        
        pFile = MapViewOfFile(hMap, FILE_MAP_READ, 0, 0, 0);
        if (pFile == NULL) {
                nRet = WT_NOT_EXPECT_ERROR;
                kr_errno = GetLastError();
                goto l_ret;
        }

        // get file name
        nRet = kr_getModuleName(pFile, szName, _MAX_PATH);
        if (nRet != 0) {
                goto l_ret;
        }

        // insert module to data
        pModule = kr_insertModuleInfo(szName, nBase);
        if (pModule == NULL) {
                goto l_ret;
        }

        nRet = setjmp(kr_util_jmppoint);
        if (nRet != 0) {
                goto l_ret;
        }
        kr_analyzeFile(pFile, pModule);
        pdb_read(szName);
        
l_ret:
        if (pFile != NULL) {
                UnmapViewOfFile(pFile);
        }
        if (hMap != NULL) {
                CloseHandle(hMap);
        }
        
        CloseHandle(hFile);

        if (nRet != 0) {
                kr_jmpWithError(nRet);
        }
        return nRet;
}