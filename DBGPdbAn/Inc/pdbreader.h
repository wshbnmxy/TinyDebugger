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

        wtuint32_t pdb_dumpAll();

private:
        wtuint32_t pdb_dumpObj(IDiaSymbol *pDiaSymCompiland);
        wtuint32_t pdb_dumpFun(IDiaSymbol *pDiaSymFunction);

private:
        HRESULT  m_hr;
        IDiaDataSource *m_pDiaDataSource;
        IDiaSession    *m_pDiaSession;
        IDiaSymbol     *m_pDiaGSymbol;
};
}

#endif // !_PDB_READER_H_