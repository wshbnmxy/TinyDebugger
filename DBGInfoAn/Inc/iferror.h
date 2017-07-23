#ifndef _IF_ERROR_H_
#define _IF_ERROR_H_

#include <ifcommon.h>

namespace wt {

class IfError {
public:
        IfError() throw() :m_error(WT_NOT_EXPECT_ERROR){}

        IfError(wtuint32_t error) throw() :m_error(error) {}

        inline wtuint32_t if_getError() const {
                return m_error;
        }
private:
        wtuint32_t m_error;
};

class IfParamError :public IfError{
public:
        IfParamError() throw() :IfError(WT_PARAM_ERROR) {}
};

class IfNoMemoryError :public IfError {
public:
        IfNoMemoryError() throw() :IfError(WT_MEMORY_ERROR) {}
};

class IfDiaComError :public IfError {
public:
        IfDiaComError() throw() :IfError(WT_DIACOM_ERROR) {}
};

}

#endif // !_IF_ERROR_H_