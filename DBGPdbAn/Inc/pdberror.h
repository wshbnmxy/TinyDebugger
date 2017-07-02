#ifndef _PDB_ERROR_H_
#define _PDB_ERROR_H_

#include <pdbcommon.h>

namespace wt {

class PdbError {
public:
        PdbError() throw() :m_error(WT_NOT_EXPECT_ERROR){}

        PdbError(wtuint32_t error) throw() :m_error(error) {}

        inline wtuint32_t pdb_getError() const {
                return m_error;
        }
private:
        wtuint32_t m_error;
};

class PdbParamError :public PdbError{
public:
        PdbParamError() throw() :PdbError(WT_PARAM_ERROR) {}
};

class PdbDiaComError :public PdbError {
public:
        PdbDiaComError() throw() :PdbError(WT_DIACOM_ERROR) {}
};

}

#endif // !_PDB_ERROR_H_