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

    virtual int Start(int argc = 0, char* argv[] = nullptr);
    virtual int Stop();
    virtual int ShutDown();
    virtual int Pause();
    virtual int Continue();

    QString Name();
    QString DisplayName();

protected:
    /**
     * @brief Derived classes implement this function
     * @return quit code
     */
    virtual int OnRun(int argc = 0, char* argv[] = nullptr);

private:
    QString m_szName;
    QString m_szDisplayName;
};

class RABBITCOMMON_EXPORT CServiceManage : public QObject
{
    Q_OBJECT
    
public:
    virtual ~CServiceManage();
    
    static CServiceManage* Instance(CService* pService = nullptr);
    
    // Need admin rights
    virtual int Install(const QString& path = QString(),
                        const QString& name = QString(),
                        const QString& displayName = QString()) = 0;
    virtual int Uninstall(const QString& name = QString()) = 0;
    virtual bool IsInstalled(const QString& name = QString()) = 0;

    virtual int Start(const QString& name = QString(),
                      int argc = 0, const char* argv[] = nullptr) = 0;
    virtual int Stop(const QString& name = QString()) = 0;
    virtual int Pause(const QString& name = QString());
    virtual int Continue(const QString& name = QString());
    virtual bool IsStoped(const QString& name = QString()) = 0;
    virtual bool IsRunning(const QString& name = QString()) = 0;

    virtual int Main(int argc, char* argv[]) = 0;

    virtual QString Name();
    virtual QString DisplayName();

    CService* GetService();
    
private:
    CService* m_pServer;

protected:
    CServiceManage(CService* pServer,
                   QObject *parent = nullptr);
};

} // namespace RabbitCommon 

#endif // RABBITCOMMON_SERVICE_KL_2021_05_20
