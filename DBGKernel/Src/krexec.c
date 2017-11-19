#include <krcommon.h>
#include <krerror.h>
#include <krevent.h>
#include <krinfo.h>
#include <krutil.h>

#include <string.h>
#include <stdlib.h>
#include <Windows.h>

extern event_cb  contEvtCbs[];
extern jmp_buf   kr_jmppoint;

wtint32_t kr_exec(krExecInfo *pInfo)
{
        wtint32_t     nRet    = wtfailure;
        wtuint32_t    nCmd    = 0;
        wtwchar_t    *szCmdL  = NULL;

        STARTUPINFOW        startupInfo;
        PROCESS_INFORMATION processInfo;

        // param check
        if (pInfo == NULL || pInfo->m_exepath == NULL) {
                return nRet;
        }

        // create command
        nCmd = wcslen(pInfo->m_exepath) + 3;
        if (pInfo->m_exeargs != NULL && wcslen(pInfo->m_exeargs)) {
                nCmd += (1 + wcslen(pInfo->m_exeargs));
        }
        szCmdL = malloc(sizeof(wtwchar_t) * nCmd);
        if (szCmdL == NULL) {
                return nRet;
        }
        if (pInfo->m_exeargs != NULL && wcslen(pInfo->m_exeargs)) {
                _swprintf(szCmdL, L"\"%s\" %s", pInfo->m_exepath, pInfo->m_exeargs);
        } else {
                _swprintf(szCmdL, L"\"%s\""   , pInfo->m_exepath);
        }

        // create debug process
        ZeroMemory(&startupInfo, sizeof(startupInfo));
        ZeroMemory(&processInfo, sizeof(processInfo));
        startupInfo.cb = sizeof(startupInfo);
        if (!CreateProcessW(NULL, szCmdL, NULL, NULL, TRUE,
                CREATE_DEFAULT_ERROR_MODE | CREATE_NEW_PROCESS_GROUP | CREATE_UNICODE_ENVIRONMENT | DEBUG_ONLY_THIS_PROCESS,
                pInfo->m_execenv, pInfo->m_execwf, &startupInfo, &processInfo)) {
                goto l_ret;
        }

        CloseHandle(processInfo.hProcess);
        CloseHandle(processInfo.hThread);

        nRet = kr_event(contEvtCbs);

l_ret:
        kr_free(&szCmdL);
        return nRet;
}