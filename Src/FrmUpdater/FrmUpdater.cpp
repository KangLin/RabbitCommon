// Copyright Copyright (c) Kang Lin studio, All Rights Reserved
// Author Kang Lin <kl222@126.com>

#include <QUrl>
#include <QStandardPaths>
#include <QFinalState>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDomDocument>
#include <QDomText>
#include <QDomElement>
#include <QProcess>
#include <QDir>
#include <QSsl>
#include <QDesktopServices>
#include <QInputDialog>
#include <QMessageBox>
#include <QMenu>
#include <QSettings>
#include <QLoggingCategory>
#include <QRegularExpression>
#include <QStateMachine>

#include "Download.h"
#include "FrmUpdater.h"
#include "RabbitCommonDir.h"
#include "RabbitCommonTools.h"
#include "ui_FrmUpdater.h"

static Q_LOGGING_CATEGORY(log, "RabbitCommon.Updater")

CFrmUpdater::CFrmUpdater(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CFrmUpdater),
    m_InstallAutoStartupType(false),
    m_ButtonGroup(this),
    m_bDownload(false),
    m_StateMachine(nullptr),
    m_pStateDownloadSetupFile(nullptr),
    m_pcbUpdate(nullptr)
{
    bool check = false;
    setAttribute(Qt::WA_DeleteOnClose);
    ui->setupUi(this);
    ui->lbNewArch->hide();
    ui->lbNewVersion->hide();
    ui->progressBar->hide();
    ui->cbHomePage->hide();
    ui->pbOK->hide();

    QSettings set(RabbitCommon::CDir::Instance()->GetFileUserConfigure(),
                  QSettings::IniFormat);
    ui->cbPrompt->setChecked(set.value("Updater/Prompt", false).toBool());
    ui->cbHomePage->setChecked(set.value("Updater/ShowHomePage", true).toBool());

    check = connect(&m_TrayIcon,
                    SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
                    this,
                    SLOT(slotShowWindow(QSystemTrayIcon::ActivationReason)));
    Q_ASSERT(check);
    m_TrayIcon.setIcon(this->windowIcon());
    m_TrayIcon.setToolTip(windowTitle() + " - "
                          + qApp->applicationDisplayName());

    int id = set.value("Update/RadioButton", -2).toInt();
    m_ButtonGroup.addButton(ui->rbEveryTime);
    m_ButtonGroup.addButton(ui->rbEveryDate);
    m_ButtonGroup.addButton(ui->rbEveryWeek);
    m_ButtonGroup.addButton(ui->rbEveryMonth);
    m_ButtonGroup.button(id)->setChecked(true);
    check = connect(&m_ButtonGroup,
                #if QT_VERSION >= QT_VERSION_CHECK(5, 15, 0)
                    SIGNAL(idClicked(int)),
                #else
                    SIGNAL(buttonClicked(int)),
                #endif
                    this, SLOT(slotButtonClickd(int)));
    Q_ASSERT(check);
    SetTitle();

    ui->lbCurrentArch->setText(tr("Current archecture: %1")
                 .arg(QSysInfo::currentCpuArchitecture()));

    QString szVerion = qApp->applicationVersion();
#ifdef RabbitCommon_VERSION
    if(szVerion.isEmpty())
        szVerion = RabbitCommon_VERSION;
#else
    szVerion = "0.0.1";
#endif
    SetVersion(szVerion);

    if(QSslSocket::supportsSsl())
    {
        QString szMsg;
#if (QT_VERSION >= QT_VERSION_CHECK(5, 4, 3))
        szMsg = "Build Version: " + QSslSocket::sslLibraryBuildVersionString();
#endif
        szMsg += "; Installed Version: " + QSslSocket::sslLibraryVersionString();
        qInfo(log) << "QSslSocket support ssl:" << szMsg;
    } else {
        QString szMsg;
#if (QT_VERSION >= QT_VERSION_CHECK(5, 4, 3))
        szMsg = "BuildVersion: " + QSslSocket::sslLibraryBuildVersionString();
#endif
        qCritical(log) <<
              "QSslSocket is not support ssl. The system is not install the OPENSSL dynamic library[" << szMsg << "]."
              " Please install OPENSSL dynamic library [" << szMsg << "]";
    }
}

CFrmUpdater::CFrmUpdater(QVector<QUrl> urls, QWidget *parent): CFrmUpdater(parent)
{
    if(urls.isEmpty())
    {
        // [Redirect configure file default urls]
        QUrl github("https://github.com/KangLin/"
                + qApp->applicationName() + "/raw/master/Update/update.json");
        QUrl gitlab("https://gitlab.com/kl222/"
                + qApp->applicationName() + "/-/raw/master/Update/update.json");
        QUrl gitee("https://gitee.com/kl222/"
                + qApp->applicationName() + "/raw/master/Update/update.json");
        QUrl sourceforge("https://sourceforge.net/p/"
                  + qApp->applicationName() + "/ci/master/tree/Update/update.json?format=raw");
        // [Redirect configure file default urls]
        m_Urls << github << gitee << sourceforge << gitlab;
    } else {
        m_Urls = urls;
    }
    qDebug(log) << "Urls:" << m_Urls;
    InitStateMachine();
}

CFrmUpdater::~CFrmUpdater()
{
    qDebug(log) << "CFrmUpdater::~CFrmUpdater()";
    m_DownloadFile.close();
    if(m_Download)
        m_Download.reset();
    if(m_StateMachine)
        m_StateMachine->deleteLater();
    delete ui;
}

/*!
 * \~chinese 初始化状态机
 * \~english
 * \brief Initialization state machine
 *
 * \~
 * \details
 * \code
 *
 *               ________
 * start o ----->|sCheck|----------------------------------|
 *               --------                                  |
 *                  |                                      |
 *                  |  sigFinished                         |
 *                  |                                      |
 *                  V                                      |
 *   -----------------------------------|                  |sigError
 *   |             s                    |                  |
 *   |----------------------------------|                  |
 *   |                                  |                  |
 *   |  |----------------------|        |                  |
 *   |  |Download config file  |        |                  |
 *   |  |(sDownloadConfigFile) |<---|   |                  |
 *   |  |----------------------|    |   |                  |
 *   |      |                       |   |                  |
 *   |      | sigFinished           |   |                  |
 *   |      |                       |   |                  |
 *   |      V   sigDownLoadRedire   |   |                  |
 *   |  |--------------------|      |   |                  |
 *   |  |Check config file   |      |   |                  |
 *   |  |(sCheckConfigFile)  |------|   |                  |
 *   |  |____________________|          |                  |
 *   |      |                           |                  |
 *   |      | sigFinished               |                  |
 *   |      | pbOk->clicked             |   sigFinished    V
 *   |      V                           |   sigError   |------|
 *   |  |--------------------|          |------------->|sFinal|
 *   |  |Download setup file |          |              |------|
 *   |  |(sDownloadSetupFile)|          |
 *   |  |--------------------|          |
 *   |      |                           |
 *   |      | sigFinished               |
 *   |      V                           |
 *   |  |--------------------|          |
 *   |  |update(sUpdate)     |          |
 *   |  |--------------------|          |
 *   |----------------------------------|
 *
 *
 * \endcode
 */
int CFrmUpdater::InitStateMachine()
{
    qDebug(log) << "Init State Machine";
    QFinalState *sFinal = new QFinalState();
    QState *sCheck = new QState();
    QState *s = new QState();
    QState *sDownloadConfigFile = new QState(s);
    QState *sCheckConfigFile = new QState(s);
    QState *sDownloadSetupFile = new QState(s);
    QState *sUpdate = new QState(s);

    bool check = connect(sFinal, SIGNAL(entered()),
                         this, SLOT(slotStateFinished()));
    Q_ASSERT(check);

    sCheck->addTransition(this, SIGNAL(sigError()), sFinal);
    sCheck->addTransition(this, SIGNAL(sigFinished()), s);
    check = connect(sCheck, SIGNAL(entered()), this, SLOT(slotCheck()));
    Q_ASSERT(check);

    s->addTransition(this, SIGNAL(sigError()), sFinal);
    s->addTransition(this, SIGNAL(sigFinished()), sFinal);

    s->setInitialState(sDownloadConfigFile);
    sDownloadConfigFile->assignProperty(ui->lbState, "text", tr("Being Download config file"));
    sDownloadConfigFile->addTransition(this, SIGNAL(sigFinished()), sCheckConfigFile);
    check = connect(sDownloadConfigFile, SIGNAL(entered()),
                     this, SLOT(slotDownloadFile()));
    Q_ASSERT(check);

    sCheckConfigFile->addTransition(this, SIGNAL(sigDownLoadRedire()), sDownloadConfigFile);
    sCheckConfigFile->addTransition(this, SIGNAL(sigFinished()), sDownloadSetupFile);
    sCheckConfigFile->addTransition(ui->pbOK, SIGNAL(clicked()), sDownloadSetupFile);
    sCheckConfigFile->assignProperty(ui->pbOK, "text", tr("OK(&O)"));
    check = connect(sCheckConfigFile, SIGNAL(entered()),  this, SLOT(slotCheckConfigFile()));
    Q_ASSERT(check);

    m_pStateDownloadSetupFile = sDownloadSetupFile;
    sDownloadSetupFile->addTransition(this, SIGNAL(sigFinished()), sUpdate);
    sDownloadSetupFile->assignProperty(ui->lbState, "text", tr("Being download update file"));
    check = connect(sDownloadSetupFile, SIGNAL(entered()), this, SLOT(slotDownloadSetupFile()));
    Q_ASSERT(check);

    sUpdate->assignProperty(ui->lbState, "text", tr("Being install update"));
    check = connect(sUpdate, SIGNAL(entered()), this, SLOT(slotUpdate()));
    Q_ASSERT(check);

    if(!m_StateMachine)
        m_StateMachine = new QStateMachine(this);
    m_StateMachine->addState(sCheck);
    m_StateMachine->addState(s);
    m_StateMachine->addState(sFinal);
    m_StateMachine->setInitialState(sCheck);
    m_StateMachine->start();
    return 0;    
}

int CFrmUpdater::SetTitle(QImage icon, const QString &szTitle)
{
    QString title = szTitle;
    if(szTitle.isEmpty())
        title = qApp->applicationDisplayName();
    ui->lbTitle->setText(title);
    
    QPixmap pixmpa = QPixmap::fromImage(icon);
    if(pixmpa.isNull())
        pixmpa.load(":/icon/RabbitCommon/App", "PNG");
    ui->lbTitleIcon->setPixmap(pixmpa);
    return 0;
}

int CFrmUpdater::SetVersion(const QString &szVersion)
{
    m_szCurrentVersion = szVersion;
    ui->lbCurrentVersion->setText(tr("Current version: %1")
                                  .arg(m_szCurrentVersion));
    return 0;
}

void CFrmUpdater::slotStateFinished()
{
    qDebug(log) << "slotStateFinished()";
    if(m_Download)
        m_Download.reset();
}

void CFrmUpdater::slotCheck()
{
    qDebug(log) << "CFrmUpdater::slotCheck()";
    QSettings set(RabbitCommon::CDir::Instance()->GetFileUserConfigure(),
                  QSettings::IniFormat);
    QDateTime d = set.value("Update/DateTime").toDateTime();
    set.setValue("Update/DateTime", QDateTime::currentDateTime());
    if(m_bDownload)
    {
        emit sigFinished();
        return;
    }

    int n = 0;
    if(ui->rbEveryDate->isChecked())
        n = 1;
    else if(ui->rbEveryWeek->isChecked())
        n = 7;
    else if(ui->rbEveryMonth->isChecked())
        n = 30;

    if(n <= d.daysTo(QDateTime::currentDateTime()))
        emit sigFinished();
    else
        emit sigError();
}

// [Process the signals of RabbitCommon::CDownload]
void CFrmUpdater::slotDownloadError(int nErr, const QString szError)
{
    ui->progressBar->hide();
    ui->progressBar->setRange(0, 100);;
    QString szMsg;
    szMsg = tr("Failed:") + tr("Download file is Failed.");
    if(!szError.isEmpty())
        szMsg += "(" + szError + ")";
    ui->lbState->setText(szMsg);
    qCritical(log) << szMsg << nErr;
    emit sigError();
}

void CFrmUpdater::slotDownloadFileFinished(const QString szFile)
{
    qDebug(log) << "slotDownloadFileFinished:" << szFile;
    ui->progressBar->hide();
    ui->progressBar->setRange(0, 100);;
    if(m_DownloadFile.isOpen())
        m_DownloadFile.close();

    QString szTmp
            = QStandardPaths::writableLocation(QStandardPaths::TempLocation);
    szTmp = szTmp + QDir::separator() + "Rabbit"
            + QDir::separator() + qApp->applicationName();

    QString szFileName(m_ConfigFile.szFileName);
    if(szFileName.isEmpty())
    {
        szFileName = szFile.mid(szFile.lastIndexOf("/"));
    }
    if(szFileName.left(1) != "/" && szFileName.left(1) != "\\")
        szFileName = QDir::separator() + szFileName;
    QString f = szTmp + szFileName;
    if(QFile::exists(f))
        QFile::remove(f);
#if HAVE_TEST
    if(QFile::copy(szFile, f))
#else
    if(QFile::rename(szFile, f))
#endif
    {
        m_DownloadFile.setFileName(f);
        qInfo(log) << "Download finished: rename"
                           << szFile << "to" << f;
    } else {
        qCritical(log) << "Download finished. rename fail from"
                           << szFile << "to" << f;
        m_DownloadFile.setFileName(szFile);
    }
    emit sigFinished();
}

void CFrmUpdater::slotDownloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    if(ui->progressBar->isHidden())
    {
        ui->progressBar->show();
        ui->progressBar->setRange(0, static_cast<int>(bytesTotal));
    }
    if(ui->progressBar->maximum() != bytesTotal)
         ui->progressBar->setRange(0, static_cast<int>(bytesTotal));

    ui->progressBar->setValue(static_cast<int>(bytesReceived));
    if(bytesTotal > 0) {
        QString szInfo = tr("Downloading %1% [%2/%3]")
                              .arg(QString::number(bytesReceived * 100 / bytesTotal))
                              .arg(QString::number(bytesReceived)).arg(QString::number(bytesTotal));
        //qDebug(log) << szInfo;
        m_TrayIcon.setToolTip(windowTitle() + " - "
                          + qApp->applicationDisplayName()
                          + ": " + szInfo);
    }
}
// [Process the signals of RabbitCommon::CDownload]

void CFrmUpdater::slotDownloadFile()
{
    qDebug(log) << "CFrmUpdater::slotDownloadFile";
    // [Use RabbitCommon::CDownload download file]
    if(!m_Urls.isEmpty())
    {
        m_Download = QSharedPointer<RabbitCommon::CDownload>(
            new RabbitCommon::CDownload(), &QObject::deleteLater);
        bool check = connect(m_Download.data(), SIGNAL(sigFinished(const QString)),
                this, SLOT(slotDownloadFileFinished(const QString)));
        Q_ASSERT(check);
        check = connect(m_Download.data(), SIGNAL(sigError(int, const QString)),
                        this, SLOT(slotDownloadError(int, const QString)));
        Q_ASSERT(check);
        check = connect(m_Download.data(), SIGNAL(sigDownloadProgress(qint64, qint64)),
                        this, SLOT(slotDownloadProgress(qint64, qint64)));
        Q_ASSERT(check);
        m_Download->Start(m_Urls);
    }
    // [Use RabbitCommon::CDownload download file]
}

void CFrmUpdater::slotCheckConfigFile()
{
    m_TrayIcon.setToolTip(windowTitle() + " - "
                          + qApp->applicationDisplayName());
    qDebug(log) << "CFrmUpdater::slotCheckConfigFile()";

    // Redirect
    int nRet = 0;
    nRet = CheckRedirectConfigFile();
    if(nRet <= 0) return;
    if(2 == nRet)
    {
        QString szText(tr("There is laster version"));
        ui->lbState->setText(szText);
        qInfo(log) << szText;
        emit sigError();
        return;
    }
    if(1 == nRet)
        CheckUpdateConfigFile();
}

/*!
 * \brief 检查重定向配置文件
 * \return
 *         = 2: Don't updater \n
 *         = 1: Is normal configure file \n
 *         = 0: Is redirect configure file \n
 *         < 0: Error
 *
 * \details
 * 重定向配置文件格式：\n
 * json format: \n
 * \include Test/data/redirect.json
 *
 */
int CFrmUpdater::CheckRedirectConfigFile()
{
    int nRet = 0;
    qDebug(log) << "CFrmUpdater::CheckRedirectConfigFile()"
                << m_DownloadFile.fileName();

    QVector<CONFIG_REDIRECT> conf;
    nRet = GetRedirectFromFile(m_DownloadFile.fileName(), conf);
    if(nRet) {
        if(nRet < 0) {
            QString szError = tr("Failed:") + tr("%2 process the file: %1")
                                      .arg(m_DownloadFile.fileName()).arg(nRet);
            ui->lbState->setText(szError);
            qCritical(log) << szError;
            emit sigError();
        }
        return nRet;
    }

    CONFIG_REDIRECT redirect;
    for(auto it = conf.begin(); it != conf.end(); it++) {

        QString szVersion = it->szVersion;
        if(szVersion.isEmpty())
        {
            QString szError = tr("Failed:") + tr("Configure file content error:")
                              + m_DownloadFile.fileName();
            ui->lbState->setText(szError);
            qCritical(log) << szError;
            emit sigError();
            return -2;
        }

        if(CompareVersion(szVersion, m_szCurrentVersion) <= 0)
            continue;

        QString szMinVersion = it->szMinUpdateVersion;
        if(!szMinVersion.isEmpty()) {
            if(CompareVersion(szMinVersion, m_szCurrentVersion) > 0)
                continue;
        }

        redirect = *it;
        break;
    }

    if(redirect.szVersion.isEmpty())
        return 2;

    CONFIG_FILE file;
    foreach (auto f, redirect.files) {
        if(!f.szSystem.isEmpty()) {
           if(QSysInfo::productType() != f.szSystem)
                continue;
        }
        if(!f.szArchitecture.isEmpty())
        {    
           QString szArchitecture = QSysInfo::currentCpuArchitecture();
#if defined(Q_OS_WIN) ||  defined(Q_OS_LINUX)
           if(!szArchitecture.compare("i386", Qt::CaseInsensitive)
               && !f.szArchitecture.compare("x86_64", Qt::CaseInsensitive))
                continue;
#else
           if(szArchitecture != f.szArchitecture)
                continue;
#endif
        }
        file = f;
        break;
    }

    m_Urls = file.urls;
    if(m_Urls.isEmpty())
    {
        if(redirect.files.isEmpty()) {
            // [Update configure file default urls]
            QUrl github("https://github.com/KangLin/"
                        + qApp->applicationName() + "/releases/download/"
                        + redirect.szVersion + "/update.json");
            m_Urls.push_back(github);
            // https://master.dl.sourceforge.net/project/rabbitremotecontrol/v0.0.36/update.json?viasf=1
            QUrl sourceforge("https://master.dl.sourceforge.net/project/"
                             + qApp->applicationName() +"/"
                             + redirect.szVersion + "/update.json?viasf=1");
            m_Urls.push_back(sourceforge);
            // [Update configure file default urls]
        } else {
            QString szError;
            szError =  tr("Failed:")
                      + tr("Don't find the urls in configure file:")
                      + m_DownloadFile.fileName()
                      + "; " + tr("Current version:") + m_szCurrentVersion
                      + "; " + tr("version:") + redirect.szVersion
                      + "; " + tr("min update version:")
                      + redirect.szMinUpdateVersion;
            qCritical(log) << szError;
            ui->lbState->setText(szError);
            return -3;
        }
    }

    qInfo(log) << "Redirect. Version:" << redirect.szVersion << m_Urls;

    emit sigDownLoadRedire();

    return 0;
}

/*!
 * \brief 检查更新配置文件
 * \details
 * 
 * 
 * 旧的 xml 格式：
 * \code
 
   <?xml version="1.0" encoding="UTF-8"?>
   <UPDATE>
    <VERSION>v0.0.1</VERSION>
    <TIME>2019-2-24 19:28:50</TIME>
    <INFO>v0.0.1 information</INFO>
    <HOME>home page</HOME>
    <MIN_UPDATE_VERSION>%MIN_UPDATE_VERSION%</MIN_UPDATE_VERSION>
    <FORCE>0</FORCE>
    
    <SYSTEM>windows</SYSTEM>         <!--windows, linux, android-->
    <PLATFORM>x86</PLATFORM>         <!--windows, linux, android-->
    <ARCHITECTURE>x86</ARCHITECTURE> <!--x86, x86_64, armeabi-v7a arm64_v8a-->
    <FILENAME>File name</FILENAME>
    <URL>url1</URL>
    <URL>url2</URL>
    <URL>...</URL>
    <MD5SUM>%RABBITIM_MD5SUM%</MD5SUM>
    
   </UPDATE>
   
 * \endcode
 * 
 * \see GetConfigFromFile
 */
int CFrmUpdater::CheckUpdateConfigFile()
{
    int nRet = 0;
    qDebug(log) << "CFrmUpdater::CheckUpdateConfigFile()";
    CONFIG_INFO info;
    nRet = GetConfigFromFile(m_DownloadFile.fileName(), info);
    if(nRet) {
        QString szError = tr("Failed:") + tr("%2 process the file: %1")
                                      .arg(m_DownloadFile.fileName()).arg(nRet);
        ui->lbState->setText(szError);
        qCritical(log) << szError;
        emit sigError();
        return nRet;
    }

    if(CompareVersion(info.version.szVerion, m_szCurrentVersion) <= 0)
    {
        QString szError;
        szError = tr("There is laster version");
        ui->lbState->setText(szError);
        qInfo(log) << szError;
        emit sigError();
        return -4;
    }
    
    if(info.files.isEmpty()) {
        QString szError;
        szError = tr("Failed:") + tr("There is not files in the configure file ")
            + m_DownloadFile.fileName();
        ui->lbState->setText(szError);
        qCritical(log) << szError;
        emit sigError();
        return -5;
    }

    QString szSystem = QSysInfo::productType();
#if defined(Q_OS_LINUX)
    QString szAppImage = QString::fromLocal8Bit(qgetenv("APPIMAGE"));
    if(!szAppImage.isEmpty())
        szSystem = "AppImage";
#endif
    QString szArchitecture = QSysInfo::currentCpuArchitecture();
    CONFIG_FILE file;
    foreach (auto f, info.files) {
        if(f.szSystem.compare(szSystem, Qt::CaseInsensitive))
            continue;

        if(szArchitecture != f.szArchitecture) {
#if defined(Q_OS_WIN) || defined(Q_OS_LINUX) || defined(Q_OS_UNIX)
            if(!szArchitecture.compare("x86_64", Qt::CaseInsensitive)
                && !f.szArchitecture.compare("i386", Qt::CaseInsensitive))
            {
                ;
            } else
                continue;
#else
            continue;
#endif
        }

        file = f;
        break;
    }

    if(file.szSystem.compare(szSystem, Qt::CaseInsensitive)) {
        QString szErr;
        szErr = tr("Failed:")
              + tr("The system or architecture is not exist in the configure file ")
              + m_DownloadFile.fileName();
        ui->lbState->setText(szErr);
        qCritical(log) << szErr;
        emit sigError();
        return -6;
    }

    m_Info.version = info.version;
    m_ConfigFile = file;

    ui->lbNewVersion->setText(tr("New version: %1").arg(info.version.szVerion));
    ui->lbNewVersion->show();
    ui->lbNewArch->setText(tr("New architecture: %1").arg(file.szArchitecture));
    ui->lbNewArch->show();
    ui->lbState->setText(tr("There is a new version, is it updated?"));
    if(info.version.bForce)
    {
        qDebug(log) << "Force update";
        emit sigFinished();
    }
    else
    {
        ui->cbHomePage->show();
        ui->cbPrompt->show();
        ui->pbOK->setText(tr("OK(&O)"));
        ui->pbOK->show();
        if(!CheckPrompt(info.version.szVerion) && this->isHidden())
            emit sigError();
        else
            show();
    }

    return nRet;
}

/*!
 * \brief Get redirect configure from file
 * \param szFile: File name
 * \param conf: Redirect configure
 * \return > 0: Is normal configure file \n
 *         = 0: Is redirect configure file \n
 *         < 0: Error
 * json format:
 * \include Test/data/redirect.json
 * \see CheckRedirectConfigFile
 */
int CFrmUpdater::GetRedirectFromFile(const QString& szFile, QVector<CONFIG_REDIRECT> &conf)
{
    QFile f(szFile);
    if(!f.open(QFile::ReadOnly))
    {
        QString szError = tr("Open file fail").arg(szFile);
        qCritical(log) << szError;
        return -1;
    }

    QJsonDocument doc;
    doc = QJsonDocument::fromJson(f.readAll());
    f.close();
    if(!doc.isObject())
    {
        QString szError = tr("Parse file %1 fail. It isn't configure file")
                              .arg(f.fileName());
        qCritical(log) << szError;
        return -2;
    }

    QJsonObject objRoot = doc.object();
    if(!objRoot.contains("redirect"))
        return 1;

    QJsonArray arrRedirect = objRoot["redirect"].toArray();
    for(auto it = arrRedirect.begin(); it != arrRedirect.end(); it++) {
        QJsonObject obj = it->toObject();
        CONFIG_REDIRECT objRedirect;
        objRedirect.szVersion = obj["version"].toString();
        objRedirect.szMinUpdateVersion = obj["min_update_version"].toString();
        qDebug(log) << "version:" << objRedirect.szVersion
                           << "min_update_version:" << objRedirect.szMinUpdateVersion;

        QJsonArray objFiles = obj["files"].toArray();
        for(auto it = objFiles.begin(); it != objFiles.end(); it++) {
            QJsonObject f = it->toObject();
            CONFIG_FILE file;
            file.szSystem = f["os"].toString();
            file.szSystemMinVersion = f["os_min_version"].toString();
            file.szArchitecture = f["arch"].toString();
            file.szArchitectureMinVersion = f["arch_min_version"].toString();
            file.szMd5sum = f["md5"].toString();
            file.szFileName = f["name"].toString();

            QJsonArray urls = f["urls"].toArray();
            foreach(auto u, urls)
            {
                file.urls.append(u.toString());
            }

            objRedirect.files.append(file);
            //*
            qDebug(log) << "OS:" << file.szSystem
                               << "os_min_version:" << file.szSystemMinVersion
                               << "arch:" << file.szArchitecture
                               << "arch_min_version:" << file.szArchitectureMinVersion
                               << "md5:" << file.szMd5sum
                               << "name:" << file.szFileName
                               << "urls:" << file.urls;//*/
        }

        conf.append(objRedirect);
    }

    return 0;
}

/*!
 * json 格式：
 * 
 * |   os  |         architecture       |platform    |
 * |:-----:|:--------------------------:|:----------:|
 * |windows|        "i386",x86_64       |mingw,cygwin|
 * |linux  |        x86,x86_64          |            |
 * |android|armv7, arm64_v8a, x86,x86_64|            |
 * |macos  |        x86,x86_64          |            |
 
 * \see: [static, since 5.4] QString QSysInfo::currentCpuArchitecture()
 *
 * \code
 * {
 *   "version": {
 *     "version": "2.0.0",
 *     "min_update_version": "1.0.0",
 *     "time": "",
 *     "information": "RabbitCommon",
 *     "home": "https://github.com/kanglin/rabbitcommon",
 *     "force": false
 *   },
 *   "files": [
 *     {
 *       "os": "windows",
 *       "os_min_version": "7",
 *       "arch": "x86",
 *       "arch_min_version": "1",
 *       "name": "RabbitCommon_setup.exe",
 *       "md5": "",
 *       "urls": [
 *         "https://github.com/kanglin/rabbitcommon/windows",
 *         "https://gitlab.com/kl222/rabbitcommon/windows"
 *       ]
 *     },
 *     {
 *       "os": "ubuntu",
 *       "os_min_version": "22.06",
 *       "arch": "x86",
 *       "arch_min_version": "1",
 *       "name": "rabbitcommon_setup.deb",
 *       "md5": "",
 *       "urls": [
 *         "https://github.com/kanglin/rabbitcommon/ubuntu",
 *         "https://gitlab.com/kl222/rabbitcommon/ubuntu"
 *       ]
 *     }
 *   ]
 * }
 * \endcode
 * \see GenerateJsonFile GetConfigFromCommandLine
 */
int CFrmUpdater::GetConfigFromFile(const QString &szFile, CONFIG_INFO& conf)
{
    QFile file(szFile);
    if(!file.open(QFile::ReadOnly)) {
        qDebug(log) << "The file isn't opened:" << szFile;
        return -1;
    }

    QJsonDocument doc;
    doc = QJsonDocument::fromJson(file.readAll());
    file.close();
    if(!doc.isObject())
    {
        qCritical(log) << "Parser configure file fail." << szFile;
        return -2;
    }
    
    QJsonObject obj = doc.object();
    if(obj.contains("version")) {
        QJsonObject objVersion = obj["version"].toObject();
        conf.version.szVerion = objVersion["version"].toString();
        conf.version.szMinUpdateVersion = objVersion["min_update_version"].toString();
        conf.version.szTime = objVersion["time"].toString();
        conf.version.szInfomation = objVersion["information"].toString();
        conf.version.szHome = objVersion["home"].toString();
        conf.version.bForce = objVersion["force"].toBool();
        //*
        qDebug(log) << "Current version:" << m_szCurrentVersion
                           << "version:" << conf.version.szVerion
                           << "minUpdateVersion:" << conf.version.szMinUpdateVersion
                           << "time:" << conf.version.szTime
                           << "information:" << conf.version.szInfomation
                           << "home:" << conf.version.szHome
                           << "bForce:" << conf.version.bForce
                           ;//*/
    }
    
    if(!obj.contains("files")) {
        qDebug(log) << "Configure file isn't contains files array";
        return 0;
    }
    
    QJsonArray objFiles = obj["files"].toArray();
    for(auto it = objFiles.begin(); it != objFiles.end(); it++) {
        QJsonObject f = it->toObject();
        CONFIG_FILE file;
        file.szSystem = f["os"].toString();
        file.szSystemMinVersion = f["os_min_version"].toString();
        file.szArchitecture = f["arch"].toString();
        file.szArchitectureMinVersion = f["arch_min_version"].toString();
        file.szMd5sum = f["md5"].toString();
        file.szFileName = f["name"].toString();
        
        QJsonArray urls = f["urls"].toArray();
        foreach(auto u, urls)
        {
            file.urls.append(u.toString());
        }

        conf.files.append(file);
        //*
        qDebug(log) << "OS:" << file.szSystem
                           << "os_min_version:" << file.szSystemMinVersion
                           << "arch:" << file.szArchitecture
                           << "arch_min_version:" << file.szArchitectureMinVersion
                           << "md5:" << file.szMd5sum
                           << "name:" << file.szFileName
                           << "urls:" << file.urls;//*/
    }

    return 0;
}

void CFrmUpdater::slotDownloadSetupFile()
{
    qDebug(log) << "CFrmUpdater::slotDownloadSetupFile()";
    ui->pbOK->setText(tr("Hide"));
    ui->lbState->setText(tr("Download ......"));
    if(IsDownLoad())
        emit sigFinished();
    else
    {
        m_Urls = m_ConfigFile.urls;
        slotDownloadFile();
    }
}

void CFrmUpdater::slotUpdate()
{
    m_TrayIcon.setToolTip(windowTitle() + " - "
                          + qApp->applicationDisplayName());
    ui->lbState->setText(tr("Being install update ......"));
    ui->progressBar->hide();
    ui->progressBar->setRange(0, 100);;
    //qDebug(log) << "CFrmUpdater::slotUpdate()";

    // Check file md5sum
    bool bSuccess = false;
    do {
        if(!m_DownloadFile.open(QIODevice::ReadOnly))
        {
            QString szErr;
            szErr = tr("Failed:") + tr("Don't open download file ")
                + m_DownloadFile.fileName();
            qCritical(log) << szErr;
            ui->lbState->setText(szErr);
            break;
        }
        QCryptographicHash md5sum(QCryptographicHash::Md5);
        if(!md5sum.addData(&m_DownloadFile))
        {
            QString szErr;
            szErr = tr("Failed:") + tr("Don't open download file ")
                    + m_DownloadFile.fileName();
            qCritical(log) << szErr;
            ui->lbState->setText(szErr);
            break;
        }
        if(md5sum.result().toHex() != m_ConfigFile.szMd5sum)
        {
            QString szFail;
            szFail = tr("Failed:") + tr("Md5sum is different.")
                    + "\n" + tr("Download file md5sum: ")
                    + md5sum.result().toHex()
                    + "\n" + tr("md5sum in Update configure file: ")
                    + m_ConfigFile.szMd5sum;
            ui->lbState->setText(szFail);
            qCritical(log) << szFail;
            break;
        }
        bSuccess = true;
    } while(0);

    m_DownloadFile.close();
    if(!bSuccess)
    {
        emit sigError();
        return;
    }
    
    // Exec download file
    bSuccess = false;
    do {
        
        if(m_pcbUpdate) {
            int nRet = m_pcbUpdate(m_DownloadFile.fileName());
            if(0 == nRet) {
                bSuccess = true;
                break;
            }
        }
           
        //修改文件执行权限  
        /*QFileInfo info(m_szDownLoadFile);
        if(!info.permission(QFile::ExeUser))
        {
            //修改文件执行权限  
            QString szErr = tr("Download file don't execute permissions. Please modify permission then manually  execute it.\n%1").arg(m_szDownLoadFile);
            slotError(-2, szErr);
            return;
        }*/

        QProcess proc;
        QFileInfo fi(m_DownloadFile.fileName());
        if(!fi.suffix().compare("AppImage", Qt::CaseInsensitive)) {

            QString szAppImage = QString::fromLocal8Bit(qgetenv("APPIMAGE"));
            bool bRet = false;
            if(!szAppImage.isEmpty()) {
                QFile f(fi.filePath());
                QFileInfo cf(szAppImage);
                QString szExec;
                szExec = cf.absoluteDir().absolutePath()
                         + QDir::separator() + fi.fileName();
                bRet = f.copy(szExec);
                if(bRet) {
                    QString szMsg(tr("Please exec:") + szExec);
                    ui->lbState->setText(szMsg);
                    qInfo(log) << szMsg;
                    QUrl url = QUrl::fromLocalFile(cf.absoluteDir().absolutePath());
                    if(!QDesktopServices::openUrl(url))
                    {
                        QString szErr;
                        szErr = tr("Failed:") + tr("Open the folder fail:")
                                + cf.absoluteDir().absolutePath();
                        qCritical(log) << szErr;
                    }
                }
            }
            if(!bRet) {
                QString szMsg(tr("Please exec: ") + fi.absoluteFilePath());
                ui->lbState->setText(szMsg);
                qInfo(log) << szMsg;
                QUrl url = QUrl::fromLocalFile(fi.absoluteDir().absolutePath());
                if(!QDesktopServices::openUrl(url))
                {
                    QString szErr;
                    szErr = tr("Failed:") + tr("Open the folder fail:")
                            + fi.absoluteDir().absolutePath();
                    qCritical(log) << szErr;
                }
            }

        } else if(fi.suffix().compare("gz", Qt::CaseInsensitive)) {
            QString szCmd;
            szCmd = m_DownloadFile.fileName();
            //启动安装程序
            qInfo(log) << "Start"
                              << szCmd
                              << "in a new process, and detaches from it.";
            if(!proc.startDetached(szCmd))
            {
                qInfo(log) << "Start new process fail."
                                  << "Use system installer to install"
                                  << m_DownloadFile.fileName();
                QUrl url(m_DownloadFile.fileName());
                if(!QDesktopServices::openUrl(url))
                {
                    QString szErr = tr("Failed:")
                                    + tr("Execute install program error.%1")
                            .arg(m_DownloadFile.fileName());
                    ui->lbState->setText(szErr);
                    break;
                }
            }
        } else {
            QString szInstall = fi.absolutePath() + QDir::separator() + "setup.sh";
            QFile f(szInstall);
            if(!f.open(QFile::WriteOnly))
            {
                QString szErr = tr("Failed:")
                    + tr("Open file %1 fail").arg(fi.absolutePath());
                ui->lbState->setText(szErr);
                break;
            }
            QString szCmd = InstallScript(m_DownloadFile.fileName(),
                                          qApp->applicationName());
            f.write(szCmd.toStdString().c_str());
            qDebug(log) << szCmd << szInstall;
            f.close();

            //启动安装程序
            if(!RabbitCommon::CTools::executeByRoot("/bin/bash",
                                                    QStringList() << szInstall))
            {
                QString szErr = tr("Failed:") + tr("Execute") + "/bin/bash "
                                + szInstall + "fail";
                ui->lbState->setText(szErr);
                break;
            }

            //启动程序
//            int nRet = QMessageBox::information(this, tr("Run"),
//                                        tr("Run after install"),
//                              QMessageBox::Yes|QMessageBox::No,
//                                             QMessageBox::Yes);
//            if(QMessageBox::No == nRet)
//                break;
//            QString szProgram = "/opt/"
//                    + qApp->applicationName()
//                    + "/install1.sh start "
//                    + qApp->applicationName();
//            QProcess exe;
//            if(!exe.startDetached(szProgram))
//            {
//                QString szErr = tr("Failed:") + tr("Execute program error.%1")
//                        .arg(szProgram);
//                ui->lbState->setText(szErr);
//                break;
//            }
        }

        ui->lbState->setText(tr("The installer has started, Please close the application"));

        //system(m_DownloadFile.fileName().toStdString().c_str());
        //int nRet = QProcess::execute(m_DownloadFile.fileName());
        //qDebug(log) << "QProcess::execute return: " << nRet;
        
        bSuccess = true;
    } while(0);

    QProcess procHome;
    QString szHome = m_Info.version.szHome;
    if((!bSuccess || ui->cbHomePage->isChecked()) && !szHome.isEmpty())
        if(!procHome.startDetached(szHome))
        {
            QUrl url(szHome);
            if(!QDesktopServices::openUrl(url))
            {
                QString szErr = tr("Failed:") + tr("Open home page fail");
                ui->lbState->setText(szErr);
            }
        }

    if(bSuccess)
    {
        emit sigFinished();
        qApp->quit();
        return;
    }
    
    emit sigError();
    QUrl url(szHome);
    if(!QDesktopServices::openUrl(url))
    {
        QString szErr = tr("Open home page fail");
        qCritical(log) << szErr;
    }
}

QString CFrmUpdater::InstallScript(const QString szDownLoadFile,
                                   const QString szApplicationName)
{
    QFileInfo fi(szDownLoadFile);
    QString szCmd;
    szCmd = "#!/bin/bash\n";
    szCmd += "set -e\n";
    szCmd += "if [ ! -d /opt/" + szApplicationName + " ]; then\n";
    szCmd += "    mkdir -p /opt/" + szApplicationName + "\n";
    szCmd += "fi\n";
    szCmd += "cd /opt/" + szApplicationName + "\n";
    szCmd += "if [ -f install1.sh ]; then\n";
    szCmd += "    ./install1.sh remove " + szApplicationName + "\n";
    //szCmd += "    rm -fr *\n";
    szCmd += "fi\n";
    szCmd += "cp " + szDownLoadFile + " ." + "\n";
    szCmd += "tar xvfz " + fi.fileName() + "\n";
    szCmd += "rm " + fi.fileName() + "\n";
    
    //See: Install/install.sh
    szCmd += "./install1.sh ";
    if(m_InstallAutoStartupType)
        szCmd += "install_autostart";
    else
        szCmd += "install";
    //启动程序
    int nRet = QMessageBox::information(this, tr("Run"),
                                        tr("Run after install"),
                                        QMessageBox::Yes|QMessageBox::No,
                                        QMessageBox::Yes);
    if(QMessageBox::Yes == nRet)
    {
        szCmd += "_run";
    }
    szCmd += " " + szApplicationName + "\n";
    return szCmd;
}

/*!
 * \brief CFrmUpdater::CompareVersion
 * \param newVersion
 * \param currentVersion
 * \return > 0:
 *         = 0: 相同
 *         < 0
 * \see https://semver.org/lang/zh-CN/
 */
int CFrmUpdater::CompareVersion(const QString &newVersion, const QString &currentVersion)
{
    int nRet = 0;
    QString sN = newVersion;
    QString sC = currentVersion;
    
    if(sN.isEmpty() || sC.isEmpty())
        return sN.length() - sC.length();

    sN = sN.replace("-", ".");
    sC = sC.replace("-", ".");

    QStringList szNew = sN.split(".");
    QStringList szCur = sC.split(".");

    int count = qMin(szNew.length(), szCur.length());
    qDebug(log) << "count:" << count;

    if(count <= 1)
        return szNew.length() - szCur.length();

    QString firstNew = szNew.at(0);
    QString firstCur = szCur.at(0);
    firstNew = firstNew.remove(QChar('v'), Qt::CaseInsensitive);
    firstCur = firstCur.remove(QChar('v'), Qt::CaseInsensitive);
    nRet = firstNew.toInt() - firstCur.toInt();
    if(nRet)
        return nRet;

    if(count < 2)
        return sN.length() - sC.length();
    nRet = szNew.at(1).toInt() - szCur.at(1).toInt();
    if(nRet)
        return nRet;

    if(count < 3)
        return sN.length() - sC.length();
    return szNew.at(2).toInt() - szCur.at(2).toInt();
}

/*!
 * \brief Check file is exist
 * \return 
 */
bool CFrmUpdater::IsDownLoad()
{
    bool bRet = false;
    QString szTmp
            = QStandardPaths::writableLocation(QStandardPaths::TempLocation);
    szTmp = szTmp + QDir::separator() + "Rabbit"
            + QDir::separator() + qApp->applicationName();

    QString szFile = szTmp + QDir::separator() + m_ConfigFile.szFileName;

    QFile f(szFile);
    if(!f.open(QIODevice::ReadOnly))
        return false;

    m_DownloadFile.setFileName(szFile);
    do {
        QCryptographicHash md5sum(QCryptographicHash::Md5);
        if(!md5sum.addData(&f))
        {
            bRet = false;
            break;
        }
        if(md5sum.result().toHex() != m_ConfigFile.szMd5sum)
        {
            bRet = false;
            break;
        }
        else 
        {
            bRet = true;
            break;
        }
    } while(0);
    f.close();
    return bRet;
}

void CFrmUpdater::on_pbOK_clicked()
{
    qDebug(log) << "CFrmUpdater::on_pbOK_clicked()";
    if(!m_pStateDownloadSetupFile->active())
        return;
    
    m_TrayIcon.show();
    hide();
}

void CFrmUpdater::on_pbClose_clicked()
{
    if(m_StateMachine->isRunning())
    {
        QMessageBox::StandardButton ret = QMessageBox::warning(this, tr("Close"),
                        tr("Is updating, be sure to close?"), QMessageBox::Yes|QMessageBox::No);
        if(QMessageBox::No == ret)
        {
            return;
        }
    }
    emit sigError();
    close();
}

void CFrmUpdater::slotButtonClickd(int id)
{
    QSettings set(RabbitCommon::CDir::Instance()->GetFileUserConfigure(), QSettings::IniFormat);
    set.setValue("Update/RadioButton", id);
}

int CFrmUpdater::GenerateUpdateJson()
{
    QCommandLineParser parser;
    int nRet = GenerateUpdateJson(parser);
    parser.process(qApp->arguments());
    return nRet;
}

int CFrmUpdater::GenerateUpdateJson(QCommandLineParser &parser)
{
    QString szFile;
    CONFIG_INFO info;
    CONFIG_TYPE type;
    int nRet = GetConfigFromCommandLine(parser, szFile, info, type);
    if(nRet)
        return nRet;
    return GenerateJsonFile(szFile, info, type);
}

/*!
 * \brief Generate Json File
 * \param szFile
 * \param info
 * \param type
 * \return 
 * \see GetConfigFromFile
 */
int CFrmUpdater::GenerateJsonFile(const QString &szFile, const CONFIG_INFO &info, CONFIG_TYPE type)
{
    QJsonDocument doc;
    
    QJsonObject version;    
    version.insert("version", info.version.szVerion);
    version.insert("min_update_version", info.version.szMinUpdateVersion);
    version.insert("time", info.version.szTime);
    version.insert("information", info.version.szInfomation);
    version.insert("home", info.version.szHome);
    version.insert("force", info.version.bForce);
    
    QJsonArray files;
    foreach (auto f, info.files) {
        QJsonObject file;
        file.insert("os", f.szSystem);
        if(!f.szSystemMinVersion.isEmpty())
            file.insert("os_min_version", f.szSystemMinVersion);
        file.insert("arch", f.szArchitecture);
        if(!f.szArchitectureMinVersion.isEmpty())
            file.insert("arch_min_version", f.szArchitectureMinVersion);
        file.insert("md5", f.szMd5sum);
        file.insert("name", f.szFileName);
        QJsonArray urls;
        foreach (auto u, f.urls) {
            urls.append(u.toString());
        }
        file.insert("urls", urls);
        files.append(file);
    }
    
    switch(type) {
    case CONFIG_TYPE::VERSION:
        doc.setObject(version);
        break;
    case CONFIG_TYPE::FILE:
        doc.setObject(files[0].toObject());
        break;
    case CONFIG_TYPE::VERSION_FILE:
    {
        QJsonObject root;
        root.insert("version", version);
        root.insert("files", files);
        doc.setObject(root);
    }
    default:
        break;
    };

    QFile f(szFile);
    if(!f.open(QIODevice::WriteOnly))
    {
        qCritical(log) << "Open file fail:" << f.fileName();
        return -1;
    }
    f.write(doc.toJson());
    f.close();
    return 0;
}

int CFrmUpdater::GenerateUpdateXmlFile(const QString &szFile, const CONFIG_INFO &info, CONFIG_TYPE &type)
{
    QDomDocument doc;
    QDomProcessingInstruction ins;
    //<?xml version='1.0' encoding='UTF-8'?>
    ins = doc.createProcessingInstruction("xml", "version=\'1.0\' encoding=\'UTF-8\'");
    doc.appendChild(ins);
    QDomElement root = doc.createElement("UPDATE");
    doc.appendChild(root);
    
    QDomText version = doc.createTextNode("VERSION");
    version.setData(info.version.szVerion);
    QDomElement eVersion = doc.createElement("VERSION");
    eVersion.appendChild(version);
    root.appendChild(eVersion);
    
    QDomText time = doc.createTextNode("TIME");
    time.setData(info.version.szTime);
    QDomElement eTime = doc.createElement("TIME");
    eTime.appendChild(time);
    root.appendChild(eTime);

    QDomText i = doc.createTextNode("INFO");
    i.setData(info.version.szInfomation);
    QDomElement eInfo = doc.createElement("INFO");
    eInfo.appendChild(i);
    root.appendChild(eInfo);
    
    QDomText force = doc.createTextNode("FORCE");
    force.setData(QString::number(info.version.bForce));
    QDomElement eForce = doc.createElement("FORCE");
    eForce.appendChild(force);
    root.appendChild(eForce);
    
    CONFIG_FILE file = info.files[0];
    QDomText system = doc.createTextNode("SYSTEM");
    system.setData(file.szSystem);
    QDomElement eSystem = doc.createElement("SYSTEM");
    eSystem.appendChild(system);
    root.appendChild(eSystem);
   
    QDomText arch = doc.createTextNode("ARCHITECTURE");
    arch.setData(file.szArchitecture);
    QDomElement architecture = doc.createElement("ARCHITECTURE");
    architecture.appendChild(arch);
    root.appendChild(architecture);
    
    QDomText md5 = doc.createTextNode("MD5SUM");
    md5.setData(file.szMd5sum);
    QDomElement eMd5 = doc.createElement("MD5SUM");
    eMd5.appendChild(md5);
    root.appendChild(eMd5);
    
    QDomText fileName = doc.createTextNode("FILENAME");
    fileName.setData(file.szFileName);
    QDomElement eFileName = doc.createElement("FILENAME");
    eFileName.appendChild(fileName);
    root.appendChild(eFileName);
    
    foreach(auto u, file.urls)
    {
        QDomText url = doc.createTextNode("URL");
        url.setData(u.toString());
        QDomElement eUrl = doc.createElement("URL");
        eUrl.appendChild(url);
        root.appendChild(eUrl);
    }

    QDomText urlHome = doc.createTextNode("HOME");
    urlHome.setData(info.version.szHome);
    QDomElement eUrlHome = doc.createElement("HOME");
    eUrlHome.appendChild(urlHome);
    root.appendChild(eUrlHome);

    QDomText min = doc.createTextNode("MIN_UPDATE_VERSION");
    min.setData(info.version.szMinUpdateVersion);
    QDomElement eMin = doc.createElement("MIN_UPDATE_VERSION");
    eMin.appendChild(min);
    root.appendChild(eMin);

    QFile f;
    f.setFileName(szFile);
    if(!f.open(QIODevice::WriteOnly))
    {
        qCritical(log)
                << "CFrmUpdater::GenerateUpdateXml file open file fail:"
                << f.fileName();
        return -1;
    }
    QTextStream stream(&f);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    stream.setCodec("UTF-8");
#endif
    doc.save(stream, 4);
    f.close();
    return 0;
}

int CFrmUpdater::GenerateUpdateXml()
{
    QCommandLineParser parser;
    int nRet = GenerateUpdateXml(parser);
    parser.process(qApp->arguments());
    return nRet;
}

int CFrmUpdater::GenerateUpdateXml(QCommandLineParser &parser)
{
    QString szFile;
    CONFIG_INFO info;
    CONFIG_TYPE type;
    int nRet = GetConfigFromCommandLine(parser, szFile, info, type);
    if(nRet)
        return nRet;
    return GenerateUpdateXmlFile(szFile + ".xml", info, type);
}

/*!
 * \brief Get configure from command-line
 * \param parser
 * \param szFile
 * \param info
 * \param type
 * \return 
 * \see GetConfigFromFile
 */
int CFrmUpdater::GetConfigFromCommandLine(/*[in]*/QCommandLineParser &parser,
                         /*[out]*/QString &szFile,
                         /*[out]*/CONFIG_INFO &info,
                         /*[out]*/CONFIG_TYPE &type)
{
    QString szSystem = QSysInfo::productType();
#if defined(Q_OS_LINUX)
    QString szAppImage = QString::fromLocal8Bit(qgetenv("APPIMAGE"));
    if(!szAppImage.isEmpty())
        szSystem = "AppImage";
#endif

    QString szFileName;
#if defined (Q_OS_WIN)
    szFileName = qApp->applicationName() + "_" + m_szCurrentVersion + "_Setup" + ".exe";
#elif defined(Q_OS_ANDROID)
    szFileName = qApp->applicationName().toLower() + "_" + m_szCurrentVersion + ".apk";
#elif defined(Q_OS_LINUX)
    QFileInfo f;
    if(szAppImage.isEmpty())
        f = QFileInfo(qApp->applicationFilePath());
    else
        f = QFileInfo(szAppImage);
    if(f.suffix().compare("AppImage", Qt::CaseInsensitive))
    {
        QString szVersion = m_szCurrentVersion;
        szVersion.replace("v", "", Qt::CaseInsensitive);
        szFileName = qApp->applicationName().toLower()
                + "_" + szVersion + "_amd64.deb";
    } else {
        szFileName = qApp->applicationName()
                + "_" + m_szCurrentVersion + ".AppImage";
    }
#endif

    QString szUrl;          
    szUrl = "https://github.com/KangLin/"
            + qApp->applicationName()
            + "/releases/download/"
            + m_szCurrentVersion + "/" + szFileName;

    parser.addHelpOption();
    parser.addVersionOption();

    QCommandLineOption oFile(QStringList() << "f" << "file",
                             tr("Configure file name"),
                             "Configure file name",
                             "update.json");
    parser.addOption(oFile);
    QCommandLineOption oFileOuputContent(QStringList() << "foc" << "file-output-content",
                                 tr("Configure file output content:") + "\n"
                                     + QString::number(static_cast<int>(CONFIG_TYPE::VERSION)) + tr(": content is version") + "\n"
                                     + QString::number(static_cast<int>(CONFIG_TYPE::FILE)) + tr(": content is file") + "\n"
                                     + QString::number(static_cast<int>(CONFIG_TYPE::VERSION_FILE)) + tr(": content is version and file"),
                                 "Configure file output content",
                                 QString::number(static_cast<int>(CONFIG_TYPE::VERSION_FILE)));
    parser.addOption(oFileOuputContent);
    QCommandLineOption oPackageVersion(QStringList() << "pv" << "package-version",
                                tr("Package version"),
                                "Package version",
                                m_szCurrentVersion);
    parser.addOption(oPackageVersion);
    QCommandLineOption oTime(QStringList() << "t" << "time",
                             tr("Time"),
                             "Time",
                             QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
    parser.addOption(oTime);
    QCommandLineOption oInfo(QStringList() << "i" << "info",
                             tr("Information"),
                             "Information",
                             qApp->applicationName() + " " + m_szCurrentVersion);
    parser.addOption(oInfo);

    QCommandLineOption oSystem(QStringList() << "s" << "system",
                             tr("Operating system"),
                             "Operating system",
                             szSystem);
    parser.addOption(oSystem);
    QCommandLineOption oArch(QStringList() << "a" << "arch",
                             tr("Architecture"),
                             "Architecture",
                             QSysInfo::buildCpuArchitecture());
    parser.addOption(oArch);
    QCommandLineOption oMd5(QStringList() << "c" << "md5",
                             tr("MD5 checksum"),
                             "MD5 checksum");
    parser.addOption(oMd5);
    QCommandLineOption oPackageFile(QStringList() << "p" << "pf" << "package-file",
                            tr("Package file, Is used to calculate md5sum"),
                            "Package file"
                            );
    parser.addOption(oPackageFile);
    QCommandLineOption oFileName(QStringList() << "n" << "file-name",
                                 tr("File name"),
                                 "File name"
                                 );
    parser.addOption(oFileName);
    QCommandLineOption oUrl(QStringList() << "u" << "urls",
                             tr("Package download urls"),
                             "Download urls",
                             szUrl);
    parser.addOption(oUrl);
    QString szHome = "https://github.com/KangLin/" + qApp->applicationName();
    QCommandLineOption oUrlHome("home",
                             tr("Project home url"),
                             "Project home url",
                             szHome);
    parser.addOption(oUrlHome);
    QCommandLineOption oMin(QStringList() << "m" << "min" << "min-update-version",
                             tr("Min update version"),
                             "Min update version",
                             m_szCurrentVersion);
    parser.addOption(oMin);
    QCommandLineOption oForce(QStringList() << "force",
                              tr("Set force flag"),
                              "Force flag",
                              "false");
    parser.addOption(oForce);

    if(!parser.parse(QApplication::arguments())) {
        qDebug(log) << "parser.parse fail" << parser.errorText()
                              << qApp->arguments();
    }

    szFile = parser.value(oFile);
    if(szFile.isEmpty())
        qDebug(log) << "File is empty";

    type = static_cast<CONFIG_TYPE>(parser.value(oFileOuputContent).toInt());
    qDebug(log) << "File content is:" << (int)type;

    info.version.szVerion = parser.value(oPackageVersion);
    info.version.szMinUpdateVersion = parser.value(oMin);
    info.version.szTime = parser.value(oTime);
    info.version.szInfomation = parser.value(oInfo);
    info.version.szHome = parser.value(oUrlHome);
    QString szForce = parser.value(oForce).trimmed();
    if(szForce.compare("true", Qt::CaseInsensitive))
        info.version.bForce = false;
    else
        info.version.bForce = true;

    CONFIG_FILE file;
    file.szSystem = parser.value(oSystem);
    file.szArchitecture = parser.value(oArch);
    file.szMd5sum = parser.value(oMd5);
    file.szFileName = parser.value(oFileName);
    
    QString szPackageFile = parser.value(oPackageFile);
    if(!szPackageFile.isEmpty()) {
        QFileInfo fi(szPackageFile);
        if(file.szFileName.isEmpty())
            file.szFileName = fi.fileName();
        if(file.szMd5sum.isEmpty())
        {
            //计算包的 MD5 和
            QCryptographicHash md5sum(QCryptographicHash::Md5);
            QFile app(szPackageFile);
            if(app.open(QIODevice::ReadOnly))
            {
                if(md5sum.addData(&app))
                {
                    file.szMd5sum = md5sum.result().toHex();
                }
                app.close();
            } else {
                qCritical(log) << "Don't open package file:" << szPackageFile;
            }
        }
    }

    if(file.szMd5sum.isEmpty())
        qWarning(log) << "Md5 is empty. please set -c or --md5 or -p";

    /* 注意：这里要放在包文件后。
     * 优先级：
     *   1. -n 参数设置
     *   2. 从包文件中提取
     *   3. 默认值
     */
    if(file.szFileName.isEmpty())
        file.szFileName = szFileName;

    QString szUrls = parser.value(oUrl);
    foreach(auto u, szUrls.split(QRegularExpression("[;,]")))
    {
        file.urls.push_back(QUrl(u));
    }

    info.files.append(file);

    return 0;
}

void CFrmUpdater::showEvent(QShowEvent *event)
{
    Q_UNUSED(event)
    if(!m_StateMachine->isRunning())
        m_StateMachine->start();
}

void CFrmUpdater::slotShowWindow(QSystemTrayIcon::ActivationReason reason)
{
    Q_UNUSED(reason)
#if defined(Q_OS_ANDROID)
    showMaximized();
#else
    show();
#endif
    m_TrayIcon.hide();
}

bool CFrmUpdater::CheckPrompt(const QString &szVersion)
{
    QSettings set(RabbitCommon::CDir::Instance()->GetFileUserConfigure(),
                  QSettings::IniFormat);
    QString version = set.value("Updater/Version", m_szCurrentVersion).toString();
    set.setValue("Updater/Version", szVersion);
    int nRet = CompareVersion(szVersion, version);
    if (nRet > 0)
        return true;
    else if(nRet == 0)
        return !ui->cbPrompt->isChecked();
    return false;
}

void CFrmUpdater::on_cbPrompt_clicked(bool checked)
{
    QSettings set(RabbitCommon::CDir::Instance()->GetFileUserConfigure(),
                  QSettings::IniFormat);
    set.setValue("Updater/Prompt", checked);
}

void CFrmUpdater::on_cbHomePage_clicked(bool checked)
{
    QSettings set(RabbitCommon::CDir::Instance()->GetFileUserConfigure(),
                  QSettings::IniFormat);
    set.setValue("Updater/ShowHomePage", checked);
}

int CFrmUpdater::SetInstallAutoStartup(bool bAutoStart)
{
    m_InstallAutoStartupType = bAutoStart;
    return 0;
}

int CFrmUpdater::SetUpdateCallback(pUpdateCallback pCb)
{
    m_pcbUpdate = pCb;
    return 0;
}
