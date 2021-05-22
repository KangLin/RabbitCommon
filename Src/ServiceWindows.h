/** @copyright Copyright (c) Kang Lin studio, All Rights Reserved
 *  @author Kang Lin(kl222@126.com)
 *  @abstract Service
 */


#ifndef CSERVICEWINDOWS_H
#define CSERVICEWINDOWS_H

#pragma once

#include "RabbitCommonService.h"

#include <Windows.h>

namespace RabbitCommon {

class CServiceWindowsHandler : public QObject
{
    Q_OBJECT
    
public:
    CServiceWindowsHandler(CService* pService);

    int Start();
    int Stop();
    int ShutDown();
    int Pause();
    int SetStatus(DWORD state);
    int Continue();
    int Main(int argc, LPTSTR* argv);
    
public:
    SERVICE_STATUS_HANDLE m_StatusHandle;
    SERVICE_STATUS m_Status;
    
private:
    CService* m_pService;
};

class CServiceManageWindows : public CServiceManage
{
    Q_OBJECT

public:
    virtual int Install(const QString& path = QString(),
                        const QString& name = QString(),
                        const QString& displayName = QString()) override;
    virtual int Uninstall(const QString& name = QString()) override;
    virtual bool IsInstalled(const QString& name = QString()) override;
    virtual int Start(const QString& name = QString(),
                      int argc = 0, const char* argv[] = nullptr) override;
    virtual int Stop(const QString& name = QString()) override;
    virtual int Pause(const QString& name = QString()) override;
    virtual int Continue(const QString& name = QString()) override;
    virtual bool IsStoped(const QString& name = QString()) override;
    virtual bool IsRunning(const QString& name = QString()) override;
    virtual int Main(int argc, char* argv[]) override;
    
    qint16 State(const QString& name = QString());
    CServiceWindowsHandler m_Handler;
    
private:
    CServiceManageWindows(CService* pService,
                    QObject *parent = nullptr);
    
    int ControlService(DWORD dwCode, const QString& name = QString());
    
    friend class CServiceManage;
};

} // namespace RabbitCommon

#endif // CSERVICEWINDOWS_H
