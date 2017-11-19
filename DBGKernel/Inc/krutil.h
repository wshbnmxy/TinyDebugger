#ifndef _KR_UTIL_H_
#define _KR_UTIL_H_

#include <krcommon.h>

wtpvoid_t kr_wstrdup(const wtwchar_t *src);
wtpvoid_t kr_malloc(const wtuint32_t len);
wtvoid_t  kr_free(wtpvoid_t *mem);

wtint32_t kr_readMemory(HANDLE hProcess, const wtpvoid_t nBase, const wtuint32_t nSize, wtpvoid_t pOut);
wtint32_t kr_writeMemory(HANDLE hProcess, const wtpvoid_t nBase, const wtuint32_t nSize, const wtpvoid_t pIn);

wtint32_t kr_readRegisters(HANDLE hThread, LPCONTEXT pRegisters);
wtint32_t kr_writeRegisters(HANDLE hThread, const LPCONTEXT pRegisters);

#endif // !_KR_UTIL_H_
