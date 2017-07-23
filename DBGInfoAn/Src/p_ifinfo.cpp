#include <ifinfo.h>

#include <map>

namespace wt {
std::map<std::wstring, DbgInfo *>  dbgInfoMapG;    // A map about moduleName and dbgInfo
}

namespace wt {

DbgLineInfo::DbgLineInfo(wtuint32_t lineNo, wtpvoid lineRVA, wtuint32_t lineLen, DbgFileInfo *pFileInfo, DbgFuncInfo *pFuncInfo) :
        m_lineNo(lineNo), m_lineRVA(lineRVA), m_lineLength(lineLen), m_fileInfo(pFileInfo), m_funcInfo(pFuncInfo) {
        if (m_funcInfo->m_FileInfo == NULL) {
                m_funcInfo->m_FileInfo = m_fileInfo;
        }
        if (m_funcInfo->m_beginLineInfo == NULL || m_funcInfo->m_beginLineInfo->m_lineRVA > m_lineRVA) {
                m_funcInfo->m_beginLineInfo = this;
        }
        if (m_funcInfo->m_endLineInfo == NULL || m_funcInfo->m_endLineInfo->m_lineRVA < m_lineRVA) {
                m_funcInfo->m_endLineInfo = this;
        }
}

}

namespace wt {

DbgFileInfo* DbgInfo::getSourceFile(std::wstring sourceName) {
        for (auto iter = m_FileList.begin(); iter != m_FileList.end(); iter++) {
                if (sourceName == (*iter)->m_fileName) {
                        return *iter;
                } else if (sourceName < (*iter)->m_fileName) {
                        break;
                }
        }
        return NULL;
}

void DbgInfo::insertSourceFileToList(std::wstring sourceName) {
        auto iter = m_FileList.begin();
        for (; iter != m_FileList.end(); iter++) {
                if (sourceName == (*iter)->m_fileName) {
                        // find the same, so return
                        return;
                } else if (sourceName < (*iter)->m_fileName) {
                        m_FileList.insert(iter, new DbgFileInfo(sourceName));
                        return;
                }
        }
        m_FileList.insert(iter, new DbgFileInfo(sourceName));
        return;
}

DbgFuncInfo * DbgInfo::insertFunctionToList(std::wstring functionName, std::wstring functionRealName) {
        auto iter = m_FuncList.begin();
        for (; iter != m_FuncList.end(); iter++) {
                if (functionRealName == (*iter)->m_funcRealName) {
                        // find the same, so return
                        return *iter;
                } else if (functionRealName < (*iter)->m_funcRealName) {
                        return *m_FuncList.insert(iter, new DbgFuncInfo(functionName, functionRealName));
                }
        }
        return *m_FuncList.insert(iter, new DbgFuncInfo(functionName, functionRealName));
}

void DbgInfo::insertLineToList(wtuint32_t lineNo, wtpvoid lineRVA, wtuint32_t lineLen, DbgFileInfo *pFileInfo, DbgFuncInfo *pFuncInfo) {
        auto iter = m_LineList.begin();
        for (; iter != m_LineList.end(); iter++) {
                if (lineRVA == (*iter)->m_lineRVA) {
                        // find the same, so return (error?)
                        return;
                } else if (lineRVA < (*iter)->m_lineRVA) {
                        m_LineList.insert(iter, new DbgLineInfo(lineNo, lineRVA, lineLen, pFileInfo, pFuncInfo));
                        return;
                }
        }
        m_LineList.insert(iter, new DbgLineInfo(lineNo, lineRVA, lineLen, pFileInfo, pFuncInfo));
        return;
}

}

wt::DbgInfo *getDbgInfo(std::wstring ModuleName) {
        if (wt::dbgInfoMapG.count(ModuleName) == 0) {
                wt::dbgInfoMapG[ModuleName] = new wt::DbgInfo();
        }
        return wt::dbgInfoMapG[ModuleName];
}