#ifndef _KR_EVENT_H_
#define _KR_EVENT_H_

#include <krcommon.h>
#include <krerror.h>

#include <Windows.h>

typedef   wtbool (*event_cb)(wtvoid *, wtuint32_t *);
wtint32_t kr_event(event_cb handers[]);


#endif // !_KR_EVENT_H_
