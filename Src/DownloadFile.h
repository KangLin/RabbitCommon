#ifndef CDOWNLOADFILE_H
#define CDOWNLOADFILE_H

#include <QUrl>
#include <QFile>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QLoggingCategory>

#include "rabbitcommon_export.h"

namespace RabbitCommon {

/*!
 * \~chinese 从多个 URLS 下载同一个文件
 * - 使用：
 *   - 实例化  RabbitCommon::CDownloadFile 对象：
 *   \snippet Src/FrmUpdater/FrmUpdater.h Instantiate the object of RabbitCommon::CDownloadFile
 *   - 增加需要处理的槽函数：
 *   \snippet Src/FrmUpdater/FrmUpdater.h Add the slot functions of RabbitCommon::CDownloadFile
 *   - 下载, 连接信号：
 *   \snippet Src/FrmUpdater/FrmUpdater.cpp Use RabbitCommon::CDownloadFile download file
 *   - 处理信号：
 *   \snippet Src/FrmUpdater/FrmUpdater.cpp Process the signals of RabbitCommon::CDownloadFile
 *
 * \~english
 * \brief Download the same file from multiple URLs
 * 
 * - Usage:
 *   - Instantiate the object of RabbitCommon::CDownloadFile:
 *   \snippet Src/FrmUpdater/FrmUpdater.h Instantiate the object of RabbitCommon::CDownloadFile
 *   - Add the slot functions of RabbitCommon::CDownloadFile:
 *   \snippet Src/FrmUpdater/FrmUpdater.h Add the slot functions of RabbitCommon::CDownloadFile
 *   - Use RabbitCommon::CDownloadFile download file and connect the signals:
 *   \snippet Src/FrmUpdater/FrmUpdater.cpp Use RabbitCommon::CDownloadFile download file
 *   - Process the signals of RabbitCommon::CDownloadFile:
 *   \snippet Src/FrmUpdater/FrmUpdater.cpp Process the signals of RabbitCommon::CDownloadFile
 *
 * \~
 * \ingroup API
 */
class RABBITCOMMON_EXPORT CDownloadFile : public QObject
{
    Q_OBJECT

public:
    explicit CDownloadFile(QObject *parent = nullptr);
    ~CDownloadFile();

    /*!
     * \~chinese
     * \param urls:
     * \param szFileName: 
     * \param bSequence:
     *     true: 按顺序向 urls 发起请求。前一个请求失败后，才请求后一个。如果成功，则返回。
     *     false: 同时向所有 urls 发起请求。
     *
     * \~english
     * \param urls:
     * \param szFileName: 
     * \param bSequence: Whether the requests are made in order.
     *                   After the previous request fails,
     *                   the latter one is requested.
     *                   Returns if successful
     */
    int Start(QVector<QUrl> urls, QString szFileName = QString(), bool bSequence = false);

signals:
    /*!
     * \param szFile: the file of be download
     */
    void sigFinished(const QString szFile);
    void sigError(int nErr, const QString szErr);
    void sigDownloadProgress(qint64 bytesReceived, qint64 bytesTotal);

private:
    /**
     * @brief DownloadFile
     * @param nIndex: url index
     * @param url: Download url
     * @param bRedirection: true: Is redirection
     * @return 
     */
    int DownloadFile(int nIndex, const QUrl &url, bool bRedirection = false);

private Q_SLOTS:
    void slotReadyRead();
    void slotError(QNetworkReply::NetworkError e);
    void slotSslError(const QList<QSslError> e);
    void slotDownloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    void slotFinished();

private:
    QLoggingCategory m_Log;
    bool m_bSequence;
    QString m_szFileName;
    QVector<QUrl> m_Url;
    QVector<QSharedPointer<QFile> > m_DownloadFile;
    bool m_bDownload;
    QNetworkAccessManager m_NetManager;
    QMap<QNetworkReply*, int> m_Reply;
    qint64 m_nBytesReceived;
    QString m_szError;
};

} // namespace RabbitCommon

#endif // CDOWNLOADFILE_H
