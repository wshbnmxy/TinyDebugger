#ifndef _KR_UTIL_H_
#define _KR_UTIL_H_

#include <krcommon.h>

wtpvoid_t kr_wstrdup(const wtwchar_t *src);
wtpvoid_t kr_malloc(const wtuint32_t len);
wtvoid_t  kr_free(wtpvoid_t *mem);

#endif // !_KR_UTIL_H_
