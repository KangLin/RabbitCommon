/** @copyright Copyright (c) Kang Lin studio, All Rights Reserved
 *  @author Kang Lin(kl222@126.com)
 *  @abstract Service
 *  @example 
 * 
 *
class CServiceExample : public RabbitCommon::CService
{
public:
    CServiceExample(){ m_bExit = false; }
    
    int Stop() override {
        m_bExit = true;
        return 0;
    }
    
    // CService interface
protected:
    virtual int OnRun() override {
        LOG_MODEL_DEBUG("CServiceExample", "CServiceExample run ...");
        while(!m_bExit)
        {
            QThread::msleep(500);
            LOG_MODEL_DEBUG("CServiceExample", "OnRun ......");
        }
        LOG_MODEL_DEBUG("CServiceExample", "CServiceExample exit");
        return 0;
    }
    
private:
    bool m_bExit;
};

int main(int argc, char *argv[])
{
    RabbitCommon::CServiceManage::Instance(new RabbitCommon::CService("Name", "DisplayName"))->Main(argc, argv);
    return -1;
}

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
     * @return 0: success
     *     other: fail
     */
    virtual int OnInit(int argc = 0, char* argv[] = nullptr);
    /**
     * @brief Derived classes implement this function
     * @return quit code
     *         0: success
     *     other: fail
     */
    virtual int OnRun();
    /**
     * @brief Derived classes implement this function
     * @return 0: success
     *     other: fail
     */
    virtual int OnClean();

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
