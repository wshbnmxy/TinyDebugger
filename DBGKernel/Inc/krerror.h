#ifndef _KR_ERROR_H_
#define _KR_ERROR_H_

#include "..\..\Share\Inc\shtype.h"
#include "..\..\Share\Inc\sherror.h"

#include <setjmp.h>

extern wtint32_t kr_errno;

wtvoid kr_jmpWithError(wtint32_t wterrno);

#endif // !_KR_ERROR_H_
