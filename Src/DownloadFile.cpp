#include "DownloadFile.h"

#include <QStandardPaths>
#include <QCoreApplication>
#include <QDir>

namespace RabbitCommon {

CDownloadFile::CDownloadFile(QVector<QUrl> urls, bool bSequence, QObject *parent)
    : QObject{parent},
      m_Log("RabbitCommon.DownloadFile"),
      m_bSequence(bSequence)
{
    // Check urls
    if(urls.isEmpty())
    {
        m_szError = "Urls is empty.";
        return;
    }
    QString szPath = urls[0].path();
    QString szFile = szPath.mid(szPath.lastIndexOf("/"));
    foreach(auto u, urls)
    {
        QString p = u.path();
        QString f = p.mid(p.lastIndexOf("/"));
        if(f != szFile)
        {
            m_szError = "Urls is not same file:" + szFile + "!=" + f;
            qCritical(m_Log) << m_szError;
            return;
        }
    }

    m_Url = urls;
    if(m_bSequence)
    {
        DownloadFile(0, m_Url[0]);
    } else {
        for(int i = 0; i < m_Url.length(); i++)
            DownloadFile(i, m_Url[i]);
    }
}

CDownloadFile::~CDownloadFile()
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
}

int CDownloadFile::DownloadFile(int nIndex, const QUrl &url, bool bRedirection)
{
    int nRet = 0;
    qDebug(m_Log) << "CDownloadFile::DownloadFile:"
          << nIndex << url << bRedirection;

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
            qCritical(m_Log) << m_szError;
            return -2;
        }
        file = m_DownloadFile.at(nIndex);
    } else {
        if(nIndex < m_DownloadFile.size())
        {
            m_szError = "The index[" + QString::number(nIndex)
                    + "] is in of bounds. [0 -"
                    + QString::number(m_DownloadFile.length())
                    + "]";
            qCritical(m_Log) << m_szError;
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
        QString szFile = szTmp + szPath.mid(szPath.lastIndexOf("/"));

        qDebug(m_Log) << "Download file: " << szFile;
        file = QSharedPointer<QFile>(new QFile());
        if(!file)
        {
            m_szError = "new QFile fail";
            qCritical(m_Log) << m_szError;
            return -1;
        }
        file->setFileName(szFile);
        m_DownloadFile.insert(nIndex, file);
    }

    if(file->isOpen())
        file->close();

    if(url.isLocalFile())
    {
        file->setFileName(url.toLocalFile());
        emit sigFinished(file->fileName());
        return 0;
    }

    if(!file->open(QIODevice::WriteOnly))
    {
        m_szError = "Open file fail: " + file->fileName();
        qDebug(m_Log) << m_szError;
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

void CDownloadFile::slotReadyRead()
{
    //qDebug() << "CDownloadFile::slotReadyRead()";
    QNetworkReply* pReply = dynamic_cast<QNetworkReply*>(this->sender());
    if(m_Reply.find(pReply) == m_Reply.end())
    {
        qCritical(m_Log) << "The reply isn't exits";
        return;
    }
    QSharedPointer<QFile> file = m_DownloadFile[m_Reply[pReply]];
    if(file && file->isOpen())
    {
        QByteArray d = pReply->readAll();
        //qDebug() << d;
        file->write(d);
    }
}

void CDownloadFile::slotFinished()
{
    qDebug(m_Log) << "CDownloadFile::slotFinished()";
    QNetworkReply* pReply = dynamic_cast<QNetworkReply*>(this->sender());
    if(m_Reply.find(pReply) == m_Reply.end())
    {
        qCritical(m_Log) << "The reply isn't exits";
        return;
    }
    int nIndex = m_Reply[pReply];
    QSharedPointer<QFile> file = m_DownloadFile[nIndex];

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
            qDebug(m_Log) << "CDownloadFile::slotFinished():redirectionTarget:url:" << u;
            DownloadFile(nIndex, u, true);
        }
        return;
    }

    if(pReply)
    {
        pReply->disconnect();
        m_Reply.remove(pReply);
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

void CDownloadFile::slotError(QNetworkReply::NetworkError e)
{
    qDebug(m_Log) << "CDownloadFile::slotError: " << e;
    QNetworkReply* pReply = dynamic_cast<QNetworkReply*>(this->sender());
    int nIndex = m_Reply[pReply];
    QSharedPointer<QFile> file = m_DownloadFile[nIndex];
    if(pReply)
    {
        pReply->disconnect();
        m_Reply.remove(pReply);
        pReply->deleteLater();
    }
    file->close();

    if(m_bSequence && (nIndex < m_Url.size() - 1))
    {
        nIndex++;
        DownloadFile(nIndex, m_Url[nIndex]);
        return;
    }

    if(m_Reply.empty())
        emit sigError(e, m_szError);
}

void CDownloadFile::slotSslError(const QList<QSslError> e)
{
    qDebug(m_Log) << "CDownloadFile::slotSslError: " << e;
    QString sErr;
    foreach(QSslError s, e)
        sErr += s.errorString() + " ";
    m_szError = sErr;
    QNetworkReply* pReply = dynamic_cast<QNetworkReply*>(this->sender());
    int nIndex = m_Reply[pReply];
    QSharedPointer<QFile> file = m_DownloadFile[nIndex];
    if(pReply)
    {
        pReply->disconnect();
        m_Reply.remove(pReply);
        pReply->deleteLater();
    }
    file->close();

    if(m_bSequence && (nIndex < m_Url.size() - 1))
    {
        nIndex++;
        DownloadFile(nIndex, m_Url[nIndex]);
        return;
    }

    if(m_Reply.empty())
        emit sigError(-1, m_szError);
}

void CDownloadFile::slotDownloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    QNetworkReply* pReply = dynamic_cast<QNetworkReply*>(this->sender());
    int nIndex = m_Reply[pReply];
    QSharedPointer<QFile> file = m_DownloadFile[nIndex];

    if(bytesTotal > 0)
        qDebug(m_Log) << m_Url[nIndex] << ";" << file->fileName()
                      << tr(": downloading %1%").arg(
                            QString::number(bytesReceived * 100 / bytesTotal));
}

} // namespace RabbitCommon
