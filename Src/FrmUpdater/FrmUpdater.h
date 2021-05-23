/** @copyright Copyright (c) Kang Lin studio, All Rights Reserved
 *  @author Kang Lin(kl222@126.com)
 *  @abstract Update diaglog
 */

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
#include <QCommandLineParser>
#include "rabbitcommon_export.h"

namespace Ui {
class CFrmUpdater;
}

class RABBITCOMMON_EXPORT CFrmUpdater : public QWidget
{
    Q_OBJECT

public:
    explicit CFrmUpdater(QString szUrl = QString(), QWidget *parent = nullptr);
    virtual ~CFrmUpdater() override;

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
    /**
     * @brief SetTitle
     * @param icon
     * @param szTitle: default is qApp->applicationDisplayName()
     * @return 
     */
    int SetTitle(QImage icon = QImage(), const QString &szTitle = QString());

    /**
     * @brief Update XML file used only to generate programs
     * @return 
     */
    int GenerateUpdateXml();
    int GenerateUpdateXml(QCommandLineParser &parser);
    
    int SetInstallAutoStartup(bool bAutoStart = true);

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
    bool CheckPrompt(const QString &szVersion);
    QString InstallScript(const QString szDownLoadFile,
                          const QString szApplicationName);

private:
    Ui::CFrmUpdater *ui;
    bool m_InstallAutoStartupType;

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
private slots:
    void on_cbPrompt_clicked(bool checked);
    void on_cbHomePage_clicked(bool checked);
};

#endif // RABBITCOMMON_FRMUPDATER_H
