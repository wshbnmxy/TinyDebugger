#include <ifpdbreader.h>

namespace wt {

PdbReader::PdbReader(wtwchar* szModuleName, DbgInfo *pDbgInfo) :
        m_pDiaDataSource(NULL), m_pDiaSession(NULL), m_pDiaGSymbol(NULL),
        m_pDbgInfo(pDbgInfo) {
        CoInitialize(NULL);
        
       if (szModuleName == NULL || pDbgInfo == NULL) {
                throw wt::IfParamError();
        }

        m_hr = CoCreateInstance(__uuidof(DiaSource), NULL, CLSCTX_INPROC_SERVER, 
                        __uuidof(IDiaDataSource), (void **) &m_pDiaDataSource);
        if (FAILED(m_hr)) {
                throw wt::IfDiaComError();
        }

        wt::PdbCallback pdbCb;
        pdbCb.AddRef();
        m_hr = m_pDiaDataSource->loadDataForExe(szModuleName, NULL, &pdbCb);
        if (FAILED(m_hr)) {
                if (m_hr == E_PDB_NOT_FOUND || m_hr == E_PDB_INVALID_SIG) {
                        return;
                }
                throw wt::IfDiaComError();
        }

        m_hr = m_pDiaDataSource->openSession(&m_pDiaSession);
        if (FAILED(m_hr)) {
                throw wt::IfDiaComError();
        }

        m_hr = m_pDiaSession->get_globalScope(&m_pDiaGSymbol);
        if (FAILED(m_hr)) {
                throw wt::IfDiaComError();
        }

        if_pdbDumpAll();
}

PdbReader::~PdbReader() {
        if (m_pDiaGSymbol != NULL) {
                m_pDiaGSymbol->Release();
                m_pDiaGSymbol = NULL;
        }

        if (m_pDiaSession != NULL) {
                m_pDiaSession->Release();
                m_pDiaSession = NULL;
        }
        CoUninitialize();
}

wtuint32_t PdbReader::if_pdbDumpAll() {

        // TODO: Use AutoPtr To Make EnumSymbol Release
        IDiaEnumSymbols *pDiaEnumSymbols;

        m_hr = m_pDiaGSymbol->findChildren(SymTagCompiland, NULL, nsNone, &pDiaEnumSymbols);
        if (FAILED(m_hr)) {
                throw wt::IfDiaComError();
        }
        
        IDiaSymbol *pDiaSymCompiland;
        wtulong  celt;

        while (SUCCEEDED(pDiaEnumSymbols->Next(1, &pDiaSymCompiland, &celt)) && celt == 1) {
                if_pdbDumpCompiland(pDiaSymCompiland);
                pDiaSymCompiland->Release();
        }
        
        pDiaEnumSymbols->Release();
        return 0;
}

wtuint32_t PdbReader::if_pdbDumpCompiland(IDiaSymbol * &pDiaSymCompiland) {

        if (!if_pdbIsCompilandIsSource(pDiaSymCompiland)) {
                return 0;
        }

        // TODO: Use AutoPtr To Make EnumSourceFiles Release
        IDiaEnumSourceFiles *pDiaSourceFiles;
        IDiaSourceFile      *pDiaSource;
        wtulong              celt;

        m_hr = m_pDiaSession->findFile(pDiaSymCompiland, NULL, nsNone, &pDiaSourceFiles);
        if (FAILED(m_hr)) {
                pDiaSymCompiland->Release();
                throw wt::IfDiaComError();
        }

        while (SUCCEEDED(pDiaSourceFiles->Next(1, &pDiaSource, &celt)) && celt == 1) {
                if_pdbDumpSourceFile(pDiaSource);
                pDiaSource->Release();
        }
        pDiaSourceFiles->Release();

        // TODO: Use AutoPtr To Make EnumSymbol Release
        IDiaEnumSymbols *pDiaEnumSymbols;
        IDiaSymbol      *pDiaSymFunction;

        m_hr = pDiaSymCompiland->findChildren(SymTagFunction, NULL, nsNone, &pDiaEnumSymbols);
        if (FAILED(m_hr)) {
                throw wt::IfDiaComError();
        }
        
        while (SUCCEEDED(pDiaEnumSymbols->Next(1, &pDiaSymFunction, &celt)) && celt == 1) {
                if_pdbDumpFunc(pDiaSymFunction);
                pDiaSymFunction->Release();
        }
        pDiaEnumSymbols->Release();
     
        return 0;
}

wtuint32_t PdbReader::if_pdbDumpSourceFile(IDiaSourceFile *&pDiaSourceFile) {

        wtwchar *szFileName;
        if (pDiaSourceFile->get_fileName(&szFileName) != S_OK || szFileName == NULL) {
                pDiaSourceFile->Release();
                throw wt::IfDiaComError();
        }

        m_pDbgInfo->insertSourceFileToList(szFileName);
        SysFreeString(szFileName);

        return 0;
}

wtuint32_t PdbReader::if_pdbDumpFunc(IDiaSymbol * &pDiaSymFunction) {

        wtwchar *szFuncName;
        wtwchar *szFuncRealName;
        wtbool   bNoNeedFree = wtfalse;
        DbgFuncInfo *pFuncInfo;

        if (pDiaSymFunction->get_name(&szFuncName) != S_OK) {
                // no need to analyse the noname function;
                return 0;
        }
        if (pDiaSymFunction->get_undecoratedName(&szFuncRealName) != S_OK ) {
                bNoNeedFree = wttrue;
                szFuncRealName = szFuncName;
        }

        pFuncInfo = m_pDbgInfo->insertFunctionToList(szFuncName, szFuncRealName);

        SysFreeString(szFuncName);
        if (!bNoNeedFree) { SysFreeString(szFuncRealName); }

        wtuint64_t funcLen;
        wtulong    funcRVA;

        IDiaEnumLineNumbers *pDiaEnumLines;


        if (pDiaSymFunction->get_length(&funcLen) != S_OK) {
                throw wt::IfDiaComError();
        }
        if (pDiaSymFunction->get_relativeVirtualAddress(&funcRVA) != S_OK) {
                throw wt::IfDiaComError();
        }
        m_hr = m_pDiaSession->findLinesByRVA(funcRVA, static_cast<wtulong>(funcLen), &pDiaEnumLines);
        if (FAILED(m_hr)) {
                throw wt::IfDiaComError();
        }
        if_pdbDumpLines(pDiaEnumLines, pFuncInfo);
        pDiaEnumLines->Release();
        return 0;
}

wtuint32_t PdbReader::if_pdbDumpLines(IDiaEnumLineNumbers *&pDiaEnumLines, DbgFuncInfo *pFuncInfo) {
        IDiaLineNumber      *pDiaLine;
        wtulong              celt;

        wtulong              lineRVA;
        wtulong              lineLength;
        wtulong              lineNo;
        wtulong              lineSrcId;
        wtulong              lineLastSrcId = -1;
        DbgFileInfo         *pFileInfo = NULL;    

        while (SUCCEEDED(pDiaEnumLines->Next(1, &pDiaLine, &celt)) && (celt == 1)) {
                //if_pdbDumpLine(pDiaLine, pFuncInfo);

                if (pDiaLine->get_relativeVirtualAddress(&lineRVA) != S_OK ||
                        pDiaLine->get_length(&lineLength) != S_OK ||
                        pDiaLine->get_lineNumber(&lineNo) != S_OK ||
                        pDiaLine->get_sourceFileId(&lineSrcId) != S_OK) {
                        pDiaLine->Release();
                        pDiaEnumLines->Release();
                        throw wt::IfDiaComError();
                }

                if (lineSrcId != lineLastSrcId) {
                        IDiaSourceFile *pDiaSource;
                        wtwchar        *szFileName;
                        if (pDiaLine->get_sourceFile(&pDiaSource) != S_OK) {
                                pDiaLine->Release();
                                pDiaEnumLines->Release();
                                throw wt::IfDiaComError();
                        }
                        if (pDiaSource->get_fileName(&szFileName) != S_OK || szFileName == NULL) {
                                pDiaLine->Release();
                                pDiaEnumLines->Release();
                                pDiaSource->Release();
                                throw wt::IfDiaComError();
                        }
                        pDiaSource->Release();
                        pFileInfo = m_pDbgInfo->getSourceFile(szFileName);
                        SysFreeString(szFileName);
                        lineLastSrcId = lineSrcId;
                }

                if (pFileInfo == NULL) {
                        pDiaLine->Release();
                        pDiaEnumLines->Release();
                        //TODO: change the right errror
                        throw wt::IfError();
                }

                m_pDbgInfo->insertLineToList(lineNo, (wtpvoid)lineRVA, lineLength, pFileInfo, pFuncInfo);
                pDiaLine->Release();
        }
        return 0;
}

wtbool PdbReader::if_pdbIsCompilandIsSource(IDiaSymbol *&pDiaSymCompiland) {

        IDiaEnumSymbols *pDiaEnumSymbols;
        IDiaSymbol      *pDiaSymbol;
        wtulong          celt;

        m_hr = pDiaSymCompiland->findChildren(SymTagCompilandDetails, NULL, nsNone, &pDiaEnumSymbols);
        if (FAILED(m_hr)) {
                pDiaSymCompiland->Release();
                throw wt::IfDiaComError();
        }

        while (SUCCEEDED(pDiaEnumSymbols->Next(1, &pDiaSymbol, &celt)) && celt == 1) {
                wtulong  language;
                pDiaSymbol->get_language(&language);
                pDiaSymbol->Release();
                if (language == CV_CFL_LINK || language == CV_CFL_CVTRES || language == CV_CFL_CVTPGD) {
                        pDiaEnumSymbols->Release();
                        return wtfalse;
                }
        }
        pDiaEnumSymbols->Release();
        return wttrue;
}

}