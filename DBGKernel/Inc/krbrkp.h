#ifndef _KR_BRKP_H_
#define _KR_BRKP_H_

#include <krcommon.h>

// breakpoint info
typedef struct _kr_brkpointinfo_s {
        wtpvoid        m_brkpVA;     // breakpoint va
        wtwchar       *m_funcName;   // breakpoint in func, may NULL if not found
        wtwchar       *m_fileName;   // breakpoint in file name, may NULL if not found
        wtuint32_t     m_lineNum;    // breakpoint in file line num, may 0 if not found
        wtuint8_t      m_opCode;     // breakpoint origin op code
        wtuint32_t     m_hitCount;   // breakpoint hitCount

        struct _kr_brkpointinfo_s *m_next;
                                     // link list next ptr
}krBrkPointInfo;

#endif // !_KR_BRKP_H_