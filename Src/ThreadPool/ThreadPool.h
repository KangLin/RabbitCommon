// Copyright Copyright (c) Kang Lin studio, All Rights Reserved
// Author Kang Lin <kl222@126.com>

#pragma once
#include <QVector>
#include <QThread>
#include <QMutex>
#include "Worker.h"

/*!
 * \brief The CThreadPool class
 * Since 2.4.0
 */
class RABBITCOMMON_EXPORT CThreadPool : public QObject
{
    Q_OBJECT

public:
    explicit CThreadPool(QObject *parent = nullptr);
    virtual ~CThreadPool() override;

    /*! Start pool with N workers
     *  \param cb: create worker callback function. \see CWorker *CreateWorkerDownload()
     *  \param maxThreadCount: Max thread counts
     */
    virtual int Start(std::function<CWorker*()> cb,
                      int maxThreadCount = QThread::idealThreadCount()/*std::thread::hardware_concurrency()*/);
    //! Stop all workers and clean up
    virtual int Stop();

    //! Choose worker
    virtual CWorker* ChooseWorker();

    enum class State{
        Stop,
        Start,
        Running
    };
    Q_ENUM(State)
    State GetState();

Q_SIGNALS:
    void sigMessage(const QString &message);
    void sigError(int nErr, const QString &szError = QString());
    void sigRunning();
    void sigFinished(bool success = true, const QString &message = QString());

protected:
    State m_State;
    QVector<QThread*> m_Threads;
    QVector<CWorker*> m_Workers;
    QMutex m_MutexWorkers;
    int m_nMaxThreadCount;
    int m_nNextId;
};
