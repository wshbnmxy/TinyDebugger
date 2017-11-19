#include <krevent.h>
#include <krerror.h>
#include <krinfo.h>
#include <krutil.h>

#define   KR_EVENT_CONTINUE      0
#define   KR_EVENT_SUSPEND       1
#define   KR_EVENT_ABORT         2

wtint32_t kr_event(event_cb handers[])
{
        DEBUG_EVENT debugEv;
        wtuint32_t  dwEventCode;
        wtuint32_t  dwContStatus = DBG_CONTINUE;

        while (wttrue) {
                WaitForDebugEvent(&debugEv, INFINITE);
                dwEventCode = debugEv.dwDebugEventCode;
                
                if (dwEventCode <= EXCEPTION_DEBUG_EVENT - 1 || dwEventCode >= RIP_EVENT + 1) {
                        return wtfailure;
                }
                if (handers[dwEventCode] == NULL) {
                        return wtfailure;
                }

                switch(handers[dwEventCode](&debugEv.u, &dwContStatus)) {
                case KR_EVENT_CONTINUE:
                        break;
                case KR_EVENT_SUSPEND:
                        return wtsuccess;
                case KR_EVENT_ABORT:
                        return wtfailure;
                default:
                        return wtfailure;
                }
                ContinueDebugEvent(debugEv.dwProcessId, debugEv.dwThreadId, dwContStatus);
        }
}

#define KR_MK_DEBUG_EVENT(event)    event

wtint32_t kr_exception_cb(LPEXCEPTION_DEBUG_INFO pInfo, wtuint32_t *pContStatus)
{
        KR_MK_DEBUG_EVENT(EXCEPTION_DEBUG_EVENT);
        *pContStatus = DBG_CONTINUE;

        if (pInfo->dwFirstChance &&
                (pInfo->ExceptionRecord.ExceptionCode == EXCEPTION_BREAKPOINT || pInfo->ExceptionRecord.ExceptionCode == EXCEPTION_SINGLE_STEP)) {
                // first chance exception to handle exception
                
        } else if (pInfo->dwFirstChance) {
                // first chance exception
                *pContStatus = DBG_EXCEPTION_HANDLED;
        } else {
                // seconde chance exception
        }
        return KR_EVENT_CONTINUE;
}

wtint32_t kr_createthread_cb(LPCREATE_THREAD_DEBUG_INFO pInfo, wtuint32_t *pContStatus)
{
        KR_MK_DEBUG_EVENT(CREATE_THREAD_DEBUG_EVENT);
        wtint32_t nRet = wtfailure;
        *pContStatus = DBG_CONTINUE;

        nRet = kr_addThreadInfo(pInfo->hThread);
        if (nRet == wtfailure) {
                CloseHandle(pInfo->hThread);
        }
        return nRet == wtsuccess ? KR_EVENT_CONTINUE : KR_EVENT_ABORT;
}

wtint32_t kr_createprocess_cb(LPCREATE_PROCESS_DEBUG_INFO pInfo, wtuint32_t *pContStatus)
{
        KR_MK_DEBUG_EVENT(CREATE_PROCESS_DEBUG_EVENT);

        wtint32_t nRet = wtfailure;
        *pContStatus = DBG_CONTINUE;

        nRet = kr_initInfo(pInfo);
        if (nRet == wtfailure) {
                CloseHandle(pInfo->hProcess);
                CloseHandle(pInfo->hThread);
        }
        CloseHandle(pInfo->hFile);
        return nRet == wtsuccess ? KR_EVENT_CONTINUE : KR_EVENT_ABORT;
}

wtint32_t kr_exitthread_cb(LPEXIT_THREAD_DEBUG_INFO pInfo, wtuint32_t *pContStatus)
{
        KR_MK_DEBUG_EVENT(EXIT_THREAD_DEBUG_EVENT);
        *pContStatus = DBG_CONTINUE;
        
        kr_delModuleInfo(pInfo->dwExitCode);
        return KR_EVENT_CONTINUE;
}

wtint32_t kr_exitprocess_cb(LPEXIT_PROCESS_DEBUG_INFO pInfo, wtuint32_t *pContStatus)
{
        KR_MK_DEBUG_EVENT(EXIT_PROCESS_DEBUG_EVENT);
        *pContStatus = DBG_CONTINUE;

        kr_uninitInfo();
        return KR_EVENT_SUSPEND;
}

wtint32_t kr_loaddll_cb(LPLOAD_DLL_DEBUG_INFO pInfo, wtuint32_t *pContStatus)
{
        KR_MK_DEBUG_EVENT(LOAD_DLL_DEBUG_EVENT);

        wtint32_t nRet = wtfailure;        
        *pContStatus = DBG_CONTINUE;

        nRet = kr_addModuleInfo(pInfo->hFile, pInfo->lpBaseOfDll);

        CloseHandle(pInfo->hFile);
        return nRet == wtsuccess ? KR_EVENT_CONTINUE : KR_EVENT_ABORT;
}

wtint32_t kr_unloaddll_cb(LPUNLOAD_DLL_DEBUG_INFO pInfo, wtuint32_t *pContStatus)
{        
        KR_MK_DEBUG_EVENT(UNLOAD_DLL_DEBUG_EVENT);

        kr_delModuleInfo(pInfo->lpBaseOfDll);
        *pContStatus = DBG_CONTINUE;
        return KR_EVENT_CONTINUE;
}

#undef KR_MK_DEBUG_EVENT

event_cb  contEvtCbs[] = {
        NULL,
        kr_exception_cb,
        kr_createthread_cb,
        kr_createprocess_cb,
        kr_exitthread_cb,
        kr_exitprocess_cb,
        kr_loaddll_cb,
        kr_unloaddll_cb,
        NULL,
        NULL,
};