// Copyright Copyright (c) Kang Lin studio, All Rights Reserved
// Author Kang Lin <kl222@126.com>

#include <QMutexLocker>
#include <QLoggingCategory>
#include <QThread>
#include "WorkerCurl.h"

static Q_LOGGING_CATEGORY(log, "FileTransfer.Worker.Curl")

CWorkerCurl::CWorkerCurl(QObject *parent)
    : CWorkerFileTransfer{parent}
    , m_nMaxConcurrent(QThread::idealThreadCount())
    , m_pMulti(nullptr)
{
    qDebug(log) << Q_FUNC_INFO;
    return;
    curl_version_info_data *version_info = curl_version_info(CURLVERSION_NOW);
    QString szVersion;
    szVersion += tr("Version:") + " " + version_info->version + "\n";
    szVersion += tr("Supported protocols:") + " ";
    if (version_info->protocols) {
        for (int i = 0; version_info->protocols[i]; i++) {
            szVersion += " ";
            szVersion += version_info->protocols[i];
        }
    }
    szVersion += "\n";
#if CURL_VERSION_SSL
    szVersion += QString("SSL supported:") + (version_info->features & CURL_VERSION_SSL ? "Yes" : "No") + "\n";
#endif
#if CURL_VERSION_LIBSSH2
    szVersion += QString("LIBSSH2 supported:") + (version_info->features & CURL_VERSION_LIBSSH2 ? "Yes" : "No") + "\n";
#endif
    qDebug(log, "%s", szVersion.toStdString().c_str());
}

CWorkerCurl::~CWorkerCurl()
{
    qDebug(log) << Q_FUNC_INFO;
}

int CWorkerCurl::Init()
{
    int nRet = 0;

    m_pMulti = curl_multi_init();
    if (!m_pMulti) {
        QString szMsg = "curl_multi_init failed";
        qCritical(log) << szMsg;
        return -1;
    }

    curl_multi_setopt(m_pMulti, CURLMOPT_SOCKETFUNCTION, &CurlSocketCallback);
    curl_multi_setopt(m_pMulti, CURLMOPT_SOCKETDATA, this);
    curl_multi_setopt(m_pMulti, CURLMOPT_TIMERFUNCTION, &CurlTimerCallback);
    curl_multi_setopt(m_pMulti, CURLMOPT_TIMERDATA, this);

    //*
    m_Timer.setSingleShot(true);
    bool check = connect(&m_Timer, &QTimer::timeout, this, [this]() {
        //qDebug(log) << "Time out";
        if (!m_pMulti) return;
        int running = 0;
        CURLMcode mc = curl_multi_socket_action(m_pMulti, CURL_SOCKET_TIMEOUT, 0, &running);
        if(CURLM_OK != mc) {
            qCritical(log) << "curl_multi_socket_action failed::" << curl_multi_strerror(mc);
        }
        if(m_Tasks.size() != running) {
            CheckMultiInfo();
            TryStartPending();
        }
    });
    Q_ASSERT(check);//*/
    return nRet;
}

int CWorkerCurl::Clean()
{
    //qDebug(log) << Q_FUNC_INFO;
    int nRet = 0;

    auto it = m_Tasks.begin();
    while (m_Tasks.end() != it) {
        CRequestCurl* req = *it;
        if(req)
            StopTask(req->GetTask());
        it = m_Tasks.begin();
    }
    m_PendingTasks.clear();

    while(!m_Handles.isEmpty()) {
        auto it = m_Handles.begin();
        RemoveSocketNotifiers(*it);
    }
    while(!m_Sockets.isEmpty()) {
        auto it = m_Sockets.begin();
        RemoveSocketNotifiers(*it);
    }

    if(m_pMulti) {
        curl_multi_cleanup(m_pMulti);
        m_pMulti = nullptr;
    }
    return nRet;
}

int CWorkerCurl::Workload()
{
    int nRet = 0;
    return nRet;
}

void CWorkerCurl::slotAddTask(CTaskFileTransfer *pTask)
{
    //qDebug(log) << Q_FUNC_INFO;
    if(!m_pMulti) return;
    m_PendingTasks.append(pTask);
    TryStartPending();
}

void CWorkerCurl::slotRemoveTask(CTaskFileTransfer *pTask)
{
    if(m_Tasks.contains(pTask)) {
        StopTask(pTask);
    }
    if(m_PendingTasks.contains(pTask))
        m_PendingTasks.removeAll(pTask);
    TryStartPending();
}

int CWorkerCurl::CurlSocketCallback(CURL *easy, curl_socket_t s, int what, void *userp, void *socketp)
{
    CWorkerCurl* self = static_cast<CWorkerCurl*>(userp);
    if(!self) return 0;
    return self->OnSocket(easy, what, s, socketp);
}

int CWorkerCurl::CurlTimerCallback(CURLM *multi, long timeout_ms, void *userp)
{
    CWorkerCurl* self = static_cast<CWorkerCurl*>(userp);
    if(!self) return 0;
    return self->OnTimer(timeout_ms);
}

int CWorkerCurl::OnSocket(CURL *easy, int what, curl_socket_t s, void *socketp)
{
    //qDebug(log) << Q_FUNC_INFO;
    if(CURL_POLL_REMOVE == what) {
        if (socketp) {
            auto si = static_cast<SocketInfo*>(socketp);
            if (si) RemoveSocketNotifiers(si);
        }
    } else {
        AddSocketNotifiers(easy, what, s, socketp);
    }
    return 0;
}

int CWorkerCurl::OnTimer(long timeout)
{
    //qDebug(log) << "CWorkerCurl::OnTimer:" << timeout;
    //*
    if(timeout < 0)
        m_Timer.stop();
    else {
        int ms = timeout;
        if(0 == ms) ms = 1;
        m_Timer.start(ms);
    }//*/
    return 0;
}

int CWorkerCurl::AddSocketNotifiers(CURL *easy, int what, curl_socket_t s, void *socketp)
{
    //qDebug(log) << Q_FUNC_INFO;
    auto si = static_cast<SocketInfo*>(socketp);
    if(!si) {
        si = new SocketInfo();
        if(si) {
            si->sockfd = s;
            si->easy = easy;
            auto p = m_Handles[easy];
            if(!p)
                p = new QList<curl_socket_t>();
            if(p && !p->contains(s))
                p->append(s);
            m_Sockets.insert(s, si);
            curl_multi_assign(m_pMulti, s, si);
            //qDebug(log) << Q_FUNC_INFO << "new socket info" << m_pMulti << easy << s << si;
        }
    }
    if(!si) return -1;
    Q_ASSERT(si->sockfd == s);

    bool wantRead = (what == CURL_POLL_IN || what == CURL_POLL_INOUT);
    bool wantWrite = (what == CURL_POLL_OUT || what == CURL_POLL_INOUT);

    if (wantRead && !si->readNotifier) {
        si->readNotifier = new QSocketNotifier(
            int(si->sockfd), QSocketNotifier::Read, this);
        connect(si->readNotifier, &QSocketNotifier::activated,
                this, [this, si](int) {
            this->OnSocketEvent(si->sockfd, CURL_CSELECT_IN);
        });
    } else if (!wantRead && si->readNotifier) {
        delete si->readNotifier; si->readNotifier = nullptr;
    }

    if (wantWrite && !si->writeNotifier) {
        si->writeNotifier = new QSocketNotifier(
            int(si->sockfd), QSocketNotifier::Write, this);
        connect(si->writeNotifier, &QSocketNotifier::activated,
                this, [this, si](int) {
            this->OnSocketEvent(si->sockfd, CURL_CSELECT_OUT);
        });
    } else if (!wantWrite && si->writeNotifier) {
        delete si->writeNotifier; si->writeNotifier = nullptr;
    }
    
    if(!si->exceptionNotifier) {
        si->exceptionNotifier = new QSocketNotifier(int(si->sockfd), QSocketNotifier::Exception, this);
        connect(si->exceptionNotifier, &QSocketNotifier::activated,
                this, [this, si](int) {
            this->OnSocketEvent(si->sockfd, CURL_CSELECT_ERR);
        });
    }
    return 0;
}

int CWorkerCurl::RemoveSocketNotifiers(CURL *easy)
{
    auto sockets = m_Handles.value(easy, nullptr);
    if(!sockets) return 0;
    while(sockets && !sockets->isEmpty())
    {
        auto s = sockets->takeFirst();
        sockets->removeFirst();
        if(m_Sockets.contains(s))
            RemoveSocketNotifiers(m_Sockets.value(s));
    }
    delete sockets;
    m_Handles.remove(easy);
    return 0;
}

int CWorkerCurl::RemoveSocketNotifiers(SocketInfo *si)
{
    //qDebug(log) << Q_FUNC_INFO;
    if(!si) return -1;
    if(si->easy) {
        auto sockets = m_Handles.value(si->easy, nullptr);
        if(sockets && sockets->contains(si->sockfd))
            sockets->removeAll(si->sockfd);
    }
    if(CURL_SOCKET_BAD != si->sockfd) {
        curl_multi_assign(m_pMulti, si->sockfd, nullptr);
        m_Sockets.remove(si->sockfd);
    }
    if(si->readNotifier) {
        delete si->readNotifier;
        si->readNotifier = nullptr;
    }
    if(si->writeNotifier) {
        delete si->writeNotifier;
        si->writeNotifier = nullptr;
    }
    if(si->exceptionNotifier) {
        delete si->exceptionNotifier;
        si->exceptionNotifier = nullptr;
    }
    delete si;
    return 0;
}

int CWorkerCurl::OnSocketEvent(curl_socket_t s, int ev_bitmask)
{
    if(!m_pMulti) return -1;
    int running = 0;
    CURLMcode mc = curl_multi_socket_action(m_pMulti, s, ev_bitmask, &running);
    /*
    qDebug(log) << "OnSocketEvent: mc" << mc << "socket:" << s
                << "ev_bitmak:" << ev_bitmask << "running counts:" << running
                << "Task size:" << m_Tasks.size();//*/
    if(CURLM_OK != mc) {
        qCritical(log) << "Error:" << curl_multi_strerror(mc);
    }

    if(m_Tasks.size() != running) {
        CheckMultiInfo();
        TryStartPending();
    }
    return 0;
}

void CWorkerCurl::TryStartPending()
{
    qDebug(log) << Q_FUNC_INFO;
    if(m_Tasks.size() >= m_nMaxConcurrent)
        return;

    if(m_PendingTasks.isEmpty()) return;
    auto task = m_PendingTasks.dequeue();
    if(task)
        StartTask(task);
}

void CWorkerCurl::StartTask(CTaskFileTransfer *pTask)
{
    auto pReq = new CRequestCurl();
    if(!pReq) return;
    int nRet = pReq->Init(pTask);
    if(nRet) return;

    CURLMcode rc = curl_multi_add_handle(m_pMulti, pReq->GetHandle());
    if (rc != CURLM_OK) {
        qCritical(log) << "curl_multi_add_handle failed:" << curl_multi_strerror(rc);
        pReq->Clean();
        return;
    }
    m_Tasks.insert(pTask, pReq);
    qDebug(log) << "Start task:" << pTask->Title();
}

void CWorkerCurl::StopTask(CTaskFileTransfer *pTask, bool emitFinished)
{
    //qDebug(log) << Q_FUNC_INFO;
    if(!pTask) return;
    if(m_Tasks.contains(pTask)) {
        auto req = m_Tasks[pTask];
        if(req) {
            RemoveRequest(req, emitFinished);
        }
        else
            qCritical(log) << "The req is nullptr:" << pTask;

        m_Tasks.remove(pTask);
        qDebug(log) << "Remove task:" << pTask->Title();
    } else {
        qCritical(log) << "The m_Tasks is not contain:" << pTask;
    }
}

void CWorkerCurl::RemoveRequest(CRequestCurl* pReq, bool emitFinished)
{
    qDebug(log) << Q_FUNC_INFO;
    auto handle = pReq->GetHandle();
    if(!pReq || !handle) return;
    if(m_Handles.contains(handle))
        RemoveSocketNotifiers(handle);
    auto rc = curl_multi_remove_handle(m_pMulti, handle);
    if(CURLM_OK != rc)
        qCritical(log) << "curl_multi_remove_handle fail:" << curl_multi_strerror(rc);
    pReq->Clean();
    if(emitFinished)
        emit pReq->sigFinished();
    pReq->deleteLater();
}

void CWorkerCurl::CheckMultiInfo()
{
    qDebug(log) << Q_FUNC_INFO;
    if (!m_pMulti) return;
    CURLMsg *msg = nullptr;
    int msgs_left = 0;
    while ((msg = curl_multi_info_read(m_pMulti, &msgs_left))) {
        qDebug(log) << "curl_multi_info_read:" << msg;
        if (msg->msg == CURLMSG_DONE) {
            CURL *easy = msg->easy_handle;
            CRequestCurl* req = nullptr;
            curl_easy_getinfo(easy, CURLINFO_PRIVATE, &req);
            if (req) {
                req->ShowInfo();
                auto pTask = req->GetTask();
                Q_ASSERT(pTask);
                CURLcode result = msg->data.result;
                bool ok = (result == CURLE_OK);
                if (ok) {
                    long response_code = 0;
                    curl_easy_getinfo(easy, CURLINFO_RESPONSE_CODE, &response_code);
                    qDebug(log) << QString("Transfer finished, response code: %1").arg(response_code);
                    req->SetResponseCode(response_code);
                    StopTask(pTask);
                } else {
                    qCritical(log) << QString("curl error[%1]: %2 (%3)").arg(result)
                                          .arg(curl_easy_strerror(result)).arg(pTask->Title());
                    int retries = 0;
                    switch(result) {
                    case CURLE_COULDNT_RESOLVE_PROXY:
                    case CURLE_COULDNT_RESOLVE_HOST:
                    case CURLE_COULDNT_CONNECT:
                    case CURLE_FTP_ACCEPT_TIMEOUT:
                        break;
                    default:
                        retries = pTask->GetRetries();
                    }
                    // Failure: if retries left, retry immediately; otherwise emit finished.
                    if (retries > 0) {
                        qDebug(log) << "Retries";
                        // cleanup current ctx without emitting finished
                        StopTask(pTask, false);
                        // decrement and re-enqueue with same options
                        pTask->SetRetries(retries - 1);
                        // small delay before retry to avoid tight loop
                        QTimer::singleShot(200, this, [this, pTask]() {
                            if (m_Tasks.size() < m_nMaxConcurrent) {
                                StartTask(pTask);
                            } else {
                                m_PendingTasks.append(pTask);
                            }
                        });
                    } else {
                        emit req->sigError(result, curl_easy_strerror(result));
                        StopTask(pTask);
                    }
                }
            } else {
                curl_multi_remove_handle(m_pMulti, easy);
                curl_easy_cleanup(easy);
                Q_ASSERT(false);
            }
        }
    }
}
