#include <pdbread.h>
#include <pdbreader.h>
#include <pdberror.h>
#include <dia2.h>

#include <exception>

wtint32_t pdb_read(wtwchar *szModuleName) {

        //HRESULT hr;

        //IDiaDataSource *pDiaDataSource;

        //hr = CoInitialize(NULL);
        //hr = CoCreateInstance(__uuidof(DiaSource), NULL, CLSCTX_INPROC_SERVER,
        //        __uuidof(IDiaDataSource), (wtpvoid *) &pDiaDataSource);
        //if (FAILED(hr)) {
        //        return 1;
        //}

        //char * a = new (std::nothrow) char();
        //hr = pDiaDataSource->loadDataForExe(szModuleName, NULL, )
        

        //CoUninitialize();

        try {
                wt::PdbReader reader(szModuleName);
        } catch (const wt::PdbError &e) {
                e.pdb_getError();
        } catch (const std::exception& e) {
                e.what();
        }
        return 0;
}