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

typedef long             wtlong;
typedef unsigned long    wtulong;

typedef float            wtfloat;
typedef double           wtdouble;

typedef char             wtchar;
typedef wchar_t          wtwchar;
typedef wtint8_t         wtbool;

typedef void             wtvoid;
typedef wtvoid *         wtpvoid;

#define wttrue           (wtbool)1
#define wtfalse          (wtbool)0


#endif // !_SH_TYPE_H_
