#ifndef _PDB_READER_H_
#define _PDB_READER_H_

#include <pdbcommon.h>
#include <dia2.h>

namespace wt {
class PdbReader
{
public:
        PdbReader(wtwchar *szModuleName);
        ~PdbReader();

private:
        HRESULT  m_hr;
        IDiaDataSource *m_pDiaDataSource;
        IDiaSession    *m_pDiaSession;
        IDiaSymbol     *m_pDiaGSymbol;
};
}

#endif // !_PDB_READER_H_