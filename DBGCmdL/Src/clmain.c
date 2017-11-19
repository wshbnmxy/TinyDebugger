#include <clcommon.h>

#include <memory.h>

wtint_t wmain(wtint_t argc, wtwchar_t *argv[])
{
        wtint32_t  ret;
        {
        krExecInfo execInfo;
        memset(&execInfo, 0, sizeof(execInfo));
        execInfo.m_exepath = L"C:\\Users\\wshbnmxy\\Documents\\visual studio 2015\\Projects\\WTinyDTest\\Binary\\Win32\\Debug\\TP1.exe";
        //execInfo.m_execwf  = L"abc";
        ret = kr_exec(&execInfo);
        }
        {
        krSbrkInfo sbrkInfo;
        memset(&sbrkInfo, 0, sizeof(sbrkInfo));
        sbrkInfo.m_brktype = KR_SBRK_TYPE_ADDR;
        sbrkInfo.m_addr    = 0x012B177E;
        ret = kr_sbrk(&sbrkInfo);
        }
        return 0;
}