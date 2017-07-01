#include <krcommon.h>
#include <krerror.h>
#include <krevent.h>
#include <krdata.h>

#include <string.h>
#include <stdlib.h>
#include <Windows.h>

extern event_cb  cbExecEvent[];
extern jmp_buf   kr_jmppoint;

wtint32_t kr_exec(krExecInfo *pInfo) {

        wtint32_t     nRet    = 0;
        wtuint32_t    nCmd    = 0;
        wtwchar      *szCmdL  = NULL;

        STARTUPINFOW        startupInfo;
        PROCESS_INFORMATION processInfo;

        kr_errno = 0;

        // param check
        if (pInfo == NULL || pInfo->m_exepath == NULL) {
                return nRet = kr_errno = KR_PARAM_ERROR;
        }

        // set error jmp point
        nRet = setjmp(kr_jmppoint);
        if (nRet != 0) {
                goto l_ret;
        }

        // create command
        nCmd = wcslen(pInfo->m_exepath) + 3;
        if (pInfo->m_exeargs != NULL && wcslen(pInfo->m_exeargs)) {
                nCmd += (1 + wcslen(pInfo->m_exeargs));
        }
        szCmdL = malloc(sizeof(wtwchar) * nCmd);
        if (szCmdL == NULL) {
                return nRet = kr_errno = KR_MEMORY_ERROR;
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
                kr_errno = GetLastError();
                switch (kr_errno)
                {
                case ERROR_FILE_NOT_FOUND:
                        nRet = kr_errno = KR_EXEFILE_NOT_FOUND;break;
                case ERROR_DIRECTORY:
                        nRet = kr_errno = KR_EXEWF_NOT_FOUND;break;
                default:
                        nRet = KR_NOT_EXPECT_ERROR;break;
                }
                goto l_ret;
        }

        // set debug info
        if (nRet = kr_initDebugInfo(pInfo->m_exepath, processInfo)) {
                goto l_ret;
        }

        kr_event(cbExecEvent);

l_ret:
        if (szCmdL != NULL)     free(szCmdL);
        return nRet;
}