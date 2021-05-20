/** @copyright Copyright (c) Kang Lin studio, All Rights Reserved
 *  @author Kang Lin(kl222@126.com)
 *  @abstract Service
 */

#include "ServiceWindows.h"
#include <QApplication>

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

int CService::Start()
{
    return OnRun();
}

int CService::Stop()
{
    return 0;
}

int CService::Pause()
{
    return 0;
}

int CService::Continue()
{
    return 0;
}

int CService::ShutDown()
{
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

int CServiceManage::Pause()
{
    return 0;
}

int CServiceManage::Continue()
{
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
#endif

    return p;
}

} //namespace RabbitCommon
