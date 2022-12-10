#ifndef __MiniDumper_H_KL_2022_07_19__
#define __MiniDumper_H_KL_2022_07_19__

#include <windows.h>

namespace RabbitCommon {

/*!
 * \brief Windows core dumper
 * \note This class is intended for internal use only
 */
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

void EnableMiniDumper();

} //namespace RabbitCommon

#endif // __MiniDumper_H_KL_2022_07_19__
