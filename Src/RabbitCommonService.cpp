/** @copyright Copyright (c) Kang Lin studio, All Rights Reserved
 *  @author Kang Lin(kl222@126.com)
 *  @abstract Service
 */

#include "ServiceWindows.h"
#include <QApplication>
#include <QMessageBox>
#include "RabbitCommonLog.h"

namespace RabbitCommon {

CService::CService(const QString &name,
                   const QString &displayName,
                   QObject *parent) : QObject(parent),
    m_szName(name),
    m_szDisplayName(displayName)
{
    if(m_szName.isEmpty())
        m_szName = qApp->applicationName();

    if(m_szDisplayName.isEmpty())
        m_szDisplayName = qApp->applicationDisplayName();
}

CService::~CService()
{}

QString CService::Name()
{
    return m_szName;
}

QString CService::DisplayName()
{
    return m_szDisplayName;
}

int CService::Start(int argc, char *argv[])
{
    //QMessageBox::information(nullptr, "Service", "Start");
    int nRet = 0;
    nRet = OnInit(argc, argv);
    if(nRet)
    {
        LOG_MODEL_ERROR("Service", "OnInit fail: %d", nRet);
        return nRet;
    }
    nRet = OnRun();
    if(nRet)
    {
        LOG_MODEL_ERROR("Service", "OnRun fail: %d", nRet);
        return nRet;
    }
    nRet = OnClean();
    if(nRet)
    {
        LOG_MODEL_ERROR("Service", "OnClean fail: %d", nRet);
        return nRet;
    }
    return nRet;
}

int CService::OnInit(int argc, char *argv[])
{
    QApplication a(argc, argv);
    return 0;
}

int CService::OnRun()
{
    //QMessageBox::information(nullptr, "Service", "OnRun");
    return qApp->exec();
}

int CService::OnClean()
{
    return 0;
}

int CService::Stop()
{
    //QMessageBox::information(nullptr, "Service", "Stop");
    qApp->quit();
    return 0;
}

int CService::ShutDown()
{
    //QMessageBox::information(nullptr, "Service", "ShutDown");
    return 0;
}

int CService::Pause()
{
    //QMessageBox::information(nullptr, "Service", "Pause");
    return 0;
}

int CService::Continue()
{
    //QMessageBox::information(nullptr, "Service", "Continue");
    return 0;
}

CServiceManage::CServiceManage(CService *pServer,
                   QObject *parent)
    : QObject(parent),
    m_pServer(pServer)
{
}

CServiceManage::~CServiceManage()
{}

int CServiceManage::Pause(const QString &name)
{
    Q_UNUSED(name);
    return 0;
}

int CServiceManage::Continue(const QString &name)
{
    Q_UNUSED(name);
    return 0;
}

QString CServiceManage::Name()
{
    if(m_pServer)
        return m_pServer->Name();
    return qApp->applicationName();
}

QString CServiceManage::DisplayName()
{
    if(m_pServer)
        return m_pServer->DisplayName();
    return qApp->applicationDisplayName();
}

CService* CServiceManage::GetService()
{
    return m_pServer;
}

CServiceManage* CServiceManage::Instance(CService* pService)
{
    static CServiceManage* p = nullptr;
    //if(!pService && !p) Q_ASSERT(false);

#if defined(Q_OS_WINDOWS)
    if(!p) p = new CServiceManageWindows(pService);
#elif defined(Q_OS_UNIX)
    
#else
    #error "Don't support "
#endif

    return p;
}

} //namespace RabbitCommon
