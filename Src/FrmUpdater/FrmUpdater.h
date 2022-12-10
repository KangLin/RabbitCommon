/** @copyright Copyright (c) Kang Lin studio, All Rights Reserved
 *  @author Kang Lin <kl222@126.com>
 *  @abstract Update diaglog
 */

#ifndef RABBITCOMMON_FRMUPDATER_H
#define RABBITCOMMON_FRMUPDATER_H

#include <QSystemTrayIcon>
#include <QWidget>
#include <QFile>
#include <QPixmap>
#include <QApplication>
#include <QStateMachine>
#include <QUrl>
#include <QButtonGroup>
#include <QCommandLineParser>
#include "rabbitcommon_export.h"

#include "DownloadFile.h"

namespace Ui {
class CFrmUpdater;
}

/**
 * @brief Updater
 * @ingroup API
 */
class RABBITCOMMON_EXPORT CFrmUpdater : public QWidget
{
    Q_OBJECT

public:
    explicit CFrmUpdater(QWidget *parent);
    explicit CFrmUpdater(QVector<QUrl> urls = QVector<QUrl>(), QWidget *parent = nullptr);
    virtual ~CFrmUpdater() override;

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

    /*!
     * \brief Set install and automation startup
     * \param bAutoStart
     * \return 
     */
    int SetInstallAutoStartup(bool bAutoStart = true);

protected Q_SLOTS:
    //! [Add the slot functions of RabbitCommon::CDownloadFile]
    void slotDownloadError(int nErr, const QString szError);
    void slotDownloadFileFinished(const QString szFile);
    void slotDownloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    //! [Add the slot functions of RabbitCommon::CDownloadFile]

    void slotButtonClickd(int id);
    void slotCheck();
    void slotDownloadFile();
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

    QFile m_DownloadFile;
    bool m_bDownload;
    QVector<QUrl> m_Urls;
    //! [Instantiate the object of RabbitCommon::CDownloadFile]
    QSharedPointer<RabbitCommon::CDownloadFile> m_Download;
    //! [Instantiate the object of RabbitCommon::CDownloadFile]

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
        QString szFileName;
        QVector<QUrl> urls;
        QString szUrlHome;
        QString szMd5sum;
        QString szMinUpdateVersion;
    } m_Info;

    int GenerateUpdateXmlFile(const QString &szFile, const INFO &info);

    // QWidget interface
protected:
    virtual void showEvent(QShowEvent *event) override;
private Q_SLOTS:
    void on_cbPrompt_clicked(bool checked);
    void on_cbHomePage_clicked(bool checked);
};

#endif // RABBITCOMMON_FRMUPDATER_H
