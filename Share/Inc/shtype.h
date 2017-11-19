#ifndef _SH_TYPE_H_
#define _SH_TYPE_H_

#include <stdio.h>

typedef char             wtint8_t;
typedef unsigned char    wtuint8_t;

typedef short            wtint16_t;
typedef unsigned short   wtuint16_t;

typedef int              wtint32_t;
typedef unsigned int     wtuint32_t;

typedef __int64          wtint64_t;
typedef unsigned __int64 wtuint64_t;

typedef wtint32_t        wtint_t;
typedef wtuint32_t       wtuint_t;

typedef long             wtlong_t;
typedef unsigned long    wtulong_t;

typedef float            wtfloat_t;
typedef double           wtdouble_t;

typedef char             wtchar_t;
typedef wchar_t          wtwchar_t;
typedef wtint8_t         wtbool_t;

typedef void             wtvoid_t;
typedef wtvoid_t *       wtpvoid_t;

#define wttrue           (wtbool_t)1
#define wtfalse          (wtbool_t)0

#define wtsuccess        (wtint8_t)0
#define wtfailure        (wtint8_t)1


#endif // !_SH_TYPE_H_
