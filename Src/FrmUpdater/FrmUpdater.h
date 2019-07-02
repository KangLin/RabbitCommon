#ifndef RABBITCOMMON_FRMUPDATER_H
#define RABBITCOMMON_FRMUPDATER_H

#include <QSystemTrayIcon>
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

namespace RabbitCommon {

class RABBITCOMMON_EXPORT CFrmUpdater : public QWidget
{
    Q_OBJECT

public:
    explicit CFrmUpdater(QString szUrl = QString(), QWidget *parent = nullptr);
    virtual ~CFrmUpdater();

    /**
     * @brief DownloadFile
     * @param url: Download url
     * @param bRedirection: true: Is redirection
     * @param bDownload: true: don't check, download immediately
     * @return 
     */
    int DownloadFile(const QUrl &url, bool bRedirection = false,
                     bool bDownload = false);
    int SetVersion(const QString &szVersion);
    int SetArch(const QString &szArch);
    int SetTitle(QPixmap icon = QPixmap(), const QString &szTitle = QString());

    /**
     * @brief Update XML file used only to generate programs
     * @return 
     */
    int GenerateUpdateXml();

protected Q_SLOTS:
    void slotReadyRead();
    void slotError(QNetworkReply::NetworkError e);
    void slotSslError(const QList<QSslError> e);
    void slotDownloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    void slotFinished();
    void slotButtonClickd(int id);

    void slotCheck();
    void slotDownloadXmlFile();
    virtual void slotCheckXmlFile();
    void slotDownloadSetupFile();
    void slotUpdate();
    void slotStateFinished();

    void on_pbOK_clicked();
    void on_pbClose_clicked();
    void slotShowWindow(QSystemTrayIcon::ActivationReason reason);
    
Q_SIGNALS:
    void sigError();
    void sigFinished();
    void sigDownLoadRedireXml();

private:
    int CompareVersion(const QString &newVersion, const QString &currentVersion);
    int InitStateMachine();
    bool IsDownLoad();
    int CheckRedirectXmlFile();
    int CheckUpdateXmlFile();
    
private:
    Ui::CFrmUpdater *ui;
    QSystemTrayIcon m_TrayIcon;
    
    QButtonGroup m_ButtonGroup;
    
    QString m_szCurrentVersion;
    QString m_szCurrentArch;
    QString m_szPlatform;

    QUrl m_Url;
    QFile m_DownloadFile;
    bool m_bDownload;
    QNetworkAccessManager m_NetManager;
    QNetworkReply *m_pReply;

    QStateMachine m_StateMachine;
    QState *m_pStateDownloadSetupFile;
    
    struct INFO{
        QString szVerion;
        QString szInfomation;
        QString szTime;
        bool bForce;
        QString szSystem;
        QString szPlatform;
        QString szArchitecture;
        QString szUrl;
        QString szUrlHome;
        QString szMd5sum;
        QString szMinUpdateVersion;
    } m_Info;
    
    int GenerateUpdateXmlFile(const QString &szFile, const INFO &info);
    
    // QWidget interface
protected:
    virtual void showEvent(QShowEvent *event) override;
};

} //namespace RabbitCommon

#endif // RABBITCOMMON_FRMUPDATER_H
