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

}