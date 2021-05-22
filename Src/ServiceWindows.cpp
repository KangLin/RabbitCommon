/** @copyright Copyright (c) Kang Lin studio, All Rights Reserved
 *  @author Kang Lin(kl222@126.com)
 *  @abstract Service
 *  @see https://wenku.baidu.com/view/1b537bc66137ee06eff91859.html?fr=search-1-X&fixfr=Xbjd8EVghLPBEhilc3do2g%3D%3D
 */


#include "ServiceWindows.h"

#include <atlconv.h>

#include <QApplication>
#include <exception>

#include "RabbitCommonDir.h"
#include "RabbitCommonLog.h"

namespace RabbitCommon {

CServiceManageWindows::CServiceManageWindows(CService *pService,
                                 QObject *parent)
    : CServiceManage(pService, parent),
      m_Handler((pService))
{
}

/**
 * @brief CServiceManageWindows::Install
 * 
 * The function is same follow command：
 * Use command line in windows:
 *        Enter RabbitCommonApp.exe directory, then exec follow command to install service
 *        sc create RabbitCommon %CD%\RabbitCommonApp.exe
 * @return 
 */
int CServiceManageWindows::Install(const QString &path,
                                   const QString &name,
                                   const QString &displayName)
{
    USES_CONVERSION;
    int nRet = 0;
    QString szName(name);
    QString szDisplayName(displayName);
    if(szName.isEmpty())
        szName = Name();
    if(szDisplayName.isEmpty())
        szDisplayName = DisplayName();
    
    if(IsInstalled(szName))
        return 0;
    
    SC_HANDLE hSCM = ::OpenSCManager(NULL, NULL, SC_MANAGER_CREATE_SERVICE);
    if(hSCM == NULL)
    {
        LOG_MODEL_ERROR("Service", 
                        "OpenSCManager() failed with error code:%d\n",
                        GetLastError());
        return -1;
    }
    
    QString szPath(path);
    if(szPath.isEmpty())
    {
        char szFilePath[_MAX_PATH];
        ::GetModuleFileNameA(NULL, szFilePath, _MAX_PATH);
        szPath = szFilePath;
    }
    SC_HANDLE hService = ::CreateServiceW(hSCM,
                                         (szName.toStdWString().c_str()),
                                       szDisplayName.toStdWString().c_str(),
                                                             SERVICE_ALL_ACCESS,
                        SERVICE_WIN32_OWN_PROCESS | SERVICE_INTERACTIVE_PROCESS,
                                         SERVICE_AUTO_START,
                                         SERVICE_ERROR_NORMAL,
                                         (szPath.toStdWString().c_str()),
                                         NULL, NULL, NULL, NULL, NULL);
    
    if(hService)
    {
        ::CloseServiceHandle(hService);
        LOG_MODEL_INFO("Service", "Install service: %s",
                       szName.toStdString().c_str());
    }
    else
    {
        LOG_MODEL_ERROR("Service",
                        "CreateService() failed with error code:%d\n",
                        GetLastError());
        nRet = -2;
    }
    
    ::CloseServiceHandle(hSCM);
    
    return nRet;
}

/**
 * @brief CServiceManageWindows::Uninstall
 *
 * The function is same follow command：
 * Use command line in windows:
 *        Exec follow command to install service
 *        sc delete RabbitCommon 
 */
int CServiceManageWindows::Uninstall(const QString &name)
{
    USES_CONVERSION;
    QString szName(name);
    if(szName.isEmpty())
        szName = Name();
    
    if(!IsInstalled(szName))
        return 0;

    SC_HANDLE hSCM = ::OpenSCManager(NULL, NULL, SC_MANAGER_CREATE_SERVICE);
    if(hSCM == NULL)
    {
        LOG_MODEL_ERROR("Service", "OpenSCManager() failed with error code:%d\n", GetLastError());
        return -1;
    }

    SC_HANDLE hService = NULL;
    do
    {
        hService = ::OpenServiceW(hSCM, szName.toStdWString().c_str(),
                                 SERVICE_STOP | DELETE);
        if(hService == NULL)
        {
            LOG_MODEL_ERROR("Service",
                            "OpenService() failed with error code:%d\n",
                            GetLastError());
            break;
        }

        SERVICE_STATUS status;
        if(!::ControlService(hService, SERVICE_CONTROL_STOP, &status))
        {
            LOG_MODEL_ERROR("Service", "Stop server fail: %s",
                            szName.toStdString().c_str());
        }

        if(!::DeleteService(hService))
        {
            LOG_MODEL_ERROR("Service",
                            "DeleteService() failed with error code:%d\n",
                            GetLastError());
            break;
        }
        LOG_MODEL_INFO("Service", "Unistall service: %s",
                       szName.toStdString().c_str());
    } while(0);

    if(hService)
        ::CloseServiceHandle(hService);
    if(hSCM)
        ::CloseServiceHandle(hSCM);

    return TRUE;
}

bool CServiceManageWindows::IsInstalled(const QString &name)
{
    USES_CONVERSION;
    bool bRet = false;

    SC_HANDLE hSCM = ::OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
    if(hSCM == NULL)
    {
        LOG_MODEL_ERROR("Service",
                        "OpenSCManager() failed with error code:%d\n",
                        GetLastError());
        return FALSE;
    }

    QString szName = name;
    if(szName.isEmpty())
        szName = Name();
    SC_HANDLE hService = ::OpenServiceW(hSCM,
                                       szName.toStdWString().c_str(),
                                       SERVICE_QUERY_CONFIG);
    if(hService)
    {
        bRet = true;
        ::CloseServiceHandle(hService);
        LOG_MODEL_INFO("Service", "Service: %s is installed",
                       szName.toStdString().c_str());
    }

    ::CloseServiceHandle(hSCM);
    return bRet;
}

int CServiceManageWindows::Start(const QString &name, int argc, const char *argv[])
{
    USES_CONVERSION;
    int nRet = 0;
    SC_HANDLE hSCM = ::OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if(hSCM == NULL)
    {
        LOG_MODEL_ERROR("Service", "OpenSCManager() failed with error code:%d\n", GetLastError());
        return -1;
    }
    
    QString szName(name);
    if(szName.isEmpty())
        szName = Name();
    
	SC_HANDLE hService = ::OpenServiceW(hSCM,
                                       szName.toStdWString().c_str(),
                                       SERVICE_START);
    do {
        if(hService == NULL)
        {
            nRet = -2;
            LOG_MODEL_ERROR("Service",
                            "OpenService() failed with error code:%d\n",
                            GetLastError());
            break;
        }

        if(!::StartServiceA(hService, argc, argv))
        {
            LOG_MODEL_ERROR("Service",
                            "StartService() failed with error code:%d\n",
                            GetLastError());
            nRet = -3;
            break;
        }
        LOG_MODEL_INFO("Service", "Start service: %s",
                       szName.toStdString().c_str());
    }while(0);

    if(hService)
        ::CloseServiceHandle(hService);
    if(hSCM)
        ::CloseServiceHandle(hSCM);

    return nRet;
}

int CServiceManageWindows::ControlService(DWORD dwCode, const QString &name)
{
    USES_CONVERSION;
    int nRet = 0;
    SC_HANDLE hSCM = ::OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if(hSCM == NULL)
    {
        LOG_MODEL_ERROR("Service", "OpenSCManager() failed with error code:%d\n", GetLastError());
        return -1;
    }

    QString szName(name);
    if(szName.isEmpty())
        szName = Name();
    
	SC_HANDLE hService = ::OpenServiceW(hSCM,
                                       szName.toStdWString().c_str(),
                                       SERVICE_STOP);
    do {
        if(hService == NULL)
        {
            nRet = -2;
            LOG_MODEL_ERROR("Service",
                            "OpenService() failed with error code:%d\n",
                            GetLastError());
            break;
        }

        SERVICE_STATUS	status = {0};
        if(!::ControlService(hService, dwCode, &status))
        {
            LOG_MODEL_ERROR("Service",
                            "Stop service failed with error code:%d; %s",
                            GetLastError(), szName.toStdString().c_str());
            nRet = -3;
            break;
        }
        LOG_MODEL_INFO("Service", "Stop service: %s",
                       szName.toStdString().c_str());
    } while(0);

    if(hService)
        ::CloseServiceHandle(hService);
    if(hSCM)
        ::CloseServiceHandle(hSCM);

    return nRet;
}

int CServiceManageWindows::Stop(const QString &name)
{
    return ControlService(SERVICE_CONTROL_STOP, name);
}

int CServiceManageWindows::Pause(const QString &name)
{
    return ControlService(SERVICE_CONTROL_PAUSE, name);
}

int CServiceManageWindows::Continue(const QString &name)
{
    return ControlService(SERVICE_CONTROL_CONTINUE, name);
}

bool CServiceManageWindows::IsStoped(const QString &name)
{
    USES_CONVERSION;
    bool bRet = false;
    SC_HANDLE hSCM = ::OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
    if(!hSCM)
    {
        LOG_MODEL_ERROR("Service",
                        "OpenSCManager() failed with error code:%d\n",
                        GetLastError());
        return false;
    }
    
    QString szName(name);
    if(szName.isEmpty())
        szName = Name();
    
    SC_HANDLE hService = ::OpenServiceW(hSCM, name.toStdWString().c_str(),
                                       SERVICE_STOP | DELETE);
    do {
        if(hService == NULL)
        {
            LOG_MODEL_ERROR("Service",
                            "OpenService() failed with error code:%d\n",
                            GetLastError());
            break;
        }
        
        SERVICE_STATUS	status = {0};
        if(!QueryServiceStatus(hService, &status))
        {
            LOG_MODEL_ERROR("Service",
                            "QueryServiceStatus() failed with error code:%d\n",
                            GetLastError());
            break;
        }
        if(SERVICE_STOPPED == status.dwCurrentState)
            bRet = true;
    } while(0);
    
    ::CloseServiceHandle(hService);
    ::CloseServiceHandle(hSCM);
    
    return bRet;
}

bool CServiceManageWindows::IsRunning(const QString &name)
{
    USES_CONVERSION;
    bool bRet = false;
    SC_HANDLE hSCM = ::OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
    if(!hSCM)
    {
        LOG_MODEL_ERROR("Service",
                        "OpenSCManager() failed with error code:%d\n",
                        GetLastError());
        return false;
    }
    
    QString szName(name);
    if(szName.isEmpty())
        szName = Name();
    
    SC_HANDLE hService = ::OpenServiceW(hSCM, szName.toStdWString().c_str(),
                                       SERVICE_STOP | DELETE);
    do {
        if(hService == NULL)
        {
            LOG_MODEL_ERROR("Service",
                            "OpenService() failed with error code:%d\n",
                            GetLastError());
            break;
        }
        
        SERVICE_STATUS	status = {0};
        if(!QueryServiceStatus(hService, &status))
        {
            LOG_MODEL_ERROR("Service",
                            "QueryServiceStatus() failed with error code:%d\n",
                            GetLastError());
            break;
        }
        if(SERVICE_RUNNING == status.dwCurrentState)
            bRet = true;
    } while(0);
    
    ::CloseServiceHandle(hService);
    ::CloseServiceHandle(hSCM);
    
    return bRet;
}

qint16 CServiceManageWindows::State(const QString& name)
{
    USES_CONVERSION;
    // - Open the SCM
    SC_HANDLE hSCM = OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT);
    if (!hSCM)
        throw std::exception("unable to open Service Control Manager", GetLastError());
    
    QString szName(name);
    if(szName.isEmpty())
        szName = Name();
    
    // - Locate the service
    SC_HANDLE hService = OpenServiceW(hSCM,
                                     szName.toStdWString().c_str(),
                                     SERVICE_INTERROGATE);
    if (!hService)
        throw std::exception("unable to open the service", GetLastError());
    
    // - Get the service status
    SERVICE_STATUS status;
    if (!::ControlService(hService, SERVICE_CONTROL_INTERROGATE, (SERVICE_STATUS*)&status))
        throw std::exception("unable to query the service", GetLastError());
    
    return status.dwCurrentState;
}

VOID WINAPI serviceHandler(DWORD control) {
    LOG_MODEL_INFO("ServerWindows", "Service handler ...");
    CServiceManageWindows* pMg = qobject_cast<CServiceManageWindows*>(CServiceManage::Instance());
    CServiceWindowsHandler* pService = &(pMg->m_Handler);
    if(!pService)
    {
        Q_ASSERT(false);
        return;
    }
    switch (control) {
    case SERVICE_CONTROL_INTERROGATE:
        LOG_MODEL_INFO("ServerWindows", "cmd: report status");
        pService->SetStatus(pService->m_Status.dwCurrentState);
        return;
    case SERVICE_CONTROL_PARAMCHANGE:
        LOG_MODEL_INFO("ServerWindows", "cmd: param change");
        return;
    case SERVICE_CONTROL_SHUTDOWN:
        LOG_MODEL_INFO("ServerWindows", "cmd: OS shutdown");
        pService->ShutDown();
        return;
    case SERVICE_CONTROL_STOP:
        LOG_MODEL_INFO("ServerWindows", "cmd: stop");
        pService->Stop();
        return;
    case SERVICE_CONTROL_PAUSE:
        pService->Pause();
        return;
    case SERVICE_CONTROL_CONTINUE:
        pService->Continue();
        return;
    };
    LOG_MODEL_DEBUG("ServerWindows", "cmd: unknown %lu", control);
}

VOID WINAPI serviceProc(DWORD dwArgc, LPSTR* lpszArgv) {
    USES_CONVERSION;
    LOG_MODEL_INFO("ServerWindows", "Registering handler ...");
    CServiceManageWindows* pMg = qobject_cast<CServiceManageWindows*>(CServiceManage::Instance());
    CServiceWindowsHandler* pService = &(pMg->m_Handler);
    if(!pService)
    {
        LOG_MODEL_ERROR("ServerWindows", "pServer is null. please set CServiceManage::Instance");
        Q_ASSERT(false);
    }
    pService->m_StatusHandle = ::RegisterServiceCtrlHandler(
                A2T(pMg->Name().toStdString().c_str()),
                serviceHandler);
    if (!pService->m_StatusHandle) {
        DWORD err = GetLastError();
        LOG_MODEL_ERROR("ServerWindows", "failed to register handler: %lu", err);
        return;
    }
    LOG_MODEL_DEBUG("ServerWindows", "registered handler (%lx)", g_Server->m_StatusHandle);
    pService->SetStatus(SERVICE_START_PENDING);
    LOG_MODEL_DEBUG("ServerWindows", "entering %s serviceMain", g_Server->Name().toStdString().c_str());
    pService->m_Status.dwWin32ExitCode = pService->Main(dwArgc, lpszArgv);
    LOG_MODEL_DEBUG("ServerWindows", "leaving %s serviceMain", g_Server->Name().toStdString().c_str());
    pService->SetStatus(SERVICE_STOPPED);
}

int CServiceManageWindows::Main(int argc, char* argv[])
{
    USES_CONVERSION;
    Q_UNUSED(argc);
    Q_UNUSED(argv);

    LOG_MODEL_DEBUG("ServerWindows", "Run Main ...");
    int nRet = 0;
    SERVICE_TABLE_ENTRYA entry[2];
    entry[0].lpServiceName = (LPSTR)Name().toStdString().c_str();
    entry[0].lpServiceProc = serviceProc;
    entry[1].lpServiceName = NULL;
    entry[1].lpServiceProc = NULL;

    if (!StartServiceCtrlDispatcherA(entry))
    {
        nRet = -1;
        LOG_MODEL_ERROR("ServerWindows", "unable to start service. %d", GetLastError());
    }
    return nRet;
}

CServiceWindowsHandler::CServiceWindowsHandler(CService* pService) : QObject()
{
    m_pService = pService;
    m_StatusHandle = NULL;
    m_Status.dwControlsAccepted = SERVICE_CONTROL_INTERROGATE | SERVICE_ACCEPT_SHUTDOWN | SERVICE_ACCEPT_STOP;
    m_Status.dwServiceType = SERVICE_WIN32_OWN_PROCESS | SERVICE_INTERACTIVE_PROCESS;
    m_Status.dwWin32ExitCode = NO_ERROR;
    m_Status.dwServiceSpecificExitCode = 0;
    m_Status.dwCheckPoint = 0;
    m_Status.dwWaitHint = 30000;
    m_Status.dwCurrentState = SERVICE_STOPPED;
}

int CServiceWindowsHandler::Main(int argc, LPSTR *argv)
{
    USES_CONVERSION;
    Q_UNUSED(argc);
    Q_UNUSED(argv);
    int nRet = -1;
    SetStatus(SERVICE_RUNNING);
    Q_ASSERT(m_pService);
    
    if(m_pService)
        nRet = m_pService->Start(argc, argv);
    else
        LOG_MODEL_ERROR("CServiceWindowsHandler", "m_pService is null. please set CServiceManage::Instance()");
    SetStatus(SERVICE_STOPPED);
    return nRet;
}

int CServiceWindowsHandler::Stop()
{
    Q_ASSERT(m_pService);
    int nRet = -1;
    if(m_pService)
        nRet = m_pService->Stop();
    else
        LOG_MODEL_ERROR("CServiceWindowsHandler", "m_pService is null. please set CServiceManage::Instance()");
    SetStatus(SERVICE_STOPPED);
    return nRet;
}

int CServiceWindowsHandler::Pause()
{
    Q_ASSERT(m_pService);
    if(m_pService)
        return m_pService->Pause();
    LOG_MODEL_ERROR("CServiceWindowsHandler", "m_pService is null. please set CServiceManage::Instance()");
    return -1;
}

int CServiceWindowsHandler::Continue()
{
    Q_ASSERT(m_pService);
    if(m_pService)
        return m_pService->Continue();
    LOG_MODEL_ERROR("CServiceWindowsHandler", "m_pService is null. please set CServiceManage::Instance()");
    return -1;
}

int CServiceWindowsHandler::ShutDown()
{
    return m_pService->ShutDown();
}

int CServiceWindowsHandler::SetStatus(DWORD state)
{
    if (m_StatusHandle == 0) {
      LOG_MODEL_ERROR("ServerWindows", "warning - cannot setStatus");
      return -1;
    }
    m_Status.dwCurrentState = state;
    if(SERVICE_START_PENDING == state)
        m_Status.dwCheckPoint = 0;
    else if(SERVICE_RUNNING == state)
    {
        m_Status.dwCheckPoint = 0;
        m_Status.dwWaitHint = 0;
    }
    else
        m_Status.dwCheckPoint++;
    if (!SetServiceStatus(m_StatusHandle, &m_Status)) {
      m_Status.dwCurrentState = SERVICE_STOPPED;
      m_Status.dwWin32ExitCode = GetLastError();
      LOG_MODEL_ERROR("ServerWindows", "unable to set service status:%u", m_Status.dwWin32ExitCode);
      return -2;
    }
    LOG_MODEL_ERROR("ServerWindows", "set status to %u(%u)", state, m_Status.dwCheckPoint);
    return 0;
}

} //namespace RabbitCommon 
