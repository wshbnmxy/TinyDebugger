#include <krcommon.h>
#include <krerror.h>

wtint32_t kr_errno = 0;
jmp_buf   kr_jmppoint;

wtint32_t kr_errif() {
        return kr_errno;
}

wtvoid kr_jmpwitherror(wtint32_t wterrno) {
        longjmp(kr_jmppoint, wterrno);
}