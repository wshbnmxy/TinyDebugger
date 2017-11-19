#include <krcommon.h>
#include <krerror.h>
#include <krbrkp.h>

krBrkPointInfo *pKrDbgBrkpG = NULL;

wtint32_t kr_sbrk(krSbrkInfo *pInfo) {
        
        if (pInfo->m_brktype == KR_SBRK_TYPE_ADDR)
        return 0;
}