#include <krevent.h>
#include <krerror.h>
#include <krinfo.h>
#include <krutil.h>

wtint32_t kr_event(event_cb handers[]) {

        DEBUG_EVENT debugEv;
        wtuint32_t  dwEventCode;
        wtuint32_t  dwContStatus = DBG_CONTINUE;

        kr_errno = 0;
        while (wttrue) {
                WaitForDebugEvent(&debugEv, INFINITE);
                dwEventCode = debugEv.dwDebugEventCode;
                
                if (dwEventCode == EXCEPTION_DEBUG_EVENT - 1 || dwEventCode >= RIP_EVENT + 1) {
                        kr_jmpWithError(kr_errno = WT_EVENT_UNKNOW);
                }
                if (handers[dwEventCode] == NULL) {
                        kr_jmpWithError(kr_errno = WT_EVENT_HANDLER_NOT_SET);
                }

                if( !handers[dwEventCode](&debugEv.u, &dwContStatus)) {
                        return 0;
                }
                ContinueDebugEvent(debugEv.dwProcessId, debugEv.dwThreadId, dwContStatus);
        }
}

#define KR_MK_DEBUG_EVENT(event)    event

wtbool kr_exception_cb(LPEXCEPTION_DEBUG_INFO pInfo, wtuint32_t *pContStatus) {

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
        return wttrue;
}

wtbool kr_createprocess_cb(LPCREATE_PROCESS_DEBUG_INFO pInfo, wtuint32_t *pContStatus) {

        KR_MK_DEBUG_EVENT(CREATE_PROCESS_DEBUG_EVENT);
        kr_errno = 0;

        CloseHandle(pInfo->hProcess);
        CloseHandle(pInfo->hThread);

        // no need to CloseHandle(hFile)
        // because of in function kr_analyzemodule
        // CloseHanlde must be called
        kr_analyzeModule(pInfo->hFile, pInfo->lpBaseOfImage);
        *pContStatus = DBG_CONTINUE;
        return wttrue;
}

wtbool kr_loaddll_cb(LPLOAD_DLL_DEBUG_INFO pInfo, wtuint32_t *pContStatus) {

        KR_MK_DEBUG_EVENT(LOAD_DLL_DEBUG_EVENT);
        kr_errno = 0;

        // no need to CloseHandle(hFile)
        // because of in function kr_analyzemodule
        // CloseHanlde must be called
        kr_analyzeModule(pInfo->hFile, pInfo->lpBaseOfDll);
        *pContStatus = DBG_CONTINUE;
        return wttrue;
}

wtbool kr_unloaddll_cb(LPUNLOAD_DLL_DEBUG_INFO pInfo, wtuint32_t *pContStatus) {
        
        KR_MK_DEBUG_EVENT(LOAD_DLL_DEBUG_EVENT);
        kr_errno = 0;

        *pContStatus = DBG_CONTINUE;
        return wttrue;
}

#undef KR_MK_DEBUG_EVENT

event_cb  cbExecEvent[] = {
        NULL,
        kr_exception_cb,
        NULL,
        kr_createprocess_cb,
        NULL,
        NULL,
        kr_loaddll_cb,
        kr_unloaddll_cb,
        NULL,
        NULL,
};