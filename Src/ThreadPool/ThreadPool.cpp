// Copyright Copyright (c) Kang Lin studio, All Rights Reserved
// Author Kang Lin <kl222@126.com>

#include <QLoggingCategory>
#include "ThreadPool.h"

static Q_LOGGING_CATEGORY(log, "Pool")
CThreadPool::CThreadPool(QObject *parent)
    : QObject{parent}
    , m_State(State::Stop)
    , m_nMaxThreadCount(QThread::idealThreadCount())
    , m_nNextId(1)
{
    qDebug(log) << Q_FUNC_INFO;
}

CThreadPool::~CThreadPool()
{
    qDebug(log) << Q_FUNC_INFO;
    QString szStop = metaObject()->className() + QString("::Stop()");
    QString szErr = tr("Forgot to call %1, please call %1 before deleting the object.").arg(szStop);
    Q_ASSERT_X(State::Stop == m_State, "CThreadPool", szErr.toStdString().c_str());
}

int CThreadPool::Start(std::function<CWorker *()> cb, int maxThreadCount)
{
    int nRet = 0;
    if (State::Stop != m_State) return 0;
    if (maxThreadCount <= 0) maxThreadCount = QThread::idealThreadCount();
    m_nMaxThreadCount = maxThreadCount;

    m_Threads.clear();
    m_Workers.clear();
    for (int i = 0; i < m_nMaxThreadCount; ++i) {
        auto* thr = new QThread();
        if(!thr) {
            QString szErr = tr("Start fail: new thread fail.");
            qCritical(log) << szErr;
            emit sigError(-1, szErr);
            Stop();
            return -1;
        }

        bool check = connect(thr, &QThread::started, this,
                             [this, thr, cb]() {
            //qDebug(log) << "Thread started:" << thr << QThread::currentThread();
            auto worker = cb();
            if(!worker) return;
            int nRet = worker->Init();
            if(nRet) {
                //worker->Clean();
                return;
            }

            bool check = connect(thr, &QThread::finished, worker,
                [this, worker]() {
                //qDebug(log) << "Thread finished:" << QThread::currentThread();
                m_MutexWorkers.lock();
                m_Workers.removeOne(worker);
                m_MutexWorkers.unlock();
                worker->Clean();
                worker->deleteLater();
            }, Qt::DirectConnection);
            Q_ASSERT(check);
            
            check = connect(worker, SIGNAL(sigFinished()), thr, SLOT(exit()));
            Q_ASSERT(check);

            int size = 0;
            m_MutexWorkers.lock();
            m_Workers.append(worker);
            size = m_Workers.size();
            m_MutexWorkers.unlock();

            if(size == m_nMaxThreadCount) {
                m_State = State::Running;
                emit sigRunning();
                QString szMsg = tr("%1 worker threads is running").arg(m_Threads.size());
                qInfo(log) << szMsg;
                emit sigMessage(szMsg);
            }
        }, Qt::DirectConnection);
        Q_ASSERT(check);

        check = connect(thr, &QThread::finished, this,
            [this, thr]() {
            //qDebug(log) << "Thread finished:" << QThread::currentThread() << thr;
            m_Threads.removeOne(thr);
        }, Qt::DirectConnection);
        Q_ASSERT(check);
        m_Threads.append(thr);
        thr->start();
    }

    m_State = State::Start;

    return nRet;
}

int CThreadPool::Stop()
{
    int nRet = 0;
    if (State::Stop == m_State) return 0;
    for (int i = 0; i < m_Threads.size(); ++i) {
        auto thr = m_Threads[i];
        if (thr) {
            thr->quit();
            thr->wait();
        }
        delete thr;
    }

    m_Threads.clear();

    m_MutexWorkers.lock();
    m_Workers.clear();
    m_MutexWorkers.unlock();

    m_State = State::Stop;

    emit sigFinished(true);
    QString szMsg = tr("Stopped workers");
    qInfo(log) << szMsg;
    emit sigMessage(szMsg);
    return nRet;
}

CWorker* CThreadPool::ChooseWorker()
{
    if (m_Workers.isEmpty()) return nullptr;
    auto *w = m_Workers.at(m_nNextId % m_Workers.size());
    qDebug(log) << "Choose worker:" << w << m_nNextId;
    m_nNextId = (m_nNextId + 1) % m_Workers.size();
    return w;
}
