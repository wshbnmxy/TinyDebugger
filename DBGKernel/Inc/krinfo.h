#ifndef _KR_INFO_H_
#define _KR_INFO_H_

#include <krcommon.h>
#include <Windows.h>

// exe info
typedef struct _kr_processinfo_s {
        DWORD   m_dwId;                   // process id
        HANDLE  m_hanlde;                 // process handle
}krProcessInfo;

typedef struct _kr_threadinfo_s {
        DWORD   m_dwId;                   // thread id
        HANDLE  m_handle;                 // thread handle
        struct  _kr_threadinfo_s *m_next; // link list next ptr
}krThreadInfo;

typedef struct _kr_moduleinfo_s {
        wtwchar *m_modulePath;            // module name full path
        struct {
                wtpvoid m_startVA;        // start va
                wtpvoid m_endVA;          // end va
        }        m_imageInfo;
        struct _kr_moduleinfo_s *m_next;  // link list next ptr
}krModuleInfo;

typedef struct _kr_dbginfo_s {
        wtwchar        *m_exePath;        // exe file name full path
        krProcessInfo  *m_pProcess;
        krThreadInfo   *m_pThreads;
        krModuleInfo   *m_pModules;
}krDbgInfo;

extern krDbgInfo *pKrDbgInfoG;

wtint32_t kr_initDebugInfo(wtwchar *exepath, PROCESS_INFORMATION info);
wtint32_t kr_uninitDebugInfo();
wtpvoid   kr_insertModuleInfo(wtwchar *modulepath, wtpvoid baseVA);
#endif // !_KR_INFO_H_