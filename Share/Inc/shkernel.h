#ifndef _SH_KERNEL_H_
#define _SH_KERNEL_H_

#include "shtype.h"

typedef struct _kr_execinfo_s {
        wtwchar_t *m_exepath;  // exe file full path
        wtwchar_t *m_exeargs;  // exec arguments
        wtwchar_t *m_execwf;   // work folder full path
        wtwchar_t *m_execenv;  // environment format: key1=value1\0key2=value2\0..keyn=valuen\0\0
}krExecInfo;

#define KR_SBRK_TYPE_ADDR       1
#define KR_SBRK_TYPE_FUNCNAME   2
#define KR_SBRK_TYPE_LINENUM    3

typedef struct _kr_sbrkinfo_s {
        wtuint16_t m_brktype;           // sbrk type
        union {
                wtpvoid_t  m_addr;        // type addr
                wtwchar_t *m_funcname;    // type func name
                struct {
                        wtwchar_t *m_filename;
                        wtuint32_t m_linenum;
                }        m_lineinfo;    // type line num
        };
}krSbrkInfo;

wtint32_t kr_exec(krExecInfo *);
wtint32_t kr_sbrk(krSbrkInfo *);
wtint32_t kr_errif();

#endif // !_SH_KERNEL_H_
