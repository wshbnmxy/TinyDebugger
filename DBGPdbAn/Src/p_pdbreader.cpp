#include <pdbreader.h>
#include <pdberror.h>
#include <pdbcallback.h>
#include <cstring>

namespace wt {

PdbReader::PdbReader(wtwchar *szModuleName) :
        m_pDiaDataSource(NULL), m_pDiaSession(NULL), m_pDiaGSymbol(NULL) {
        CoInitialize(NULL);
        
        if (szModuleName == NULL) {
                throw wt::PdbParamError();
        }

        m_hr = CoCreateInstance(__uuidof(DiaSource), NULL, CLSCTX_INPROC_SERVER, 
                        __uuidof(IDiaDataSource), (void **) &m_pDiaDataSource);
        if (FAILED(m_hr)) {
                throw wt::PdbDiaComError();
        }

        wt::PdbCallback pdbCb;
        pdbCb.AddRef();
        m_hr = m_pDiaDataSource->loadDataForExe(szModuleName, NULL, &pdbCb);
        if (FAILED(m_hr)) {
                throw wt::PdbDiaComError();
        }

        m_hr = m_pDiaDataSource->openSession(&m_pDiaSession);
        if (FAILED(m_hr)) {
                throw wt::PdbDiaComError();
        }

        m_hr = m_pDiaSession->get_globalScope(&m_pDiaGSymbol);
        if (FAILED(m_hr)) {
                throw wt::PdbDiaComError();
        }
}


PdbReader::~PdbReader()
{
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

wtuint32_t PdbReader::pdb_dumpAll() {

        IDiaEnumSymbols *pDiaEnumSymbols;

        m_hr = m_pDiaGSymbol->findChildren(SymTagCompiland, NULL, nsNone, &pDiaEnumSymbols);
        if (FAILED(m_hr)) {
                throw wt::PdbDiaComError();
        }
        
        IDiaSymbol *pDiaSymCompiland;
        wtulong  celt;

        while (SUCCEEDED(pDiaEnumSymbols->Next(1, &pDiaSymCompiland, &celt)) && celt == 1) {
                pdb_dumpObj(pDiaSymCompiland);
                pDiaSymCompiland->Release();
        }
        
        return 0;
}

wtuint32_t PdbReader::pdb_dumpObj(IDiaSymbol *pDiaSymCompiland) {

        IDiaEnumSymbols *pDiaEnumSymbols;
        IDiaSymbol      *pDiaSymbol;
        wtulong  celt;

        m_hr = pDiaSymCompiland->findChildren(SymTagCompilandDetails, NULL, nsNone, &pDiaEnumSymbols);
        if (FAILED(m_hr)) {
                pDiaSymCompiland->Release();
                throw wt::PdbDiaComError();
        }

        while (SUCCEEDED(pDiaEnumSymbols->Next(1, &pDiaSymbol, &celt)) && celt == 1) {
                wtulong  language;
                pDiaSymbol->get_language(&language);
                pDiaSymbol->Release();
                if (language == CV_CFL_LINK || language == CV_CFL_CVTRES || language == CV_CFL_CVTPGD) {
                        // not a source obj
                        return 0;
                }
        }
        pDiaEnumSymbols->Release();

        m_hr = pDiaSymCompiland->findChildren(SymTagFunction, NULL, nsNone, &pDiaEnumSymbols);
        if (FAILED(m_hr)) {
                pDiaSymCompiland->Release();
                throw wt::PdbDiaComError();
        }

        while (SUCCEEDED(pDiaEnumSymbols->Next(1, &pDiaSymbol, &celt)) && celt == 1) {
                pdb_dumpFun(pDiaSymbol);
                pDiaSymbol->Release();
        }
        pDiaEnumSymbols->Release();
        
        return 0;
}

wtuint32_t PdbReader::pdb_dumpFun(IDiaSymbol *pDiaSymFunction) {

        wtwchar   *szFuncName;
        wtuint64_t ullSize;
        wtulong    ulRVA;
        if (pDiaSymFunction->get_name(&szFuncName) != S_OK) {
                return 0;
        }

        SysFreeString(szFuncName);
        if (pDiaSymFunction->get_length(&ullSize) != S_OK) {
                return 0;
        }

        IDiaEnumLineNumbers *pDiaLineNumbers;
        if (pDiaSymFunction->get_relativeVirtualAddress(&ulRVA) != S_OK) {

        }
        m_pDiaSession->findLinesByRVA(ulRVA, static_cast<wtuint32_t>(ullSize), &pDiaLineNumbers);

        return 0;
}

}