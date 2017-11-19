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
        wtwchar_t *m_modulePath;          // module name full path
        struct {
                wtpvoid_t m_startVA;      // start va
                wtpvoid_t m_endVA;        // end va
        }        m_imageInfo;
        wtpvoid_t  m_pdbInfo;
        struct _kr_moduleinfo_s *m_next;  // link list next ptr
}krModuleInfo;

typedef struct _kr_dbginfo_s {
        krProcessInfo  *m_pProcess;
        krThreadInfo   *m_pThreads;
        krModuleInfo   *m_pModules;
}krDbgInfo;

extern krDbgInfo *pKrDbgInfoG;

wtint32_t kr_addModuleInfo(HANDLE hFile, const wtpvoid_t baseVA);
wtint32_t kr_delModuleInfo(const wtpvoid_t baseVA);

wtint32_t kr_addThreadInfo(HANDLE hThread);
wtint32_t kr_delThreadInfo(DWORD dwExitCode);

wtint32_t kr_initInfo(LPCREATE_PROCESS_DEBUG_INFO pCreateInfo);
wtint32_t kr_uninitInfo();
#endif // !_KR_INFO_H_