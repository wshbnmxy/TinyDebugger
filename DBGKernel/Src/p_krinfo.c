#include <krinfo.h>
#include <krerror.h>
#include <krutil.h>

#include <string.h>
#include <Windows.h>
#include <psapi.h>

static krDbgInfo  krDbgInfoG;
static krDbgInfo *pKrDbgInfoG = &krDbgInfoG;

wtint32_t kr_initInfo(LPCREATE_PROCESS_DEBUG_INFO pCreateInfo)
{
        wtint32_t nRet = wtfailure;
        krProcessInfo *processInfo;
        krThreadInfo  *threadInfo;
        ZeroMemory(pKrDbgInfoG, sizeof(krDbgInfo));

        processInfo = kr_malloc(sizeof(krProcessInfo));
        if (processInfo == NULL) {
                goto l_ret;
        }
        processInfo->m_hanlde = pCreateInfo->hProcess;
        processInfo->m_dwId   = GetProcessId(pCreateInfo->hProcess);

        threadInfo = kr_malloc(sizeof(krThreadInfo));
        if (threadInfo == NULL) {
                goto l_ret;
        }

        threadInfo->m_handle = pCreateInfo->hThread;
        threadInfo->m_dwId   = GetThreadId(pCreateInfo->hThread);
        threadInfo->m_next   = NULL;

        pKrDbgInfoG->m_pProcess = processInfo;
        pKrDbgInfoG->m_pThreads = threadInfo;

        nRet = kr_addModuleInfo(pCreateInfo->hFile, pCreateInfo->lpBaseOfImage);
l_ret:
        if (nRet == wtfailure) {
                kr_free(&processInfo);
                kr_free(&threadInfo);
                kr_free(&(pKrDbgInfoG->m_pModules));
        }
        return nRet;
}

wtint32_t kr_uninitInfo()
{
        while (pKrDbgInfoG->m_pModules != NULL) {
                krModuleInfo *pVisitor = pKrDbgInfoG->m_pModules;
                pKrDbgInfoG->m_pModules = pVisitor->m_next;
                
                kr_free(&pVisitor);
        }

        CloseHandle(pKrDbgInfoG->m_pProcess->m_hanlde);
        kr_free(&(pKrDbgInfoG->m_pProcess));

        while (pKrDbgInfoG->m_pThreads != NULL) {
                krThreadInfo *pVisitor = pKrDbgInfoG->m_pThreads;
                pKrDbgInfoG->m_pThreads = pVisitor->m_next;

                CloseHandle(pVisitor->m_handle);
                kr_free(&pVisitor);
        }

        return wtsuccess;
}

wtwchar_t *kr_getModuleFullPathSub(wtwchar_t *pNtName)
{
        wtwchar_t  szDrive[_MAX_DRIVE] = L" :";
        wtwchar_t  szName[_MAX_PATH] = L"";
        wtwchar_t  szFullPath[_MAX_PATH] = L"";
        wtuint32_t nNameLen = 0;

        for (szDrive[0] = L'A'; szDrive[0] <= L'Z'; szDrive[0]++) {
                if (QueryDosDeviceW(szDrive, szName, MAX_PATH) > 0) {
                        nNameLen = wcslen(szName);
                        if (_wcsnicmp(szName, pNtName, nNameLen) == 0 
                                        && pNtName[nNameLen] == L'\\') {
                                _snwprintf(szFullPath, _MAX_PATH - 1, L"%s%s",
                                                szDrive, &pNtName[nNameLen]);
                                return kr_wstrdup(szFullPath);
                        }
                } else {
                        if (GetLastError() == ERROR_FILE_NOT_FOUND) {
                                continue;
                        } else {
                                break;
                        }
                }
        }
        return NULL;
}

wtwchar_t *kr_getModuleFullPath(wtpvoid_t pFile)
{
        wtwchar_t  szNtNameBuffer[_MAX_PATH * 2];

        if (pFile == NULL) {
                return NULL;
        }

        if (!GetMappedFileNameW(GetCurrentProcess(), pFile, szNtNameBuffer, _MAX_PATH * 2)) {
                return NULL;
        }

        return kr_getModuleFullPathSub(szNtNameBuffer);
}

wtint32_t kr_analyzeModule(krModuleInfo *pModule, const wtpvoid_t pFile)
{
        PIMAGE_DOS_HEADER     pDosHeader;
        PIMAGE_NT_HEADERS     pNtHeader;
        PIMAGE_SECTION_HEADER pSectionHeader;

        if (pModule == NULL || pFile == NULL) {
                return wtfailure;
        }

        pDosHeader = pFile;
        if (pDosHeader->e_magic != IMAGE_DOS_SIGNATURE) {
                // check MZ flag
                return wtfailure;
        }

        pNtHeader = (wtpvoid_t)((wtchar_t *)pFile + pDosHeader->e_lfanew);
        if (pNtHeader->Signature != IMAGE_NT_SIGNATURE) {
                // check PE00 flag
                return wtfailure;
        }

        if (pNtHeader->FileHeader.Machine != IMAGE_FILE_MACHINE_I386) {
                // check machine code
                return wtfailure;
        }

        pModule->m_imageInfo.m_endVA = (wtwchar_t *)pModule->m_imageInfo.m_startVA
                + pNtHeader->OptionalHeader.SizeOfImage;

        pSectionHeader = (wtpvoid_t)&pNtHeader[1];
        for (wtuint32_t i = 0; i < pNtHeader->FileHeader.NumberOfSections; i++) {
                wtpvoid_t ptr = &pSectionHeader[i];
                // TODO: get more info from PE file
        }

        return wtsuccess;
}

wtint32_t kr_addModuleInfo(HANDLE hFile, wtpvoid_t baseVA)
{
        krModuleInfo *pVisitor;
        krModuleInfo *pModuleInfo = NULL;

        HANDLE        hMap = NULL;
        wtpvoid_t     pFile = NULL;
        wtwchar_t    *pModuleName = NULL;

        wtint32_t     nRet = wtfailure;

        // param check
        if (hFile == INVALID_HANDLE_VALUE || baseVA == NULL) {
                goto l_ret;
        }

        // get PE file ptr
        hMap = CreateFileMappingW(hFile, NULL, PAGE_READONLY, 0, 0, NULL);
        if (hMap == NULL) {
                goto l_ret;
        }
        pFile = MapViewOfFile(hMap, FILE_MAP_READ, 0, 0, 0);
        if (pFile == NULL) {
                goto l_ret;
        }

        // get module full path
        pModuleName = kr_getModuleFullPath(pFile);
        if (pModuleName == NULL) {
                goto l_ret;
        }

        // malloc
        pModuleInfo = kr_malloc(sizeof(krModuleInfo));
        if (pModuleInfo == NULL) {
                goto l_ret;
        }

        // copy info
        pModuleInfo->m_modulePath = pModuleName;
        pModuleInfo->m_imageInfo.m_startVA = baseVA;
        pModuleInfo->m_next = NULL;

        // get more info
        nRet = kr_analyzeModule(pModuleInfo, pFile);
        if (nRet == wtfailure) {
                goto l_ret;
        }

        // search the end point and insert
        pVisitor = pKrDbgInfoG->m_pModules;
        if (pVisitor == NULL) {
                pKrDbgInfoG->m_pModules = pModuleInfo;
        } else {
                while (pVisitor->m_next != NULL) {
                        pVisitor = pVisitor->m_next;
                }
                pVisitor->m_next = pModuleInfo;
        }

l_ret:
        if (nRet == wtfailure) {
                kr_free(&(pModuleInfo->m_modulePath));
                kr_free(&pModuleInfo);
        }

        if (pFile != NULL) {
                UnmapViewOfFile(pFile);
        }
        if (hMap != NULL) {
                CloseHandle(hMap);
        }
        return nRet;
}

wtint32_t kr_delModuleInfo(const wtpvoid_t baseVA)
{
        krModuleInfo *pVisitor;
        krModuleInfo *pLast;

        // param check
        if (baseVA == NULL) {
                return wtfailure;
        }

        pVisitor = pKrDbgInfoG->m_pModules;
        if (pVisitor == NULL) {
                return wtfailure;
        } else if (pVisitor->m_imageInfo.m_startVA == baseVA) {
                // del the first node
                pKrDbgInfoG->m_pModules = pVisitor->m_next;

                kr_free(&pVisitor);
                return wtsuccess;
        } else if (pVisitor->m_next != NULL) {
                do {
                        pLast = pVisitor;
                        pVisitor = pVisitor->m_next;

                        if (pVisitor->m_imageInfo.m_startVA == baseVA) {
                                pLast->m_next = pVisitor->m_next;

                                kr_free(&pVisitor);
                                return wtsuccess;
                        }
                } while (pVisitor->m_next != NULL);
        }
        
        return wtfailure;
}

wtint32_t kr_addThreadInfo(HANDLE hThread)
{
        krThreadInfo *pVisitor;
        krThreadInfo *pThreadInfo = NULL;

        wtint32_t     nRet = wtfailure;

        // param check
        if (hThread == INVALID_HANDLE_VALUE) {
                return wtfailure;
        }

        // malloc
        pThreadInfo = kr_malloc(sizeof(krThreadInfo));
        if (pThreadInfo == NULL) {
                return wtfailure;
        }

        // copy info
        pThreadInfo->m_handle = hThread;
        pThreadInfo->m_dwId = GetThreadId(hThread);
        pThreadInfo->m_next = NULL;

        // search the end point and insert
        pVisitor = pKrDbgInfoG->m_pThreads;
        if (pVisitor == NULL) {
                pKrDbgInfoG->m_pThreads = pThreadInfo;
        } else {
                while (pVisitor->m_next != NULL) {
                        pVisitor = pVisitor->m_next;
                }
                pVisitor->m_next = pThreadInfo;
        }
        return wtsuccess;
}

wtint32_t kr_delThreadInfo(DWORD dwExitCode)
{
        krThreadInfo *pVisitor;
        krModuleInfo *pLast;
        DWORD         dwCode;
        
        pVisitor = pKrDbgInfoG->m_pThreads;
        if (pVisitor == NULL) {
                return wtfailure;
        } else if (GetExitCodeThread(pVisitor->m_handle, &dwCode)
                && dwCode == dwExitCode && dwCode != STILL_ACTIVE) {
                pKrDbgInfoG->m_pThreads = pVisitor->m_next;

                CloseHandle(pVisitor->m_handle);
                kr_free(&pVisitor);
                return wtsuccess;
        } else if (pVisitor->m_next != NULL) {
                do {
                        pLast = pVisitor;
                        pVisitor = pVisitor->m_next;

                        if (GetExitCodeThread(pVisitor->m_handle, &dwCode)
                                && dwCode == dwExitCode && dwCode != STILL_ACTIVE) {
                                pLast->m_next = pVisitor->m_next;

                                CloseHandle(pVisitor->m_handle);
                                kr_free(&pVisitor);
                                return wtsuccess;
                        }
                } while (pVisitor->m_next != NULL);
        }

        return wtfailure;
}