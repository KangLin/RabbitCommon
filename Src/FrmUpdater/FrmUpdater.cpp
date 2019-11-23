#include "FrmUpdater.h"
#include "RabbitCommonDir.h"
#include "RabbitCommonTools.h"
#include "AdminAuthoriser/adminauthoriser.h"
#include "ui_FrmUpdater.h"

#include <QtNetwork>
#include <QUrl>
#include <QDebug>
#include <QStandardPaths>
#include <QFinalState>
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

CFrmUpdater::CFrmUpdater(QString szUrl, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CFrmUpdater),
    m_ButtonGroup(this),
    m_bDownload(false),
    m_pReply(nullptr),
    m_pStateDownloadSetupFile(nullptr)
{
    bool check = false;
    setAttribute(Qt::WA_QuitOnClose, false);
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
    check = connect(&m_ButtonGroup, SIGNAL(buttonClicked(int)),
                    this, SLOT(slotButtonClickd(int)));
    Q_ASSERT(check);
    SetTitle();
    SetArch(BUILD_ARCH);
#if defined (Q_OS_WIN)
        m_szPlatform += "x86";
#elif defined(Q_OS_ANDROID)
        m_szPlatform += "arm";        
#elif defined (Q_OS_LINUX)
        m_szPlatform += "x86_64";
#endif
        
    QString szVerion = qApp->applicationVersion();
#ifdef BUILD_VERSION
    if(szVerion.isEmpty())
        szVerion = BUILD_VERSION;
#else
    szVerion = "0.0.1";
#endif
    SetVersion(szVerion);

    if(!QSslSocket::supportsSsl())
    {
#if (QT_VERSION >= QT_VERSION_CHECK(5, 4, 3))
        qCritical() << "Please install openssl first. openssl build version:"
                << QSslSocket::sslLibraryBuildVersionString();
#endif
    } else {
        qDebug() << QSslSocket::supportsSsl()
            #if (QT_VERSION >= QT_VERSION_CHECK(5, 4, 3))
                 <<QSslSocket::sslLibraryBuildVersionString()
           #endif
                <<QSslSocket::sslLibraryVersionString();
    }

    if(szUrl.isEmpty())
    {
        szUrl = "https://raw.githubusercontent.com/KangLin/"
                + qApp->applicationName() +"/master/Update/update.xml";
    }
    DownloadFile(QUrl(szUrl));
    
    InitStateMachine();
}

CFrmUpdater::~CFrmUpdater()
{
    m_DownloadFile.close();
    delete ui;
}

/**
 * @brief CFrmUpdater::InitStateMachine
 * @return 
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
 *   |  |--------------------|          |                  |
 *   |  |Download xml fil    |          |                  |
 *   |  |(sDownloadXmlFile)  |<-----|   |                  |
 *   |  |--------------------|      |   |                  |
 *   |      |                       |   |                  |
 *   |      | sigFinished           |   |                  |
 *   |      |                       |   |                  |
 *   |      V         sigDownLoadXml|   |                  |
 *   |  |--------------------|      |   |                  |
 *   |  |Check xml file      |      |   |                  |
 *   |  |(sCheckXmlFile)     |------|   |                  |
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
 */
int CFrmUpdater::InitStateMachine()
{
    QFinalState *sFinal = new QFinalState();
    QState *sCheck = new QState();
    QState *s = new QState();
    QState *sDownloadXmlFile = new QState(s);
    QState *sCheckXmlFile = new QState(s);
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

    s->setInitialState(sDownloadXmlFile);
    sDownloadXmlFile->assignProperty(ui->lbState, "text", tr("Being download xml file"));
    sDownloadXmlFile->addTransition(this, SIGNAL(sigFinished()), sCheckXmlFile);
    check = connect(sDownloadXmlFile, SIGNAL(entered()),
                     this, SLOT(slotDownloadXmlFile()));
    Q_ASSERT(check);
    
    sCheckXmlFile->addTransition(this, SIGNAL(sigDownLoadRedireXml()), sDownloadXmlFile);
    sCheckXmlFile->addTransition(this, SIGNAL(sigFinished()), sDownloadSetupFile);
    sCheckXmlFile->addTransition(ui->pbOK, SIGNAL(clicked()), sDownloadSetupFile);
    sCheckXmlFile->assignProperty(ui->pbOK, "text", tr("OK(&O)"));
    check = connect(sCheckXmlFile, SIGNAL(entered()),  this, SLOT(slotCheckXmlFile()));
    Q_ASSERT(check);
    
    m_pStateDownloadSetupFile = sDownloadSetupFile;
    sDownloadSetupFile->addTransition(this, SIGNAL(sigFinished()), sUpdate);
    sDownloadSetupFile->assignProperty(ui->lbState, "text", tr("Being download update file"));
    check = connect(sDownloadSetupFile, SIGNAL(entered()), this, SLOT(slotDownloadSetupFile()));
    Q_ASSERT(check);
    
    sUpdate->assignProperty(ui->lbState, "text", tr("Being install update"));
    check = connect(sUpdate, SIGNAL(entered()), this, SLOT(slotUpdate()));
    Q_ASSERT(check);
    
    m_StateMachine.addState(sCheck);    
    m_StateMachine.addState(s);
    m_StateMachine.addState(sFinal);
    m_StateMachine.setInitialState(sCheck);
    m_StateMachine.start();
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

int CFrmUpdater::SetArch(const QString &szArch)
{
    m_szCurrentArch = szArch;
    ui->lbCurrentArch->setText(tr("Current archecture: %1")
                               .arg(m_szCurrentArch));
    return 0;
}

int CFrmUpdater::SetVersion(const QString &szVersion)
{
    m_szCurrentVersion = szVersion;
    ui->lbCurrentVersion->setText(tr("Current version: %1")
                                  .arg(m_szCurrentVersion));
    return 0;
}

int CFrmUpdater::DownloadFile(const QUrl &url, bool bRedirection, bool bDownload)
{
    int nRet = 0;
    qDebug() << "CFrmUpdater::DownloadFile:" << url << bRedirection << bDownload;
    if(!m_StateMachine.isRunning())
    {
        m_bDownload = bDownload;        
        m_Url = url;
        return 0;
    }
    m_DownloadFile.close();
    if(url.isLocalFile())
    {
        m_DownloadFile.setFileName(url.toLocalFile());
        emit sigFinished();
        return 0;
    }

    if(!bRedirection)
    {
        QString szTmp
                = QStandardPaths::writableLocation(QStandardPaths::TempLocation);
        szTmp = szTmp + QDir::separator() + "Rabbit"
                + QDir::separator() + qApp->applicationName();
        QDir d;
        if(!d.exists(szTmp))
            d.mkpath(szTmp);
        QString szPath = url.path();   
        QString szFile = szTmp + szPath.mid(szPath.lastIndexOf("/"));
        m_DownloadFile.setFileName(szFile);
        qDebug() << "CFrmUpdater download file: " << m_DownloadFile.fileName();
        
    }

    if(!m_DownloadFile.open(QIODevice::WriteOnly))
    {
        qDebug() << "Open file fail: " << m_DownloadFile.fileName();
        return -1;
    }
    
    QNetworkRequest request(url);
    //https://blog.csdn.net/itjobtxq/article/details/8244509
    /*QSslConfiguration config;
    config.setPeerVerifyMode(QSslSocket::VerifyNone);
    config.setProtocol(QSsl::AnyProtocol);
    request.setSslConfiguration(config);
    */
    m_pReply = m_NetManager.get(request);
    if(!m_pReply)
        return -1;
    
    bool check = false;
    check = connect(m_pReply, SIGNAL(readyRead()), this, SLOT(slotReadyRead()));
    Q_ASSERT(check);
    check = connect(m_pReply, SIGNAL(downloadProgress(qint64, qint64)),
                    this, SLOT(slotDownloadProgress(qint64, qint64)));
    Q_ASSERT(check);
    check = connect(m_pReply, SIGNAL(error(QNetworkReply::NetworkError)),
                    this, SLOT(slotError(QNetworkReply::NetworkError)));
    Q_ASSERT(check);
    check = connect(m_pReply, SIGNAL(sslErrors(const QList<QSslError>)),
                    this, SLOT(slotSslError(const QList<QSslError>)));
    check = connect(m_pReply, SIGNAL(finished()),
                    this, SLOT(slotFinished()));
    
    ui->progressBar->show();
    return nRet;
}

void CFrmUpdater::slotReadyRead()
{
    //qDebug() << "CFrmUpdater::slotReadyRead()";
    if(m_DownloadFile.isOpen() && m_pReply)
    {
        QByteArray d = m_pReply->readAll();
        //qDebug() << d;
        m_DownloadFile.write(d);
    }
}

void CFrmUpdater::slotFinished()
{
    qDebug() << "CFrmUpdater::slotFinished()";
    
    QVariant redirectionTarget;
    if(m_pReply)
       redirectionTarget = m_pReply->attribute(QNetworkRequest::RedirectionTargetAttribute);
    if(redirectionTarget.isValid())
    {
        if(m_pReply)
        {
            m_pReply->disconnect();
            m_pReply->deleteLater();
            m_pReply = nullptr;
        }
        QUrl u = redirectionTarget.toUrl();  
        if(u.isValid())
        {
            qDebug() << "CFrmUpdater::slotFinished():redirectionTarget:url:" << u;
            DownloadFile(u, true);
        }
        return;
    }
    
    if(m_pReply)
    {
        m_pReply->disconnect();
        m_pReply->deleteLater();
        m_pReply = nullptr;
    }
    m_DownloadFile.close();
    ui->progressBar->hide();
    emit sigFinished();
}

void CFrmUpdater::slotDownloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    ui->progressBar->setRange(0, static_cast<int>(bytesTotal));
    ui->progressBar->setValue(static_cast<int>(bytesReceived));
    m_TrayIcon.setToolTip(windowTitle() + " - "
                          + qApp->applicationDisplayName()
                          + tr(": downloading %1%").arg(
                            QString::number(bytesReceived * 100 / bytesTotal)));
}

void CFrmUpdater::slotError(QNetworkReply::NetworkError e)
{
    qDebug() << "CFrmUpdater::slotError: " << e;
    if(m_pReply)
    {
        ui->lbState->setText(tr("Download network error: ")
                             + m_pReply->errorString());
        m_pReply->disconnect();
        m_pReply->deleteLater();
        m_pReply = nullptr;
    }
    ui->progressBar->hide();
    m_DownloadFile.close();
    emit sigError();
}

void CFrmUpdater::slotSslError(const QList<QSslError> e)
{
    qDebug() << "CFrmUpdater::slotSslError: " << e;
    QString sErr;
    foreach(QSslError s, e)
        sErr += s.errorString() + " ";
    ui->lbState->setText(tr("Download fail:") + sErr);
    if(m_pReply)
    {
        m_pReply->disconnect();
        m_pReply->deleteLater();
        m_pReply = nullptr;
    }
    ui->progressBar->hide();
    m_DownloadFile.close();
    emit sigError();
}

void CFrmUpdater::slotStateFinished()
{
    if(m_pReply)
        m_pReply->abort();
}

void CFrmUpdater::slotCheck()
{
    qDebug() << "CFrmUpdater::slotCheck()";
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

void CFrmUpdater::slotDownloadXmlFile()
{
    qDebug() << "CFrmUpdater::slotDownloadXmlFile";
    if(m_Url.isValid())
        DownloadFile(m_Url);
}

void CFrmUpdater::slotCheckXmlFile()
{
    m_TrayIcon.setToolTip(windowTitle() + " - "
                          + qApp->applicationDisplayName());
    qDebug() << "CFrmUpdater::slotCheckXmlFile()";
    if(CheckRedirectXmlFile() <= 0)
        return;
    
    CheckUpdateXmlFile();
}

/*
   <?xml version="1.0" encoding="UTF-8"?>
   <REDIRECT>
       <VERSION>v0.0.1</VERSION>
       <WINDOWS>
           <URL>url</URL>
       </WINDOWS>
       <LINUX>
           <URL>url</URL>
       </LINUX>
       <LINUX_APPIMAGE>
           <URL>url</URL>
       </LINUX_APPIMAGE>
       <ANDROID>
           <URL>url</URL>
       </ANDROID>   
   </REDIRECT>
 */
int CFrmUpdater::CheckRedirectXmlFile()
{
    qDebug() << "CFrmUpdater::CheckRedirectXmlFile()";
    if(!m_DownloadFile.open(QIODevice::ReadOnly))
    {
        qCritical() << "CFrmUpdater::slotCheckXmlFile: open file fail:" << m_DownloadFile.fileName();
        emit sigError();
        return -1;
    }
    QDomDocument doc;
    if(!doc.setContent(&m_DownloadFile))
    {
        QString szError = tr("Parse file %1 fail. It isn't xml file")
                .arg(m_DownloadFile.fileName());
        ui->lbState->setText(szError);
        qDebug() << "CFrmUpdater::slotCheckXmlFile:" << szError;
        m_DownloadFile.close();
        emit sigError();
        return -2;
    }
    m_DownloadFile.close();
    if(doc.elementsByTagName("REDIRECT").isEmpty())
        return 1;

    QString szUrl;
    QString szVersion;
    QDomNodeList versionNode = doc.documentElement().elementsByTagName("VERSION");
    if(versionNode.isEmpty())
    {
        emit sigError();
        return -3;
    }

    QDomElement node = versionNode.item(0).toElement();
    szVersion = node.text();
    szUrl = "https://github.com/KangLin/" 
            + qApp->applicationName() + "/releases/download/"
            + szVersion + "/update_";
#if defined (Q_OS_WIN)
    szUrl += "windows";
    QDomNodeList n = doc.documentElement().elementsByTagName("WINDOWS");
    if(!n.isEmpty())
    {
        QDomElement url = n.item(0).firstChildElement("URL");
        szUrl = url.text();
    }
#elif defined(Q_OS_ANDROID)
    szUrl += "android";
    QDomNodeList n = doc.documentElement().elementsByTagName("ANDROID");
    if(!n.isEmpty())
    {
        QDomElement url = n.item(0).firstChildElement("URL");
        szUrl = url.text();
    }
#elif defined (Q_OS_LINUX)
    /*QFileInfo f(qApp->applicationFilePath());
    if(f.suffix().compare("AppImage", Qt::CaseInsensitive))
    {   
        szUrl += "linux";
        QDomNodeList n = doc.documentElement().elementsByTagName("LINUX");
        if(!n.isEmpty())
        {
            QDomElement url = n.item(0).firstChildElement("URL");
            m_Url = url.text();
            emit sigDownLoadRedireXml();
            return 0;
        }
    }
    else*/
    {
        szUrl += "linux_appimage";
        QDomNodeList n = doc.documentElement().elementsByTagName("LINUX_APPIMAGE");
        if(!n.isEmpty())
        {
            QDomElement url = n.item(0).firstChildElement("URL");
            m_Url = url.text();
            emit sigDownLoadRedireXml();
            return 0;
        }
    }
    
#endif
    szUrl += ".xml";
    
    m_Url = szUrl;
    emit sigDownLoadRedireXml();

    return 0;
}

/*
   <?xml version="1.0" encoding="UTF-8"?>
   <UPDATE>
    <VERSION>v0.0.1</VERSION>
    <TIME>2019-2-24 19:28:50</TIME>
    <INFO>v0.0.1 information</INFO>
    <FORCE>0</FORCE>
    <SYSTEM>windows</SYSTEM>         <!--windows, linux, android-->
    <PLATFORM>x86</PLATFORM>         <!--windows, linux, android-->
    <ARCHITECTURE>x86</ARCHITECTURE> <!--x86, x86_64, armeabi-v7a-->
    <URL>url</URL>
    <HOME>home page</HOME>
    <MD5SUM>%RABBITIM_MD5SUM%</MD5SUM>
    <MIN_UPDATE_VERSION>%MIN_UPDATE_VERSION%</MIN_UPDATE_VERSION>
   </UPDATE>
 */
int CFrmUpdater::CheckUpdateXmlFile()
{
    qDebug() << "CFrmUpdater::CheckUpdateXmlFile()";
    if(!m_DownloadFile.open(QIODevice::ReadOnly))
    {
        qCritical() << "CFrmUpdater::slotCheckXmlFile: open file fail:" << m_DownloadFile.fileName();
        emit sigError();
        return -1;
    }
    QDomDocument doc;
    if(!doc.setContent(&m_DownloadFile))
    {
        QString szError = tr("Parse file %1 fail. It isn't xml file")
                .arg(m_DownloadFile.fileName());
        ui->lbState->setText(szError);
        qDebug() << "CFrmUpdater::slotCheckXmlFile:" << szError;
        m_DownloadFile.close();
        emit sigError();
        return -2;
    }
    m_DownloadFile.close();
    if(doc.elementsByTagName("UPDATE").isEmpty())
    {
        QString szError = tr("Parse file %1 fail. It isn't update xml file")
                .arg(m_DownloadFile.fileName());
        ui->lbState->setText(szError);
        emit sigError();
        return -3;
    }
    QDomNodeList lstNode = doc.documentElement().childNodes();
    for(int i = 0; i < lstNode.length(); i++)
    {
        QDomElement node = lstNode.item(i).toElement();
        if(node.nodeName() == "VERSION")
            m_Info.szVerion = node.text();
        else if(node.nodeName() == "TIME")
            m_Info.szTime = node.text();
        else if(node.nodeName() == "INFO")
            m_Info.szInfomation = node.text();
        else if(node.nodeName() == "FORCE")
            m_Info.bForce = node.text().toInt();
        else if(node.nodeName() == "SYSTEM")
            m_Info.szSystem = node.text();
        else if(node.nodeName() == "PLATFORM")
            m_Info.szPlatform = node.text();
        else if(node.nodeName() == "ARCHITECTURE")
            m_Info.szArchitecture = node.text();
        else if(node.nodeName() == "URL")
            m_Info.szUrl = node.text();
        else if(node.nodeName() == "HOME")
            m_Info.szUrlHome = node.text();
        else if(node.nodeName() == "MD5SUM")
            m_Info.szMd5sum = node.text();
        else if(node.nodeName() == "MIN_UPDATE_VERSION")
            m_Info.szMinUpdateVersion = node.text();
    }
    qDebug() << "version: " << m_Info.szVerion
             << "time: " << m_Info.szTime
             << "bForce: " << m_Info.bForce
             << "system: " << m_Info.szSystem
             << "Platform: " << m_Info.szPlatform
             << "Arch: " << m_Info.szArchitecture
             << "url: " << m_Info.szUrl
             << "md5: " << m_Info.szMd5sum
             << "minUpdateVersion: " << m_Info.szMinUpdateVersion;
    if(CompareVersion(m_Info.szVerion, m_szCurrentVersion) <= 0)
    {
        ui->lbState->setText(tr("There is laster version"));
        emit sigError();
        return -4;
    }

    ui->lbNewVersion->setText(tr("New version: %1").arg(m_Info.szVerion));
    ui->lbNewVersion->show();
    ui->lbNewArch->setText(tr("New architecture: %1").arg(m_Info.szArchitecture));
    ui->lbNewArch->show();
    
#if defined (Q_OS_WIN)
    if(m_Info.szSystem.compare("windows", Qt::CaseInsensitive))
    {
        ui->lbState->setText(tr("System is different"));
        emit sigError();
        return -5;
    }
    if(!m_szCurrentArch.compare("x86", Qt::CaseInsensitive)
        && !m_Info.szArchitecture.compare("x86_64", Qt::CaseInsensitive))
    {
        ui->lbState->setText(tr("Architecture is different"));
        emit sigError();
        return -6;
    }
#elif defined(Q_OS_ANDROID)
    if(m_Info.szSystem.compare("android", Qt::CaseInsensitive))
    {
        ui->lbState->setText(tr("System is different"));
        emit sigError();
        return -7;
    }
    if(m_szCurrentArch != m_Info.szArchitecture)
    {
        ui->lbState->setText(tr("Architecture is different"));
        emit sigError();
        return -8;
    }
#elif defined (Q_OS_LINUX)
    if(m_Info.szSystem.compare("linux", Qt::CaseInsensitive))
    {
        ui->lbState->setText(tr("System is different"));
        emit sigError();
        return -9;
    }
    if(!m_szCurrentArch.compare("x86", Qt::CaseInsensitive)
        && !m_Info.szArchitecture.compare("x86_64", Qt::CaseInsensitive))
    {
        ui->lbState->setText(tr("Architecture is different"));
        emit sigError();
        return -10;
    }
#endif   
    
    ui->lbState->setText(tr("There is a new version, is it updated?"));
    if(m_Info.bForce)
    {
        qDebug() << "Force update";
        emit sigFinished();
    }
    else
    {
        ui->cbHomePage->show();
        ui->cbPrompt->show();
        ui->pbOK->setText(tr("OK(&O)"));
        ui->pbOK->show();
        if(!CheckPrompt(m_Info.szVerion) && this->isHidden())
            emit sigError();
        else
            show();
    }
    return 0;
}

void CFrmUpdater::slotDownloadSetupFile()
{
    qDebug() << "CFrmUpdater::slotDownloadSetupFile()";
    ui->pbOK->setText(tr("Hide"));
    ui->lbState->setText(tr("Download ......"));
    if(IsDownLoad())
        emit sigFinished();
    else
        DownloadFile(m_Info.szUrl);
}

void CFrmUpdater::slotUpdate()
{
    m_TrayIcon.setToolTip(windowTitle() + " - "
                          + qApp->applicationDisplayName());
    ui->lbState->setText(tr("Being install update ......"));
    qDebug() << "CFrmUpdater::slotUpdate()";
    if(!m_DownloadFile.open(QIODevice::ReadOnly))
    {
        qCritical() << "Download file fail: " << m_DownloadFile.fileName();
        ui->lbState->setText(tr("Download file fail"));
        emit sigError();
        return;
    }

    do{
        QCryptographicHash md5sum(QCryptographicHash::Md5);
        if(!md5sum.addData(&m_DownloadFile))
        {
            ui->lbState->setText(tr("Download file fail"));
            emit sigError();
            break;
        }
        if(md5sum.result().toHex() != m_Info.szMd5sum)
        {
            QString szFail;
            szFail = tr("Md5sum is different. ")
                    + "\n" + tr("Download file md5sum: ")
                    + md5sum.result().toHex()
                    + "\n" + tr("md5sum in Update.xml:")
                    + m_Info.szMd5sum;
            ui->lbState->setText(szFail);
            emit sigError();
            break;
        }

        m_DownloadFile.close();

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
        if(fi.suffix().compare("gz", Qt::CaseInsensitive))
        {
            //启动安装程序  
            if(!proc.startDetached(m_DownloadFile.fileName()))
            {
                QUrl url(m_DownloadFile.fileName());
                if(!QDesktopServices::openUrl(url))
                {
                    QString szErr = tr("Execute install program error.%1")
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
                QString szErr = tr("Open file %1 fail").arg(fi.absolutePath());
                ui->lbState->setText(szErr);
                break;
            }
            QString szCmd = InstallScript(m_DownloadFile.fileName(),
                                          qApp->applicationName());
            f.write(szCmd.toStdString().c_str());
            qDebug() << szCmd << szInstall;
            f.close();

            //启动安装程序
            if(!RabbitCommon::CTools::executeByRoot("/bin/bash",
                                                    QStringList() << szInstall))
            {
                QString szErr = tr("Execute") + "/bin/bash "
                                + szInstall + "fail";
                ui->lbState->setText(szErr);
                break;
            }
        }

        ui->lbState->setText(tr("The installer has started, Please close the application"));
        
        //system(m_DownloadFile.fileName().toStdString().c_str());
        //int nRet = QProcess::execute(m_DownloadFile.fileName());
        //qDebug() << "QProcess::execute return: " << nRet;
        emit sigFinished();
    }while(0);

    QProcess procHome;
    if(ui->cbHomePage->isChecked() && !m_Info.szUrlHome.isEmpty())
        if(!procHome.startDetached(m_Info.szUrlHome))
        {
            QUrl url(m_Info.szUrlHome);
            if(!QDesktopServices::openUrl(url))
            {
                QString szErr = tr("Execute install program error.%1")
                        .arg(m_DownloadFile.fileName());
                ui->lbState->setText(szErr);
            }
        }
    
    m_DownloadFile.close();

    qApp->quit();
}

QString CFrmUpdater::InstallScript(const QString szDownLoadFile,
                                   const QString szApplicationName)
{
    QFileInfo fi(szDownLoadFile);
    QString szCmd;
    szCmd = "#!/bin/bash\n";
    szCmd += "set -e\n";
    szCmd += "mkdir -p /opt/" + szApplicationName + "\n";
    szCmd += "cd /opt/" + szApplicationName + "\n";
    szCmd += "if [ -f install.sh ]; then\n";
    szCmd += "    ./install.sh remove " + szApplicationName + "\n";
    //szCmd += "    rm -fr *\n";
    szCmd += "fi\n";
    szCmd += "cp " + szDownLoadFile + " ." + "\n";
    szCmd += "tar xvfz " + fi.fileName() + "\n";
    szCmd += "rm " + fi.fileName() + "\n";
    
    //See: Install/install.sh
    szCmd += "./install1.sh";

    int nRet = 0;
    switch (m_RunType) {
    case RUN_AUTO_STARTUP:
        nRet = QInputDialog::getInt(this, tr("Run"),
                                    tr("0: install"
                                       "1: run after install"
                                       "2: auto run when boot?"));
        break;
    default:
        nRet = QInputDialog::getInt(this, tr("Run"),
                                    tr("0: install"
                                       "1: Is run after install?"));
        break;
    }
    
    switch (nRet) {
    case 1:
        szCmd += " install-run "; 
        break;
    case 2:
        szCmd += " install_auto_run ";
        break;
    default:
        szCmd += " install ";
    }

    szCmd += szApplicationName + "\n";
    return szCmd;
}

int CFrmUpdater::CompareVersion(const QString &newVersion, const QString &currentVersion)
{
    int nRet = 0;
    QString sN = newVersion;
    sN = sN.replace("-", ".");
    QString sC = currentVersion;
    sC = sC.replace("-", ".");
    QStringList szNew = sN.split(".");
    QStringList szCur = sC.split(".");
    QString firstNew = szNew.at(0);
    QString firstCur = szCur.at(0);
    firstNew = firstNew.remove(QChar('v'), Qt::CaseInsensitive);
    firstCur = firstCur.remove(QChar('v'), Qt::CaseInsensitive);
    if(firstNew.toInt() > firstCur.toInt())
        return 1;
    else if(firstNew.toInt() < firstCur.toInt()){
        return -1;
    }
    if(szNew.at(1).toInt() > szCur.at(1).toInt())
        return 1;
    else if(szNew.at(1).toInt() < szCur.at(1).toInt()){
        return -1;
    }
    if(szNew.at(2).toInt() > szCur.at(2).toInt())
        return 1;
    else if(szNew.at(2).toInt() < szCur.at(2).toInt()){
        return -1;
    }
    if(szNew.length() >=4 && szCur.length() < 4)
        return 1;
    else if(szNew.length() < 4 && szCur.length() >= 4)
        return -1;
    else if(szNew.length() >= 4 && szCur.length() >= 4)
    {
        if(szNew.at(3).toInt() > szCur.at(3).toInt())
            return 1;
        else if(szNew.at(3).toInt() < szCur.at(3).toInt()){
            return -1;
        }
    }
    return nRet;
}

bool CFrmUpdater::IsDownLoad()
{
    bool bRet = false;
    QString szTmp
            = QStandardPaths::writableLocation(QStandardPaths::TempLocation);
    szTmp = szTmp + QDir::separator() + "Rabbit"
            + QDir::separator() + qApp->applicationName();

    QString szPath = QUrl(m_Info.szUrl).path();   
    QString szFile = szTmp + szPath.mid(szPath.lastIndexOf("/"));
    
    QFile f(szFile);
    if(!f.open(QIODevice::ReadOnly))
        return false;
    
    m_DownloadFile.setFileName(szFile);
    do{
        QCryptographicHash md5sum(QCryptographicHash::Md5);
        if(!md5sum.addData(&f))
        {
            bRet = false;
            break;
        }
        if(md5sum.result().toHex() != m_Info.szMd5sum)
        {
            bRet = false;
            break;
        }
        else 
        {
            bRet = true;
            break;
        }
    }while(0);
    f.close();
    return bRet;
}

void CFrmUpdater::on_pbOK_clicked()
{
    qDebug() << "CFrmUpdater::on_pbOK_clicked()";
    if(!m_pStateDownloadSetupFile->active())
        return;
    
    m_TrayIcon.show();
    hide();
}

void CFrmUpdater::on_pbClose_clicked()
{
    if(m_StateMachine.isRunning())
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

int CFrmUpdater::GenerateUpdateXmlFile(const QString &szFile, const INFO &info)
{    
    QDomDocument doc;
    QDomProcessingInstruction ins;
    //<?xml version='1.0' encoding='UTF-8'?>
    ins = doc.createProcessingInstruction("xml", "version=\'1.0\' encoding=\'UTF-8\'");
    doc.appendChild(ins);
    QDomElement root = doc.createElement("UPDATE");
    doc.appendChild(root);
    
    QDomText version = doc.createTextNode("VERSION");
    version.setData(info.szVerion);
    QDomElement eVersion = doc.createElement("VERSION");
    eVersion.appendChild(version);
    root.appendChild(eVersion);
    
    QDomText time = doc.createTextNode("TIME");
    time.setData(info.szTime);
    QDomElement eTime = doc.createElement("TIME");
    eTime.appendChild(time);
    root.appendChild(eTime);

    QDomText i = doc.createTextNode("INFO");
    i.setData(info.szInfomation);
    QDomElement eInfo = doc.createElement("INFO");
    eInfo.appendChild(i);
    root.appendChild(eInfo);
    
    QDomText force = doc.createTextNode("FORCE");
    force.setData(QString::number(info.bForce));
    QDomElement eForce = doc.createElement("FORCE");
    eForce.appendChild(force);
    root.appendChild(eForce);
    
    QDomText system = doc.createTextNode("SYSTEM");
    system.setData(info.szSystem);
    QDomElement eSystem = doc.createElement("SYSTEM");
    eSystem.appendChild(system);
    root.appendChild(eSystem);
    
    QDomText platform = doc.createTextNode("PLATFORM");
    platform.setData(info.szPlatform);
    QDomElement ePlatform = doc.createElement("PLATFORM");
    ePlatform.appendChild(platform);
    root.appendChild(ePlatform);
    
    QDomText arch = doc.createTextNode("ARCHITECTURE");
    arch.setData(info.szArchitecture);
    QDomElement eArch = doc.createElement("ARCHITECTURE");
    eArch.appendChild(arch);
    root.appendChild(eArch);
    
    QDomText md5 = doc.createTextNode("MD5SUM");
    md5.setData(info.szMd5sum);
    QDomElement eMd5 = doc.createElement("MD5SUM");
    eMd5.appendChild(md5);
    root.appendChild(eMd5);
    
    QDomText url = doc.createTextNode("URL");
    url.setData(info.szUrl);
    QDomElement eUrl = doc.createElement("URL");
    eUrl.appendChild(url);
    root.appendChild(eUrl);
    
    QDomText urlHome = doc.createTextNode("HOME");
    urlHome.setData(info.szUrlHome);
    QDomElement eUrlHome = doc.createElement("HOME");
    eUrlHome.appendChild(urlHome);
    root.appendChild(eUrlHome);
    
    QDomText min = doc.createTextNode("MIN_UPDATE_VERSION");
    min.setData(info.szMinUpdateVersion);
    QDomElement eMin = doc.createElement("MIN_UPDATE_VERSION");
    eMin.appendChild(min);
    root.appendChild(eMin);
    
    QFile f;
    f.setFileName(szFile);
    if(!f.open(QIODevice::WriteOnly))
    {
        qCritical() << "CFrmUpdater::GenerateUpdateXml file open file fail:"
                    << f.fileName();
        return -1;
    }
    QTextStream stream(&f);
    stream.setCodec("UTF-8");
    doc.save(stream, 4);
    f.close();
    return 0;
}

int CFrmUpdater::GenerateUpdateXml()
{
    int nRet = -1;
    INFO info;
    
    QString szSystem, szUrl;
#if defined (Q_OS_WIN)
    szSystem = "Windows";
    szUrl = "https://github.com/KangLin/"
            + qApp->applicationName()
            + "/releases/download/"
            + m_szCurrentVersion + "/"
            + qApp->applicationName()
            + "-Setup-"
            + m_szCurrentVersion + ".exe";
#elif defined(Q_OS_ANDROID)
    szSystem = "Android";
    szUrl = "https://github.com/KangLin/"
            + qApp->applicationName()
            + "/releases/download/"
            + m_szCurrentVersion + "/"
            + qApp->applicationName().toLower()
            + m_szCurrentVersion + ".apk";
#elif defined(Q_OS_LINUX)
    szSystem = "Linux";
    QFileInfo f(qApp->applicationFilePath());
    if(f.suffix().compare("AppImage", Qt::CaseInsensitive))
    {
        QString szVersion = m_szCurrentVersion;
        szVersion.replace("v", "", Qt::CaseInsensitive);
        szUrl = "https://github.com/KangLin/"
                + qApp->applicationName()
                + "/releases/download/"
                + m_szCurrentVersion + "/"
                + qApp->applicationName().toLower()
                + "_" + szVersion + "_amd64.deb";
    } else {
        szUrl = "https://github.com/KangLin/"
                + qApp->applicationName()
                + "/releases/download/"
                + m_szCurrentVersion + "/"
                + qApp->applicationName()
                + "_" + m_szCurrentVersion + ".tar.gz";
    }
#endif
    
    QCommandLineParser parser;
    parser.addHelpOption();
    parser.addVersionOption();

    QCommandLineOption oFile(QStringList() << "f" << "file",
                             "xml file name",
                             "xml file name");
    parser.addOption(oFile);
    QCommandLineOption oPackageVersion("pv",
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
    QCommandLineOption oPlatform(QStringList() << "p" << "platform",
                             tr("Platform"),
                             "Platform",
                             m_szPlatform);
    parser.addOption(oPlatform);
    QCommandLineOption oArch(QStringList() << "a" << "arch",
                             tr("Architecture"),
                             "Architecture",
                             m_szCurrentArch);
    parser.addOption(oArch);
    QCommandLineOption oMd5(QStringList() << "c" << "md5",
                             tr("MD5 checksum"),
                             "MD5 checksum");
    parser.addOption(oMd5);
    QCommandLineOption oUrl(QStringList() << "u" << "url",
                             tr("Package download url"),
                             "Download url",
                             szUrl);
    parser.addOption(oUrl);
    QString szHome = "https://github.com/KangLin/" + qApp->applicationName();
    QCommandLineOption oUrlHome("home",
                             tr("Project home url"),
                             "Project home url",
                             szHome);
    parser.addOption(oUrlHome);
    QCommandLineOption oMin(QStringList() << "m" << "min",
                             tr("Min update version"),
                             "Min update version",
                             m_szCurrentVersion);
    parser.addOption(oMin);

    parser.process(QApplication::arguments());
    
    QString szFile = parser.value(oFile);
    if(szFile.isEmpty())
        return nRet;
    
    info.szVerion = parser.value(oPackageVersion);
    info.szTime = parser.value(oTime);
    info.szInfomation = parser.value(oInfo);
    info.bForce = false;
    info.szSystem = parser.value(oSystem);
    info.szPlatform = parser.value(oPlatform);
    info.szArchitecture = parser.value(oArch);
    info.szMd5sum = parser.value(oMd5);
    if(info.szMd5sum.isEmpty())
    {
        //TODO: Add package
        QCryptographicHash md5sum(QCryptographicHash::Md5);
        QFile app(qApp->applicationFilePath());
        if(app.open(QIODevice::ReadOnly))
        {
            if(md5sum.addData(&app))
            {
                info.szMd5sum = md5sum.result().toHex();
            }
            app.close();
        }
    }
    info.szUrl = parser.value(oUrl);
    info.szUrlHome = parser.value(oUrlHome);
    info.szMinUpdateVersion = parser.value(oMin);
   
    return GenerateUpdateXmlFile(szFile, info);
}

void CFrmUpdater::showEvent(QShowEvent *event)
{
    Q_UNUSED(event)
    if(!m_StateMachine.isRunning())
        m_StateMachine.start();
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

int CFrmUpdater::SetRunType(RunType type)
{
    m_RunType = type;
    return 0;
}
