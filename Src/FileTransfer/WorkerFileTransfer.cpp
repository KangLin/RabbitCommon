// Copyright Copyright (c) Kang Lin studio, All Rights Reserved
// Author Kang Lin <kl222@126.com>

#include <QDir>
#include <QLoggingCategory>
#include <QStandardPaths>
#include "WorkerFileTransfer.h"

#if HAVE_CURL
    #include "WorkerCurl.h"
#endif

static Q_LOGGING_CATEGORY(logTask, "FileTransfer.Task")
static Q_LOGGING_CATEGORY(logWorker, "FileTransfer.Worker")

CTaskFileTransfer::CTaskFileTransfer(QObject *parent)
    : CTaskFileTransfer(QUrl(), QString(), true, parent)
{
    //qDebug(logTask) << Q_FUNC_INFO;
}

CTaskFileTransfer::CTaskFileTransfer(const QUrl& url, const QString& szFile,
                             bool bDownload, QObject *parent)
    : QObject{parent}
    , m_Url(url)
    , m_szFile(szFile)
    , m_bDownload(bDownload)
    , m_bRedirect(true)
    , m_nTimeout(0)
    , m_nConnectTimeout(0)
    , m_bVerifyPeer(false)
    , m_bVerifyHost(false)
    , m_Transferred(0)
    , m_FileSize(0)
    , m_nRetries(0)
    , m_ProxyType(ProxyType::No)
{
    qDebug(logTask) << Q_FUNC_INFO;
    if(!m_Url.fileName().isEmpty() && m_szFile.isEmpty() && m_bDownload){
        m_szFile = QStandardPaths::writableLocation(QStandardPaths::DownloadLocation);
        if(m_szFile.right(1) != "/" && m_szFile.right(1) != "\\")
            m_szFile += QDir::separator();
        m_szFile += m_Url.fileName();
    }

    if(!url.scheme().isEmpty() && GetProtocol().isEmpty())
        SetProtocol(url.scheme().toLower());
    if(!url.userName().isEmpty() && GetUser().isEmpty())
        SetUser(url.userName());
    if(!url.password().isEmpty() && GetPassword().isEmpty())
        SetPassword(url.password());
}

CTaskFileTransfer::~CTaskFileTransfer()
{
    qDebug(logTask) << Q_FUNC_INFO; // << Title();
}

QUrl CTaskFileTransfer::GetUrl() const
{
    return m_Url;
}

void CTaskFileTransfer::SetUrl(const QUrl &newUrl)
{
    m_Url = newUrl;
}

QString CTaskFileTransfer::GetProtocol() const
{
    return m_Protocol;
}

void CTaskFileTransfer::SetProtocol(const QString &newProtocol)
{
    m_Protocol = newProtocol;
}

const QString CTaskFileTransfer::GetFile() const
{
    return m_szFile;
}

void CTaskFileTransfer::SetFile(const QString &newFile)
{
    m_szFile = newFile;
}

bool CTaskFileTransfer::IsDownload() const
{
    return m_bDownload;
}

void CTaskFileTransfer::SetDownload(bool newDownload)
{
    m_bDownload = newDownload;
}

QString CTaskFileTransfer::Title()
{
    QString szDownload;
    if(IsDownload())
        szDownload = " → ";
    else
        szDownload = " ← ";
    return GetUrl().toString() + szDownload + GetFile();
}

const QString CTaskFileTransfer::GetUser() const
{
    return m_szUser;
}

void CTaskFileTransfer::SetUser(const QString &newUser)
{
    m_szUser = newUser;
}

const QString CTaskFileTransfer::GetPassword() const
{
    return m_szPassword;
}

void CTaskFileTransfer::SetPassword(const QString &newPassword)
{
    m_szPassword = newPassword;
}

bool CTaskFileTransfer::IsRedirect() const
{
    return m_bRedirect;
}

void CTaskFileTransfer::SetRedirect(bool newRedirect)
{
    m_bRedirect = newRedirect;
}

long CTaskFileTransfer::GetTimeout() const
{
    return m_nTimeout;
}

void CTaskFileTransfer::SetTimeout(long newTimeout)
{
    m_nTimeout = newTimeout;
}

long CTaskFileTransfer::GetConnectTimeout() const
{
    return m_nConnectTimeout;
}

void CTaskFileTransfer::SetConnectTimeout(long newConnectTimeout)
{
    m_nConnectTimeout = newConnectTimeout;
}

bool CTaskFileTransfer::GetVerifyPeer() const
{
    return m_bVerifyPeer;
}

void CTaskFileTransfer::SetVerifyPeer(bool newVerifyPeer)
{
    m_bVerifyPeer = newVerifyPeer;
}

bool CTaskFileTransfer::GetVerifyHost() const
{
    return m_bVerifyHost;
}

void CTaskFileTransfer::SetVerifyHost(bool newVerifyHost)
{
    m_bVerifyHost = newVerifyHost;
}

QString CTaskFileTransfer::GetCaInfoPath() const
{
    return m_szCaInfoPath;
}

void CTaskFileTransfer::SetCaInfoPath(const QString &newCaInfoPath)
{
    m_szCaInfoPath = newCaInfoPath;
}

QString CTaskFileTransfer::GetSslCertPath() const
{
    return m_szSslCertPath;
}

void CTaskFileTransfer::SetSslCertPath(const QString &newSslCertPath)
{
    m_szSslCertPath = newSslCertPath;
}

QString CTaskFileTransfer::GetSslKeyPath() const
{
    return m_szSslKeyPath;
}

void CTaskFileTransfer::SetSslKeyPath(const QString &newSslKeyPath)
{
    m_szSslKeyPath = newSslKeyPath;
}

qint64 CTaskFileTransfer::GetTransferred() const
{
    return m_Transferred;
}

void CTaskFileTransfer::slotSetTransferred(const qint64 &newTransferred)
{
    m_Transferred = newTransferred;
}

qint64 CTaskFileTransfer::GetFileSize() const
{
    return m_FileSize;
}

void CTaskFileTransfer::slotSetFileSize(const qint64 &newFileSize)
{
    m_FileSize = newFileSize;
}

CTaskFileTransfer::ProxyType CTaskFileTransfer::GetProxyType() const
{
    return m_ProxyType;
}

void CTaskFileTransfer::SetProxyType(const ProxyType &newProxyType)
{
    m_ProxyType = newProxyType;
}

quint16 CTaskFileTransfer::GetProxyPort() const
{
    return m_ProxyPort;
}

void CTaskFileTransfer::SetProxyPort(quint16 newProxyPort)
{
    m_ProxyPort = newProxyPort;
}

QString CTaskFileTransfer::GetProxyUser() const
{
    return m_ProxyUser;
}

void CTaskFileTransfer::SetProxyUser(const QString &newProxyUser)
{
    m_ProxyUser = newProxyUser;
}

QString CTaskFileTransfer::GetProxyPassword() const
{
    return m_ProxyPassword;
}

void CTaskFileTransfer::SetProxyPassword(const QString &newProxyPassword)
{
    m_ProxyPassword = newProxyPassword;
}

QString CTaskFileTransfer::GetProxyHost() const
{
    return m_ProxyHost;
}

void CTaskFileTransfer::SetProxyHost(const QString &newProxyHost)
{
    m_ProxyHost = newProxyHost;
}

int CTaskFileTransfer::GetRetries() const
{
    return m_nRetries;
}

void CTaskFileTransfer::SetRetries(int newRetriesLeft)
{
    m_nRetries = newRetriesLeft;
}

CWorkerFileTransfer::CWorkerFileTransfer(QObject *parent) : CWorker(parent)
{
    //qDebug(logWorker) << Q_FUNC_INFO;
}

CWorkerFileTransfer::~CWorkerFileTransfer()
{
    //qDebug(logWorker) << Q_FUNC_INFO;
}

CWorker* CreateWorkerFileTransfer()
{
    CWorkerFileTransfer* pWorker = nullptr;
#if HAVE_CURL
    pWorker = new CWorkerCurl();
#endif
    return pWorker;
}

int CWorkerFileTransfer::AddTask(CTaskFileTransfer* pTask)
{
    //qDebug(logWorker) << Q_FUNC_INFO;
    QMetaObject::invokeMethod(this, "slotAddTask", Qt::QueuedConnection,
                              Q_ARG(CTaskFileTransfer*, pTask));
    return 0;
}

int CWorkerFileTransfer::RemoveTask(CTaskFileTransfer* pTask)
{
    QMetaObject::invokeMethod(this, "slotRemoveTask", Qt::QueuedConnection,
                              Q_ARG(CTaskFileTransfer*, pTask));
    return 0;
}
