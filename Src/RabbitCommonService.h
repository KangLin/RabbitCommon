/** @copyright Copyright (c) Kang Lin studio, All Rights Reserved
 *  @author Kang Lin(kl222@126.com)
 *  @abstract Service
 */

#ifndef RABBITCOMMON_SERVICE_KL_2021_05_20
#define RABBITCOMMON_SERVICE_KL_2021_05_20

#pragma once

#include <QObject>
#include "rabbitcommon_export.h"

namespace RabbitCommon {

class RABBITCOMMON_EXPORT CService : public QObject
{
    Q_OBJECT
    
public:
    explicit CService(const QString &name = QString(),
                      const QString& displayName = QString(),
                      QObject *parent = nullptr);
    virtual ~CService();

    virtual int Start();
    virtual int Stop();
    virtual int ShutDown();
    virtual int Pause();
    virtual int Continue();

    QString Name();
    QString DisplayName();
    
protected:
    virtual int OnRun() = 0;

private:
    QString m_szName;
    QString m_szDisplayName;
};

class RABBITCOMMON_EXPORT CServiceManage : public QObject
{
    Q_OBJECT
    
public:
    virtual ~CServiceManage();

    // Service manage
    virtual int Install() = 0;
    virtual int Uninstall() = 0;
    virtual bool IsInstalled() = 0;

    virtual int Start() = 0;
    virtual int Stop() = 0;
    virtual int Pause();
    virtual int Continue();
    virtual bool IsStoped() = 0;

    virtual int Main(int argc, char* argv[]) = 0;

    virtual QString Name();
    virtual QString DisplayName();

    static CServiceManage* Instance(CService* pService = nullptr);
    CService* GetService();
    
private:
    CService* m_pServer;

protected:
    CServiceManage(CService* pServer,
                   QObject *parent = nullptr);
};

} // namespace RabbitCommon 

#endif // RABBITCOMMON_SERVICE_KL_2021_05_20
