#include <krinfo.h>
#include <krerror.h>

#include <string.h>
#include <Windows.h>

static krDbgInfo  krDbgInfoG;
static krDbgInfo *pKrDbgInfoG = &krDbgInfoG;

wtint32_t kr_initDebugInfo(wtwchar *exepath, PROCESS_INFORMATION info) {

        kr_errno = 0;
        ZeroMemory(pKrDbgInfoG, sizeof(krDbgInfo));

        // param check
        if (exepath == NULL) {
                kr_errno = WT_PARAM_ERROR;
                goto l_ret;
        }

        // malloc
        pKrDbgInfoG->m_exePath  = malloc(sizeof(wtwchar) * (wcslen(exepath) + 1));
        pKrDbgInfoG->m_pProcess = malloc(sizeof(krProcessInfo));
        pKrDbgInfoG->m_pThreads = malloc(sizeof(krThreadInfo));

        if (pKrDbgInfoG->m_exePath  == NULL ||
            pKrDbgInfoG->m_pProcess == NULL ||
            pKrDbgInfoG->m_pThreads == NULL) {
                kr_errno = WT_MEMORY_ERROR;
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

wtint32_t kr_uninitDebugInfo() {
        
        if (pKrDbgInfoG->m_exePath != NULL) {
                free(pKrDbgInfoG->m_exePath);
                pKrDbgInfoG->m_exePath = NULL;
        }

        while (pKrDbgInfoG->m_pModules != NULL) {
                krModuleInfo *pVisitor = pKrDbgInfoG->m_pModules;
                pKrDbgInfoG->m_pModules = pVisitor->m_next;
                
                if (pVisitor->m_modulePath != NULL) {
                        free(pVisitor->m_modulePath);
                }
                free(pVisitor);
        }
        pKrDbgInfoG->m_pModules = NULL;

        if (pKrDbgInfoG->m_pProcess != NULL) {
                free(pKrDbgInfoG->m_pProcess);
                pKrDbgInfoG->m_pProcess = NULL;
        }

        while (pKrDbgInfoG->m_pThreads != NULL) {
                krThreadInfo *pVisitor = pKrDbgInfoG->m_pThreads;
                pKrDbgInfoG->m_pThreads = pVisitor->m_next;

                free(pVisitor);
        }
        pKrDbgInfoG->m_pThreads = NULL;

        return kr_errno;
}

wtpvoid kr_insertModuleInfo(wtwchar *modulepath, wtpvoid baseVA) {

        kr_errno = 0;
        krModuleInfo *pVisitor;
        krModuleInfo *pModuleInfo = NULL;

        // param check
        if (modulepath == NULL) {
                kr_errno = WT_PARAM_ERROR;
                goto l_ret;
        }

        // malloc
        pModuleInfo = malloc(sizeof(krModuleInfo));
        if (pModuleInfo == NULL) {
                kr_errno = WT_MEMORY_ERROR;
                goto l_ret;
        }

        // copy info
        ZeroMemory(pModuleInfo, sizeof(krModuleInfo));
        pModuleInfo->m_modulePath = malloc(sizeof(wtwchar) * (wcslen(modulepath) + 1));
        if (pModuleInfo->m_modulePath == NULL) {
                kr_errno = WT_MEMORY_ERROR;
                goto l_ret;
        }
        wcscpy(pModuleInfo->m_modulePath, modulepath);
        pModuleInfo->m_imageInfo.m_startVA = baseVA;
        
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