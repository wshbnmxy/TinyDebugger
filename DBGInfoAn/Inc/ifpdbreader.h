#ifndef _IF_PDB_READER_H_
#define _IF_PDB_READER_H_

#include <ifcommon.h>
#include <iferror.h>
#include <ifinfo.h>

#include <Windows.h>
#include <dia\dia2.h>
#include <ifpdbcallback.h>


namespace wt {

class PdbReader
{
public:
        PdbReader(wtwchar_t *szModuleName, DbgInfo *pDbgInfo);
        ~PdbReader();

private:
        HRESULT         m_hr;
        IDiaDataSource *m_pDiaDataSource;
        IDiaSession    *m_pDiaSession;
        IDiaSymbol     *m_pDiaGSymbol;

        DbgInfo        *m_pDbgInfo;

private:
        wtuint32_t if_pdbDumpAll();
        wtuint32_t if_pdbDumpCompiland(IDiaSymbol * &pDiaSymCompiland);
        wtuint32_t if_pdbDumpSourceFile(IDiaSourceFile * &pDiaSourceFile);
        wtuint32_t if_pdbDumpFunc(IDiaSymbol * &pDiaSymFunction);

        wtuint32_t if_pdbDumpLines(IDiaEnumLineNumbers *&pDiaEnumLines, DbgFuncInfo *pFuncInfo);

private:
        wtbool_t   if_pdbIsCompilandIsSource(IDiaSymbol * &pDiaSymCompiland);
};
}

#endif // !_IF_PDB_READER_H_
