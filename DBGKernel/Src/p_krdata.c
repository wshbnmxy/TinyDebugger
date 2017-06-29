#include <krdata.h>
#include <krerror.h>

#include <string.h>
#include <Windows.h>

krDbgInfo  krDbgInfoG;
krDbgInfo *pKrDbgInfoG = &krDbgInfoG;

wtint32_t kr_initdebuginfo(wtwchar *exepath, PROCESS_INFORMATION info) {

        kr_errno = 0;
        ZeroMemory(pKrDbgInfoG, sizeof(krDbgInfo));

        // param check
        if (exepath == NULL) {
                kr_errno = KR_PARAM_ERROR;
                goto l_ret;
        }

        // malloc
        pKrDbgInfoG->m_exePath  = malloc(sizeof(wtwchar) * wcslen(exepath));
        pKrDbgInfoG->m_pProcess = malloc(sizeof(krProcessInfo));
        pKrDbgInfoG->m_pThreads = malloc(sizeof(krThreadInfo));

        if (pKrDbgInfoG->m_exePath  == NULL ||
            pKrDbgInfoG->m_pProcess == NULL ||
            pKrDbgInfoG->m_pThreads == NULL) {
                kr_errno = KR_MEMORY_ERROR;
                goto l_ret;
        }
        
        // copy info
        wcscpy(pKrDbgInfoG->m_exePath, exepath);
        pKrDbgInfoG->m_pProcess->m_dwId   = info.dwProcessId;
        pKrDbgInfoG->m_pProcess->m_hanlde = info.hProcess;

        pKrDbgInfoG->m_pThreads->m_dwId   = info.dwThreadId;
        pKrDbgInfoG->m_pThreads->m_handle = info.hThread;
        pKrDbgInfoG->m_pThreads->m_next   = NULL;
        
        return kr_errno;
l_ret:
        if (pKrDbgInfoG->m_exePath != NULL) {
                free(pKrDbgInfoG->m_exePath);
                pKrDbgInfoG->m_exePath = NULL;
        }
        if (pKrDbgInfoG->m_pProcess != NULL) {
                free(pKrDbgInfoG->m_pProcess);
                pKrDbgInfoG->m_pProcess = NULL;
        }
        if (pKrDbgInfoG->m_pThreads != NULL) {
                free(pKrDbgInfoG->m_pThreads);
                pKrDbgInfoG->m_pThreads = NULL;
        }
        return kr_errno;
}

wtpvoid kr_insertmoduleinfo(wtwchar *modulepath, wtuint_t baseRVA) {

        kr_errno = 0;
        krModuleInfo *pVisitor;
        krModuleInfo *pModuleInfo = NULL;

        // param check
        if (modulepath == NULL) {
                kr_errno = KR_PARAM_ERROR;
                goto l_ret;
        }

        // malloc
        pModuleInfo = malloc(sizeof(krModuleInfo));
        if (pModuleInfo == NULL) {
                kr_errno = KR_MEMORY_ERROR;
                goto l_ret;
        }

        // copy info
        ZeroMemory(pModuleInfo, sizeof(krModuleInfo));
        pModuleInfo->m_modulePath = malloc(sizeof(wtwchar) * wcslen(modulepath));
        if (pModuleInfo->m_modulePath == NULL) {
                kr_errno = KR_MEMORY_ERROR;
                goto l_ret;
        }
        wcscpy(pModuleInfo->m_modulePath, modulepath);
        pModuleInfo->m_imageInfo.m_startRVA = baseRVA;
        
        pModuleInfo->m_next = NULL;

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

        return pModuleInfo;
l_ret:
        if (pModuleInfo != NULL) {
                free(pModuleInfo);
        }
        return NULL;
}