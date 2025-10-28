// Copyright Copyright (c) Kang Lin studio, All Rights Reserved
// Author Kang Lin <kl222@126.com>

#pragma once

#include <QHash>
#include <QQueue>
#include <QSocketNotifier>
#include <QTimer>
#include <curl/curl.h>
#include "WorkerFileTransfer.h"
#include "RequestCurl.h"

/*!
 * \brief File transfer with libcurl
 */
class CWorkerCurl : public CWorkerFileTransfer
{
    Q_OBJECT

public:
    explicit CWorkerCurl(QObject *parent = nullptr);
    virtual ~CWorkerCurl() override;

public:
    virtual int Init() override;
    virtual int Clean() override;
    virtual int Workload() override;

protected:
    virtual void slotAddTask(CTaskFileTransfer *pTask) override;
    virtual void slotRemoveTask(CTaskFileTransfer *pTask) override;

private:
    // socket/timer integration
    static int CurlSocketCallback(CURL *easy, curl_socket_t s, int what, void *userp, void *socketp);
    static int CurlTimerCallback(CURLM *multi, long timeout_ms, void *userp);

    struct SocketInfo {
        curl_socket_t sockfd = CURL_SOCKET_BAD;
        CURL *easy = nullptr;
        QSocketNotifier *readNotifier = nullptr;
        QSocketNotifier *writeNotifier = nullptr;
        QSocketNotifier *exceptionNotifier = nullptr;
    };
    int OnSocket(CURL *easy, int what, curl_socket_t s, void *socketp);
    int OnTimer(long timeout);

    int OnSocketEvent(curl_socket_t s, int ev_bitmask);
    int AddSocketNotifiers(CURL *easy, int what, curl_socket_t s, void *socketp);
    int RemoveSocketNotifiers(CURL *easy);
    int RemoveSocketNotifiers(SocketInfo* si);

    void StartTask(CTaskFileTransfer *pTask);
    void StopTask(CTaskFileTransfer *pTask, bool emitFinished = true);

    void RemoveRequest(CRequestCurl* pReq, bool emitFinished = true);

    void TryStartPending();
    void CheckMultiInfo();

private:
    int m_nMaxConcurrent;
    CURLM* m_pMulti = nullptr;
    QTimer m_Timer;
    
    QQueue<CTaskFileTransfer*> m_PendingTasks;
    QMap<CTaskFileTransfer*, CRequestCurl*> m_Tasks;

    QMap<CURL*, QList<curl_socket_t>*> m_Handles;
    QMap<curl_socket_t, SocketInfo*> m_Sockets;
};
