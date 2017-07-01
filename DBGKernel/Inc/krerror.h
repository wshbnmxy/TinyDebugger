#ifndef _KR_ERROR_H_
#define _KR_ERROR_H_

#include "..\..\Share\Inc\shtype.h"

#include <setjmp.h>

extern wtint32_t kr_errno;

#define KR_PARAM_ERROR             1
#define KR_MEMORY_ERROR            2
#define KR_MEMORY_SMALL_ERROR      3

#define KR_EXEFILE_NOT_FOUND     101
#define KR_EXEWF_NOT_FOUND       102

#define KR_EVENT_UNKNOW          201
#define KR_EVENT_HANDLER_NOT_SET 202

#define KR_INVAILD_PEFILE        251

#define KR_NOT_EXPECT_ERROR      9999

wtvoid kr_jmpWithError(wtint32_t wterrno);

#endif // !_KR_ERROR_H_
