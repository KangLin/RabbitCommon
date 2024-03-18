#include "Download.h"

#include <QStandardPaths>
#include <QCoreApplication>
#include <QDir>
#include <QLoggingCategory>

namespace RabbitCommon {

static Q_LOGGING_CATEGORY(log, "RabbitCommon.DownloadFile")

CDownload::CDownload(QObject *parent)
    : QObject{parent},
    m_bSequence(false),
    m_nBytesReceived(0)
{}

int CDownload::Start(QVector<QUrl> urls, QString szFileName, bool bSequence)
{
    int nRet = 0;
    if(urls.isEmpty())
    {
        m_szError = "Urls is empty.";
        return -1;
    }
    /*
    QString szPath = urls[0].path();
    QString szFile = szPath.mid(szPath.lastIndexOf("/"));
    foreach(auto u, urls)
    {
        QString p = u.path();
        QString f = p.mid(p.lastIndexOf("/"));
        if(f != szFile)
        {
            m_szError = "Urls is not same file:" + szFile + "!=" + f;
            qCritical(log) << m_szError;
            return;
        }
    }//*/

    m_Url = urls;
    m_szFileName = szFileName;
    m_bSequence = bSequence;
    if(m_bSequence)
    {
        nRet = DownloadFile(0, m_Url[0]);
    } else {
        for(int i = 0; i < m_Url.length(); i++)
            DownloadFile(i, m_Url[i]);
    }
    return nRet;
}

CDownload::~CDownload()
{
    QMap<QNetworkReply*, int>::Iterator it;
    it = m_Reply.begin();
    while(it != m_Reply.end())
    {
        QNetworkReply* r = it.key();
        r->disconnect();
        m_Reply.remove(r);
        if(r->isRunning())
            r->abort();
        r->deleteLater();
        it = m_Reply.begin();
    }

    foreach(auto f, m_DownloadFile)
    {
        if(f->isOpen())
            f->close();
    }
}

int CDownload::DownloadFile(int nIndex, const QUrl &url, bool bRedirection)
{
    int nRet = 0;

    m_nBytesReceived = 0;
    QSharedPointer<QFile> file;
    if(bRedirection)
    {
        if(nIndex < 0 || nIndex >= m_DownloadFile.size())
        {
            m_szError = "The index["
                    + QString::number(nIndex)
                    + "] is out of bounds. [0 -"
                    + QString::number(m_DownloadFile.length())
                    + "]";
            qCritical(log) << m_szError;
            return -2;
        }
        file = m_DownloadFile.at(nIndex);
        if(!file)
        {
            m_szError = "file is null. index: "
                        + QString::number(nIndex);
            qCritical(log) << m_szError;
            return -1;
        }
    } else {
        if(nIndex < m_DownloadFile.size())
        {
            m_szError = "The index[" + QString::number(nIndex)
                    + "] is in of bounds. [0 -"
                    + QString::number(m_DownloadFile.length())
                    + "]";
            qCritical(log) << m_szError;
            return -3;
        }
        QString szTmp
                = QStandardPaths::writableLocation(QStandardPaths::TempLocation);
        szTmp = szTmp + QDir::separator() + "Rabbit"
                + QDir::separator() + qApp->applicationName()
                + QDir::separator() + QString::number(nIndex);
        QDir d;
        if(!d.exists(szTmp))
            d.mkpath(szTmp);
        QString szPath = url.path();
        if(m_szFileName.isEmpty())
        {
            m_szFileName = szPath.mid(szPath.lastIndexOf("/"));
        }
        if(m_szFileName.left(1) != "/" && m_szFileName.left(1) != "\\")
            m_szFileName = QDir::separator() + m_szFileName;
        QString szFile = szTmp + m_szFileName;
        
        file = QSharedPointer<QFile>(new QFile(), &QObject::deleteLater);
        if(!file)
        {
            m_szError = "new QFile fail";
            qCritical(log) << m_szError;
            return -1;
        }
        file->setFileName(szFile);
        m_DownloadFile.insert(nIndex, file);
    }
    
    if(!file)
    {
        m_szError = "file is null. index:" + QString::number(nIndex) ;
        qCritical(log) << m_szError;
        return -1;
    }

    qInfo(log) << "Download file:"
                 << nIndex << url
                 << "(redirection:" << bRedirection << ")"
                 << file->fileName();

    if(file->isOpen())
        file->close();

    if(url.isLocalFile())
    {
        qDebug(log) << "Is local file:" << url;
        file->setFileName(url.toLocalFile());
        if(QFile::exists(file->fileName()))
        {
            emit sigFinished(file->fileName());
            return 0;
        }

        QString szErr = tr("The file is not exists: ") + file->fileName();
        qCritical(log) << szErr;
        emit sigError(-4, szErr);
        return -4;
    }

    if(!file->open(QIODevice::WriteOnly))
    {
        m_szError = "Open file fail: " + file->fileName();
        qDebug(log) << m_szError;
        return -1;
    }

    QNetworkRequest request(url);
    //https://blog.csdn.net/itjobtxq/article/details/8244509
    /*QSslConfiguration config;
    config.setPeerVerifyMode(QSslSocket::VerifyNone);
    config.setProtocol(QSsl::AnyProtocol);
    request.setSslConfiguration(config);
    */
    QNetworkReply* pReply = m_NetManager.get(request);
    if(!pReply)
        return -1;

    m_Reply.insert(pReply, nIndex);

    bool check = false;
    check = connect(pReply, SIGNAL(readyRead()), this, SLOT(slotReadyRead()));
    Q_ASSERT(check);
    check = connect(pReply, SIGNAL(downloadProgress(qint64, qint64)),
                    this, SLOT(slotDownloadProgress(qint64, qint64)));
    Q_ASSERT(check);
    check = connect(pReply, 
                    #if QT_VERSION >= QT_VERSION_CHECK(5, 15, 0)
                        SIGNAL(errorOccurred(QNetworkReply::NetworkError)),
                    #else
                        SIGNAL(error(QNetworkReply::NetworkError)),
                    #endif
                    this, SLOT(slotError(QNetworkReply::NetworkError)));
    Q_ASSERT(check);
    check = connect(pReply, SIGNAL(sslErrors(const QList<QSslError>)),
                    this, SLOT(slotSslError(const QList<QSslError>)));
    Q_ASSERT(check);
    check = connect(pReply, SIGNAL(finished()),
                    this, SLOT(slotFinished()));
    Q_ASSERT(check);

    return nRet;
}

void CDownload::slotReadyRead()
{
    //qDebug() << "CDownload::slotReadyRead()";
    QNetworkReply* pReply = dynamic_cast<QNetworkReply*>(this->sender());
    if(m_Reply.find(pReply) == m_Reply.end())
    {
        qCritical(log) << "The reply isn't exits";
        return;
    }
    QSharedPointer<QFile> file = m_DownloadFile[m_Reply[pReply]];
    if(!file)
    {
        qCritical(log) << "The file isn't exits";
        return;
    }
    if(file && file->isOpen())
    {
        QByteArray d = pReply->readAll();
        //qDebug() << d;
        file->write(d);
    }
}

void CDownload::slotFinished()
{
    QNetworkReply* pReply = dynamic_cast<QNetworkReply*>(this->sender());
    if(m_Reply.find(pReply) == m_Reply.end())
    {
        qCritical(log) << "The reply isn't exits" << pReply->url();
        return;
    }
    int nIndex = m_Reply[pReply];
    qInfo(log) << "Download finished." << nIndex << pReply->url();
    QSharedPointer<QFile> file = m_DownloadFile[nIndex];
    if(!file)
    {
        qCritical(log) << "The file isn't exits." << file->fileName();
        return;
    }

    file->close();

    QVariant redirectionTarget;
    if(pReply)
       redirectionTarget = pReply->attribute(QNetworkRequest::RedirectionTargetAttribute);
    if(redirectionTarget.isValid())
    {
        if(pReply)
        {
            pReply->disconnect();
            m_Reply.remove(pReply);
            pReply->deleteLater();
        }
        QUrl u = redirectionTarget.toUrl();  
        if(u.isValid())
        {
            qDebug(log) << "redirectionTarget:url:" << u;
            DownloadFile(nIndex, u, true);
        }
        return;
    }

    if(pReply)
    {
        m_Reply.remove(pReply);
        pReply->disconnect();
        pReply->deleteLater();
    }

    if(!m_bSequence)
    {
        QMap<QNetworkReply*, int>::Iterator it;
        it = m_Reply.begin();
        while(it != m_Reply.end())
        {
            QNetworkReply* r = it.key();
            if(r == pReply)
            {
                it++;
                continue;
            }
            m_Reply.remove(r);
            if(r->isRunning())
            {
                r->disconnect();
                r->abort();
                r->deleteLater();
            }
            it = m_Reply.begin();
        }
    }

    emit sigFinished(file->fileName());
}

void CDownload::slotError(QNetworkReply::NetworkError e)
{  
    QNetworkReply* pReply = dynamic_cast<QNetworkReply*>(this->sender());
    if(m_Reply.find(pReply) == m_Reply.end())
    {
        qCritical(log) << "Network error: The reply isn't exits."
                         << pReply->url() << "NetworkError:" << e;
        return;
    }
 
    int nIndex = m_Reply[pReply];
    qDebug(log) << "Network error[" << e << "]:" << pReply->errorString()
                << "index:" << nIndex << pReply->url();
    QSharedPointer<QFile> file = m_DownloadFile[nIndex];
    if(pReply)
    {
        m_Reply.remove(pReply);
        pReply->disconnect();
        pReply->deleteLater();
    }
    if(!file)
    {
        qCritical(log) << "Network error: The file is null. index: "
                              + QString::number(nIndex);
        return;
    }
    file->close();

    if(m_bSequence && (nIndex < m_Url.size() - 1))
    {
        nIndex++;
        DownloadFile(nIndex, m_Url[nIndex]);
        return;
    }

    if(m_Reply.empty())
    {
        if(m_szError.isEmpty()) {
            m_szError = "Network error: ";
            if(!pReply->errorString().isEmpty())
                m_szError += pReply->errorString() + "; ";
            m_szError += "from " + m_Url[nIndex].toString()
                         + " to " + file->fileName();
        }
        emit sigError(e, m_szError);
    }
}

void CDownload::slotSslError(const QList<QSslError> e)
{
    QString sErr;
    foreach(QSslError s, e)
        sErr += s.errorString() + " ";
    m_szError = sErr;
    QNetworkReply* pReply = dynamic_cast<QNetworkReply*>(this->sender());
    if(m_Reply.find(pReply) == m_Reply.end())
    {
        qCritical(log) << "ssl error: The reply isn't exits. QSslError:" << e;
        return;
    }
    int nIndex = m_Reply[pReply];
    qDebug(log) << "ssl error[" << e << "]:" << pReply->errorString()
                << "index:" << nIndex << pReply->url();
    QSharedPointer<QFile> file = m_DownloadFile[nIndex];
    if(pReply)
    {
        m_Reply.remove(pReply);
        pReply->disconnect();
        pReply->deleteLater();
    }
    if(!file)
    {
        qCritical(log) << "ssl error: The file null. index:"
                              + QString::number(nIndex);
        return;
    }
    file->close();

    if(m_bSequence && (nIndex < m_Url.size() - 1))
    {
        nIndex++;
        DownloadFile(nIndex, m_Url[nIndex]);
        return;
    }

    if(m_Reply.empty())
    {
        if(m_szError.isEmpty())
        {
            m_szError = "ssl error: ";
            if(!pReply->errorString().isEmpty())
                m_szError += pReply->errorString() + "; ";
             m_szError += "from " + m_Url[nIndex].toString()
                    + " to " + file->fileName();
        }
        emit sigError(-1, m_szError);
    }
}

void CDownload::slotDownloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    QNetworkReply* pReply = dynamic_cast<QNetworkReply*>(this->sender());
    if(!pReply)
    {
        qCritical(log) << "slotDownloadProgress: Teh sender is not reply";
        return;
    }
    if(m_Reply.find(pReply) == m_Reply.end())
    {
        qCritical(log) << "slotDownloadProgress: The reply isn't exits"
                         << pReply->url();
        return;
    }
    int nIndex = m_Reply[pReply];
    QSharedPointer<QFile> file = m_DownloadFile[nIndex];
    if(!file)
    {
        qCritical(log) << "slotDownloadProgress: The file null. index: "
                              + QString::number(nIndex);
        return;
    }

    if(bytesTotal > 0)
    {
        if(m_nBytesReceived < bytesReceived)
        {
            m_nBytesReceived = bytesReceived;
            emit sigDownloadProgress(m_nBytesReceived, bytesTotal);
        }
        qDebug(log) << tr("Downloading %1% [%2/%3]")
                       .arg(QString::number(bytesReceived * 100 / bytesTotal))
         .arg(QString::number(bytesReceived)).arg(QString::number(bytesTotal))
            << m_Url[nIndex] << file->fileName();
    }
}

} // namespace RabbitCommon
