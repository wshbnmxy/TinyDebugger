#ifndef _KR_DATA_H_
#define _KR_DATA_H_

#include <krcommon.h>
#include <Windows.h>

typedef struct _kr_processinfo_s {
        DWORD   m_dwId;
        HANDLE  m_hanlde;
}krProcessInfo;

typedef struct _kr_threadinfo_s {
        DWORD   m_dwId;
        HANDLE  m_handle;
        struct  _kr_threadinfo_s *m_next;
}krThreadInfo;

typedef struct _kr_moduleinfo_s {
        wtwchar *m_modulePath;
        struct {
                wtuint_t m_startRVA;
                wtuint_t m_endRVA;
        }        m_imageInfo;
        struct _kr_moduleinfo_s *m_next;
}krModuleInfo;

typedef struct _kr_dbginfo_s {
        wtwchar        *m_exePath;
        krProcessInfo  *m_pProcess;
        krThreadInfo   *m_pThreads;
        krModuleInfo   *m_pModules;
}krDbgInfo;

extern krDbgInfo *pKrDbgInfoG;

wtint32_t kr_initDebugInfo(wtwchar *exepath, PROCESS_INFORMATION info);
wtint32_t kr_uninitDebugInfo();
wtpvoid   kr_insertModuleInfo(wtwchar *modulepath, wtuint_t baseRVA);
#endif // !_KR_DATA_H_
