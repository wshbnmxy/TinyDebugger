#ifndef _IF_INFO_H_
#define _IF_INFO_H_

#include<ifcommon.h>
#include<list>
#include<string>

namespace wt {

struct DbgFileInfo;
struct DbgFuncInfo;
struct DbgLineInfo;

struct DbgFileInfo {
        std::wstring    m_fileName;
        DbgFileInfo(std::wstring fileName) : m_fileName(fileName) {}
};

struct DbgFuncInfo {
        std::wstring    m_funcName;
        std::wstring    m_funcRealName;
        DbgFileInfo    *m_FileInfo;
        DbgLineInfo    *m_beginLineInfo;
        DbgLineInfo    *m_endLineInfo;
        DbgFuncInfo(std::wstring funcName, std::wstring funcRealName) : 
                m_funcName(funcName), m_funcRealName(funcRealName),
                m_FileInfo(NULL), m_beginLineInfo(NULL), m_endLineInfo(NULL) {}
};

struct DbgLineInfo {
        wtuint32_t      m_lineNo;
        wtpvoid_t       m_lineRVA;
        wtuint32_t      m_lineLength;
        DbgFileInfo    *m_fileInfo;
        DbgFuncInfo    *m_funcInfo;
        DbgLineInfo(wtuint32_t lineNo, wtpvoid_t lineRVA, wtuint32_t lineLen, DbgFileInfo *pFileInfo, DbgFuncInfo *pFuncInfo);
};

class DbgInfo {
friend class PdbReader;
public:
        DbgInfo() {}
        ~DbgInfo() {}
private:
        std::list<DbgFileInfo *>   m_FileList;
        std::list<DbgFuncInfo *>   m_FuncList;
        std::list<DbgLineInfo *>   m_LineList;

public:
        DbgFileInfo* getSourceFile(std::wstring sourceName);

private:
        void insertSourceFileToList(std::wstring sourceName);
        DbgFuncInfo* insertFunctionToList(std::wstring functionName, std::wstring functionRealName);
        void insertLineToList(wtuint32_t lineNo, wtpvoid_t lineRVA, wtuint32_t lineLen, DbgFileInfo *pFileInfo, DbgFuncInfo *pFuncInfo);
};

}
#endif // !_IF_INFO_H_
