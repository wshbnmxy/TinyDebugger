#include <ifcommon.h>
#include <ifpdbreader.h>
#include <ifinfo.h>

wt::DbgInfo *getDbgInfo(std::wstring ModuleName);

wtuint32_t if_initPDB(wtwchar *szModuleName) {

        wt::PdbReader reader(szModuleName, getDbgInfo(szModuleName));
        return 0;
}