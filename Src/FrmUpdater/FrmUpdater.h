#ifndef FRMUPDATER_H
#define FRMUPDATER_H

#include <QWidget>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QFile>
#include <QPixmap>
#include <QApplication>
#include <QStateMachine>
#include <QUrl>
#include <QButtonGroup>
#include "rabbitcommon_export.h"

namespace Ui {
class CFrmUpdater;
}

class RABBITCOMMON_EXPORT CFrmUpdater : public QWidget
{
    Q_OBJECT

public:
    explicit CFrmUpdater(QWidget *parent = nullptr);
    virtual ~CFrmUpdater();

    /**
     * @brief DownloadFile
     * @param url: 
     * @param bRedirection: true: Is redirection
     * @param bDownload: true: don't check, download immediately
     * @return 
     */
    int DownloadFile(const QUrl &url, bool bRedirection = false,
                     bool bDownload = false);
    int SetVersion(const QString &szVersion);
    int SetArch(const QString &szArch);
    int SetTitle(const QString &szTitle, QPixmap icon = QPixmap());

    /**
     * @brief Update XML file used only to generate programs
     * @return 
     */
    int GenerateUpdateXml();

private Q_SLOTS:
    void slotReadyRead();
    void slotError(QNetworkReply::NetworkError e);
    void slotSslError(const QList<QSslError> e);
    void slotDownloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    void slotFinished();
    void slotButtonClickd(int id);

    void slotCheck();
    void slotDownloadXmlFile();
    void slotDownload();
    void slotUpdate();

    void on_pbOK_clicked();
    void on_pbClose_clicked();

Q_SIGNALS:
    void sigError();
    void sigFinished();
    void sigDownloadFinished();

private:
    int CompareVersion(const QString &newVersion, const QString &currentVersion);
    int InitStateMachine();
    bool IsDownLoad();
    
private:
    Ui::CFrmUpdater *ui;
    QButtonGroup m_ButtonGroup;
    
    QString m_szCurrentVersion;
    QString m_szCurrentArch;

    QUrl m_Url;
    QFile m_DownloadFile;
    bool m_bDownload;
    QNetworkAccessManager m_NetManager;
    QNetworkReply *m_pReply;

    QStateMachine m_StateMachine;

    struct INFO{
        QString szVerion;
        QString szInfomation;
        QString szTime;
        bool bForce;
        QString szSystem;
        QString szPlatform;
        QString szArchitecture;
        QString szUrl;
        QString szMd5sum;
        QString szMinUpdateVersion;
    } m_Info;
    
    int GenerateUpdateXmlFile(const QString &szFile, const INFO &info);
};

#endif // FRMUPDATER_H
