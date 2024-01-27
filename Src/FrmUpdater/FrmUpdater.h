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

/*!
 * \~chinese
 * \brief 更新软件
 * \details
 * - 原理：
 *   - 状态机图：详见： \ref InitStateMachine
 *
 * - 使用：
 *   - 生成配置文件:
 *     - [必选] 生成更新配置文件(update_XXX.json)。 格式详见： \ref GetConfigFromFile \n
 *       默认位置： \snippet Src/FrmUpdater/FrmUpdater.cpp Update configure file default urls
 *     - [可选] 生成重定向文件配置文件(update.xml)。 格式详见： \ref CheckRedirectConfigFile \n
 *       默认位置： \snippet Src/FrmUpdater/FrmUpdater.cpp Redirect configure file default urls
 *     - 例子：
 *       \snippet App/Tests/main.cpp Use CFrmUpdater GenerateUpdateJson
 *       \see GenerateUpdateJson() GenerateUpdateJson(QCommandLineParser &parser)
 *   - 在应用程序中使用 CFrmUpdater:
 *     \snippet App/Tests/MainWindow.cpp Use CFrmUpdater
 * 
 * \~english
 * \brief Updater
 * \details
 * - Principle:
 *   - State machine. See: \ref InitStateMachine
 *
 * - Usage:
 *   - Generate configuration file
 *     - [MUST] Generate update configuration file(update_XXX.json).
 *              Format see: \ref GetConfigFromFile \n
 *       Default: \snippet Src/FrmUpdater/FrmUpdater.cpp Update configure file default urls
 *     - [OPTION] Generate redirect configure file(update.xml).
 *             Format see: \ref CheckRedirectConfigFile \n
 *       Default: \snippet Src/FrmUpdater/FrmUpdater.cpp Redirect configure file default urls
 *     - Example:
 *       \snippet App/Tests/main.cpp Use CFrmUpdater GenerateUpdateJson
 *       \see GenerateUpdateJson() GenerateUpdateJson(QCommandLineParser &parser)
 *   - Used by application CFrmUpdater:
 *     \snippet App/Tests/MainWindow.cpp Use CFrmUpdater
 *     
 * \~
 * \see RabbitCommon::CDownloadFile
 * \ingroup API
 */
class RABBITCOMMON_EXPORT CFrmUpdater : public QWidget
{
    Q_OBJECT

public:
    /*!
     * \brief CFrmUpdater
     * \param urls: 重定向配置文件或下载配置文件的下载地址
     * \param parent
     */
    explicit CFrmUpdater(QVector<QUrl> urls = QVector<QUrl>(), QWidget *parent = nullptr);
    virtual ~CFrmUpdater() override;

    int SetVersion(const QString &szVersion);
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
     * \brief Generate update json configure file
     * \note  If your application hasn't a command-line argument, use it.
     * \see GetConfigFromFile
     */
    int GenerateUpdateJson();
    /*!
     * \brief Generate update json configure file
     * \param parser: QCommandLineParser
     * \note  If your application has a command-line argument, use it.
     * \snippet App/Tests/main.cpp Use CFrmUpdater GenerateUpdateJson
     * \see GetConfigFromFile
     */
    int GenerateUpdateJson(QCommandLineParser &parser);

    /*!
     * \brief Set install and automation startup
     * \param bAutoStart
     * \return 
     */
    int SetInstallAutoStartup(bool bAutoStart = true);

protected Q_SLOTS:
    // [Add the slot functions of RabbitCommon::CDownloadFile]
    void slotDownloadError(int nErr, const QString szError);
    void slotDownloadFileFinished(const QString szFile);
    void slotDownloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    // [Add the slot functions of RabbitCommon::CDownloadFile]

    void slotButtonClickd(int id);
    void slotCheck();
    void slotDownloadFile();
    virtual void slotCheckConfigFile();
    void slotDownloadSetupFile();
    void slotUpdate();
    void slotStateFinished();

    void on_pbOK_clicked();
    void on_pbClose_clicked();
    void slotShowWindow(QSystemTrayIcon::ActivationReason reason);

Q_SIGNALS:
    void sigError();
    void sigFinished();
    void sigDownLoadRedire();

private:
    CFrmUpdater(QWidget *parent);
    int CompareVersion(const QString &newVersion, const QString &currentVersion);
    int InitStateMachine();
    bool IsDownLoad();
    int CheckRedirectConfigFile();
    int CheckUpdateConfigFile();
    bool CheckPrompt(const QString &szVersion);
    QString InstallScript(const QString szDownLoadFile,
                          const QString szApplicationName);

private:
    Ui::CFrmUpdater *ui;
    bool m_InstallAutoStartupType;

    QSystemTrayIcon m_TrayIcon;

    QButtonGroup m_ButtonGroup;

    QString m_szCurrentVersion;

    QFile m_DownloadFile;
    bool m_bDownload;
    QVector<QUrl> m_Urls;
    // [Instantiate the object of RabbitCommon::CDownloadFile]
    QSharedPointer<RabbitCommon::CDownloadFile> m_Download;
    // [Instantiate the object of RabbitCommon::CDownloadFile]

    QStateMachine m_StateMachine;
    QState *m_pStateDownloadSetupFile;

    enum class CONFIG_TYPE{
        VERSION,
        FILE,
        VERSION_FILE
    };
    
    struct CONFIG_VERSION {
        QString szVerion;
        QString szMinUpdateVersion;
        QString szInfomation;
        QString szTime;
        bool bForce;
        QString szHome;
    };

    struct CONFIG_FILE {
        QString szSystem;
        QString szSystemMinVersion;
        QString szArchitecture;
        QString szArchitectureMinVersion;
        QString szFileName;
        QVector<QUrl> urls;
        QString szMd5sum;
    };

    struct CONFIG_INFO {
        CONFIG_VERSION version;
        QVector<CONFIG_FILE> files;
    } m_Info;
    
    CONFIG_FILE m_ConfigFile;
    
    int GetConfigFromFile(const QString& szFile, CONFIG_INFO &conf);
    int GetConfigFromCommandLine(/*[in]*/QCommandLineParser &parser,
                /*[out]*/QString &szFile,
                /*[out]*/CONFIG_INFO &info,
                /*[out]*/CONFIG_TYPE &type);
    int GenerateUpdateXmlFile(const QString &szFile,
                              const CONFIG_INFO &info,
                              CONFIG_TYPE &type);
    int GenerateJsonFile(const QString &szFile,
                         const CONFIG_INFO &info,
                         CONFIG_TYPE type);

    // QWidget interface
protected:
    virtual void showEvent(QShowEvent *event) override;
private Q_SLOTS:
    void on_cbPrompt_clicked(bool checked);
    void on_cbHomePage_clicked(bool checked);

#if defined(HAVE_TEST)
    // Only test!!!
public:
    int test_json();
    void test_generate_json_file();
    void test_json_file();
    void test_default_json_file();

#endif
};

#endif // RABBITCOMMON_FRMUPDATER_H
