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
    virtual int Install() override;
    virtual int Uninstall() override;
    virtual bool IsInstalled() override;
    virtual int Start() override;
    virtual int Stop() override;
    virtual int Pause() override;
    virtual int Continue() override;
    virtual bool IsStoped() override;
    virtual bool IsRunning() override;
    virtual int Main(int argc, char* argv[]) override;
    
    qint16 State();
    CServiceWindowsHandler m_Handler;
    
private:
    CServiceManageWindows(CService* pService,
                    QObject *parent = nullptr);
    
    int ControlService(DWORD dwCode);
    
    friend class CServiceManage;
};

} // namespace RabbitCommon

#endif // CSERVICEWINDOWS_H
