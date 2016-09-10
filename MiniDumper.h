#ifndef MINIDUMPER_H
#define MINIDUMPER_H

#include <windows.h>

// 无法验证代码来源（dumper），据说是电驴的部分代码，私有项目请注意授权

class CMiniDumper
{
public:

    CMiniDumper(bool bPromptUserForMiniDump);
    ~CMiniDumper(void);

private:

    static LONG WINAPI unhandledExceptionHandler(struct _EXCEPTION_POINTERS *pExceptionInfo);
    void setMiniDumpFileName(void);
    bool getImpersonationToken(HANDLE* phToken);
    BOOL enablePrivilege(LPCTSTR pszPriv, HANDLE hToken, TOKEN_PRIVILEGES* ptpOld);
    BOOL restorePrivilege(HANDLE hToken, TOKEN_PRIVILEGES* ptpOld);
    LONG writeMiniDump(_EXCEPTION_POINTERS *pExceptionInfo );

    _EXCEPTION_POINTERS *m_pExceptionInfo;
    TCHAR m_szMiniDumpPath[MAX_PATH];
    TCHAR m_szAppPath[MAX_PATH];
    TCHAR m_szAppBaseName[MAX_PATH];
    bool m_bPromptUserForMiniDump;

    static CMiniDumper* s_pMiniDumper;
    static LPCRITICAL_SECTION s_pCriticalSection;
};

#endif // MINIDUMPER_H
