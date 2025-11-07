// Copyright Copyright (c) Kang Lin studio, All Rights Reserved
// Author Kang Lin <kl222@126.com>

#pragma once

#include <QUrl>
#include <QAtomicInteger>
#include "Worker.h"
#include "rabbitcommon_export.h"

/*!
 * \brief File transfer task
 * \note
 *   - It is running in main thread(UI thread)
 *   - The owner is the caller
 */
class RABBITCOMMON_EXPORT CTaskFileTransfer : public QObject
{
    Q_OBJECT

public:
    CTaskFileTransfer(QObject *parent = nullptr);
    explicit CTaskFileTransfer(const QUrl& url, const QString& szFile = QString(),
                           bool bDownload = true, QObject *parent = nullptr);
    virtual ~CTaskFileTransfer() override;

    QString Title();

    // Base options
    QUrl GetUrl() const;
    void SetUrl(const QUrl &newUrl);
    QString GetProtocol() const;
    void SetProtocol(const QString &newProtocol);
    const QString GetUser() const;
    void SetUser(const QString &newUser);
    const QString GetPassword() const;
    void SetPassword(const QString &newPassword);
    const QString GetFile() const;
    void SetFile(const QString &newFile);
    bool IsDownload() const;
    void SetDownload(bool newDownload);
    bool IsRedirect() const;
    void SetRedirect(bool newRedirect);

    long GetTimeout() const;
    void SetTimeout(long newTimeout);
    long GetConnectTimeout() const;
    void SetConnectTimeout(long newConnectTimeout);

    // TLS
    bool GetVerifyPeer() const;
    void SetVerifyPeer(bool newVerifyPeer);
    bool GetVerifyHost() const;
    void SetVerifyHost(bool newVerifyHost);
    QString GetCaInfoPath() const;
    void SetCaInfoPath(const QString &newCaInfoPath);
    QString GetSslCertPath() const;
    void SetSslCertPath(const QString &newSslCertPath);
    QString GetSslKeyPath() const;
    void SetSslKeyPath(const QString &newSslKeyPath);

    /*!
     * \~chinese 重试次数
     * \~english Number of retries
     */
    int GetRetries() const;
    void SetRetries(int newRetriesLeft);

    qint64 GetTransferred() const;
    qint64 GetFileSize() const;

    // Proxy
    enum class ProxyType {
        No,
        Socks4,
        Socks4A,
        Socks5,
        Socks5HostName,
        Http,
        Http1_0,
        Https,
        Https2
    };
    Q_ENUM(ProxyType)
    ProxyType GetProxyType() const;
    void SetProxyType(const ProxyType &newProxyType);
    QString GetProxyHost() const;
    void SetProxyHost(const QString &newProxyHost);
    quint16 GetProxyPort() const;
    void SetProxyPort(quint16 newProxyPort);
    QString GetProxyUser() const;
    void SetProxyUser(const QString &newProxyUser);
    QString GetProxyPassword() const;
    void SetProxyPassword(const QString &newProxyPassword);

public Q_SLOTS:
    void slotSetTransferred(const qint64 &newTransferred);    
    void slotSetFileSize(const qint64 &newFileSize);

Q_SIGNALS:
    void sigProcess(qint64 nProcessed, qint64 nTotal);
    void sigFinished();
    void sigError(int nErr, const QString& szError);

private:
    QUrl m_Url;
    QString m_Protocol;
    QString m_szFile;
    bool m_bDownload;
    QString m_szUser;
    QString m_szPassword;

    bool m_bRedirect;

    long m_nTimeout;
    long m_nConnectTimeout;

    bool m_bVerifyPeer;
    bool m_bVerifyHost;

    QString m_szCaInfoPath;
    QString m_szSslCertPath;
    QString m_szSslKeyPath;

    qint64 m_Transferred;
    qint64 m_FileSize;

    int m_nRetries;

    ProxyType m_ProxyType;
    QString m_ProxyHost;
    quint16 m_ProxyPort;
    QString m_ProxyUser;
    QString m_ProxyPassword;
};

/*!
 * \brief File transfer worker
 * \note
 *   - It is running in worker thread
 *   - The owner is the CThreadPool
 *
 * Since 2.4.0
 */
class RABBITCOMMON_EXPORT CWorkerFileTransfer : public CWorker
{
    Q_OBJECT

public:
    explicit CWorkerFileTransfer(QObject* parent = nullptr);
    virtual ~CWorkerFileTransfer() override;

    /*!
     * \brief Add Task
     * \note It is called in main thread(UI thread)
     */
    virtual int AddTask(CTaskFileTransfer* pTask);
    /*!
     * \brief Remove Task
     * \note It is called in main thread(UI thread)
     */
    virtual int RemoveTask(CTaskFileTransfer* pTask);

protected Q_SLOTS:
    /*!
     * \brief slotAddTask
     * \param pTask
     * \note It is running in worker thread
     */
    virtual void slotAddTask(CTaskFileTransfer* pTask) = 0;
    /*!
     * \brief slotRemoveTask
     * \param pTask
     * \note It is running in worker thread
     */
    virtual void slotRemoveTask(CTaskFileTransfer* pTask) = 0;
};

//! Create file transfer worker
RABBITCOMMON_EXPORT CWorker *CreateWorkerFileTransfer();
