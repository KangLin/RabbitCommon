// Copyright Copyright (c) Kang Lin studio, All Rights Reserved
// Author Kang Lin <kl222@126.com>

#include "RabbitCommonTools.h"
#include "RabbitCommonDir.h"
#ifdef HAVE_ADMINAUTHORISER
#include "AdminAuthoriser/adminauthoriser.h"
#endif
#include "RabbitCommonRegister.h"
#include "Log/Log.h"

#include <QSettings>

#ifdef HAVE_RABBITCOMMON_GUI
    #include <QIcon>
    #include <QMainWindow>
    #include "Style.h"
    #include <QApplication>
#else
    #include <QCoreApplication>
#endif

#include <QLocale>
#include <QDir>
#include <QStandardPaths>
#include <QSslSocket>

#if defined(Q_OS_ANDROID)
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    #include <QtCore/private/qandroidextras_p.h>
#endif
#endif

#if defined(HAVE_OPENSSL)
    #include "openssl/opensslv.h"
#endif

#ifdef WINDOWS
    #include <windows.h>
    #include <tchar.h>
//    #include <stdio.h>
    //#include "lm.h"
    //#pragma comment(lib,"netapi32.lib")
#elif UNIX
    #include <pwd.h>
    #include <unistd.h>
#endif

#ifdef Q_OS_WIN
    #include "CoreDump/MiniDumper.h"
#endif

#ifdef HAVE_RABBITCOMMON_GUI
    #include "Log/DockDebugLog.h"
    #include "Log/DlgFilter.h"
    extern CDockDebugLog* g_pDcokDebugLog;
#endif

inline void g_RabbitCommon_InitResource()
{
    Q_INIT_RESOURCE(ResourceRabbitCommon);

#ifdef BUILD_QUIWidget
    Q_INIT_RESOURCE(QUIWidget);
#endif
#if DEBUG
    // Must set cmake parameters: -DCMAKE_BUILD_TYPE=Debug
    //Q_INIT_RESOURCE(translations_RabbitCommon);
#ifdef BUILD_QUIWidget
    Q_INIT_RESOURCE(QUIWidgetQss);
#endif
#endif
}

inline void g_RabbitCommon_CleanResource()
{
    Q_CLEANUP_RESOURCE(ResourceRabbitCommon);
#ifdef BUILD_QUIWidget
    Q_CLEANUP_RESOURCE(QUIWidget);
#endif
#if DEBUG
    //Q_CLEANUP_RESOURCE(translations_RabbitCommon);
#ifdef BUILD_QUIWidget
    Q_CLEANUP_RESOURCE(QUIWidgetQss);
#endif
#endif
}

namespace RabbitCommon {

static Q_LOGGING_CATEGORY(log, "RabbitCommon.Tools")
static Q_LOGGING_CATEGORY(logTranslation, "RabbitCommon.Tools.Translation")

CTools::CTools() : 
    m_Initialized(false)
{   
}

CTools::~CTools()
{
#if HAVE_RABBITCOMMON_GUI
    if(g_pDcokDebugLog)
        delete g_pDcokDebugLog;
#endif
}

CTools* CTools::Instance()
{
    static CTools* pTools = nullptr;
    if(nullptr == pTools)
        pTools = new CTools();
    return pTools;
}

int CTools::SetLanguage(const QString szLanguage)
{
    m_szLanguage = szLanguage;
    return 0;
}

QString CTools::GetLanguage()
{
    if(m_szLanguage.isEmpty())
        return QLocale::system().name();
    return m_szLanguage;
}

QString CTools::Version()
{
    QString szReturn;
    QString szVersion(RabbitCommon_VERSION);
    QString szRevision(RabbitCommon_REVISION);
    if(szRevision.isEmpty()) {
        szReturn = QObject::tr("Version: ") + szVersion;
    } else {
        szReturn = QObject::tr("Version: ") + szVersion + QObject::tr(" (From revision: "); 
#if (defined(HAVE_CMARK) || defined(HAVE_CMARK_GFM)) && defined(HAVE_WebEngineWidgets)
            szReturn += "[" + szRevision + "](http://github.com/KangLin/RabbitCommon/tree/" + szRevision + ")";
#else
            szReturn += szRevision;
#endif
        szReturn += ") ";
    }
    return szReturn;
}

QString CTools::Information()
{
    QString szInfo;
    szInfo = QObject::tr("- Functions:") + "\n";
#if defined(HAVE_RABBITCOMMON_GUI)
    szInfo += QObject::tr("  - Have GUI") + "\n";
#endif
#if defined(HAVE_ABOUT)
    szInfo += QObject::tr("  - Have about diaglog") + "\n";
//    #ifdef HAVE_CMARK_GFM
//    szInfo += QObject::tr("    - Use cmark-gfm") + "\n";
//    #elif HAVE_CMARK
//    szInfo += QObject::tr("    - Use cmark") + "\n";
//    #endif
//    #ifdef HAVE_WebEngineWidgets
//    szInfo += QObject::tr("    - Use WebEngineWidgets") + "\n";
//    #endif
#endif
#if defined(HAVE_UPDATE)
    szInfo += QObject::tr("  - Have update") + "\n";
#endif
#if defined(HAVE_OPENSSL)
    szInfo += QObject::tr("  - Have encrypt(OPENSSL)") + "\n";
#endif
#if defined(BUILD_QUIWidget)
    szInfo += QObject::tr("  - Have QUIWidget") + "\n";
#endif
    szInfo += QObject::tr("  - Applicatoin paths and files: ") + "\n";
    szInfo += QObject::tr("    - Install root path: ") + RabbitCommon::CDir::Instance()->GetDirApplicationInstallRoot() + "\n";
    szInfo += QObject::tr("    - Application path: ") + RabbitCommon::CDir::Instance()->GetDirApplication() + "\n";
    szInfo += QObject::tr("    - Configure path: ") + RabbitCommon::CDir::Instance()->GetDirConfig() + "\n";
    szInfo += QObject::tr("    - Configure file: ") + RabbitCommon::CDir::Instance()->GetFileApplicationConfigure() + "\n";
    szInfo += QObject::tr("    - Translations path: ") + RabbitCommon::CDir::Instance()->GetDirTranslations() + "\n";
    szInfo += QObject::tr("    - Log path: ") + RabbitCommon::CDir::Instance()->GetDirLog() + "\n";
    szInfo += QObject::tr("    - Data path: ") + RabbitCommon::CDir::Instance()->GetDirData() + "\n";
    szInfo += QObject::tr("    - Icons path: ") + RabbitCommon::CDir::Instance()->GetDirIcons() + "\n";
    szInfo += QObject::tr("    - Database path: ") + RabbitCommon::CDir::Instance()->GetDirDatabase() + "\n";
    szInfo += QObject::tr("    - Database file: ") + RabbitCommon::CDir::Instance()->GetDirDatabaseFile() + "\n";
    szInfo += QObject::tr("    - Plugins path: ") + RabbitCommon::CDir::Instance()->GetDirPlugins() + "\n";
    szInfo += QObject::tr("  - User folders and files: ") + "\n";
    szInfo += QObject::tr("    - Documents path: ") + RabbitCommon::CDir::Instance()->GetDirUserDocument() + "\n";
    szInfo += QObject::tr("    - Configure path: ") + RabbitCommon::CDir::Instance()->GetDirUserConfig() + "\n";
    szInfo += QObject::tr("    - Configure file: ") + RabbitCommon::CDir::Instance()->GetFileUserConfigure() + "\n";
    szInfo += QObject::tr("    - Data path: ") + RabbitCommon::CDir::Instance()->GetDirUserData() + "\n";
    szInfo += QObject::tr("    - Image path: ") + RabbitCommon::CDir::Instance()->GetDirUserImage() + "\n";
    szInfo += QObject::tr("    - Database path: ") + RabbitCommon::CDir::Instance()->GetDirUserDatabase() + "\n";
    szInfo += QObject::tr("    - Database file: ") + RabbitCommon::CDir::Instance()->GetDirUserDatabaseFile() + "\n";

    szInfo += QObject::tr("- Dependent libraries:") + "\n";
    szInfo += QObject::tr("  - OpenSSL:") + "\n";
#if defined(HAVE_OPENSSL)
    szInfo += "    - " + QObject::tr("RabbitCommon Build Version: ") + OPENSSL_VERSION_TEXT + "\n";
#endif
    if(QSslSocket::supportsSsl())
    {
#if (QT_VERSION >= QT_VERSION_CHECK(5, 4, 3))
        szInfo += "    - Qt " + QObject::tr("Build Version: ") + QSslSocket::sslLibraryBuildVersionString() + "\n";
#endif
        szInfo += "    - " + QObject::tr("Installed Version: ") + QSslSocket::sslLibraryVersionString() + "\n";
    } else {
#if (QT_VERSION >= QT_VERSION_CHECK(5, 4, 3))
        szInfo += "    - Qt " + QObject::tr("Build Version: ") + QSslSocket::sslLibraryBuildVersionString() + "\n";
#endif
        szInfo += "    - " + QObject::tr("Don't install OPENSSL dynamic library. Please install it") + "\n";
    }
#ifdef HAVE_CMARK_GFM
    szInfo += QObject::tr("  - cmark-gfm") + "\n";
#elif HAVE_CMARK
    szInfo += QObject::tr("  - cmark") + "\n";
#endif
#ifdef HAVE_WebEngineWidgets
    szInfo += QObject::tr("  - WebEngineWidgets") + "\n";
#endif
    return szInfo;
}

int CTools::AndroidRequestPermission(const QString &permission)
{
#if defined (Q_OS_ANDROID)
    #if QT_VERSION > QT_VERSION_CHECK(6, 0, 0)
    // get android storage permission
    // 该接口也可以直接使用字符串，可自行跳转到头文件自行查看
    
    if (QtAndroidPrivate::checkPermission(permission).result()
        == QtAndroidPrivate::Denied)
    {
        QtAndroidPrivate::requestPermission(permission).waitForFinished();
    }
    qInfo(log) << "android permission" << permission << ";status:"
                  << QtAndroidPrivate::checkPermission(permission).result();
    #endif
#endif

    return 0;
}

int CTools::AndroidRequestPermission(const QStringList &permissions)
{
    foreach (auto p, permissions) {
        AndroidRequestPermission(p);
    }
    return 0;
}

void CTools::Init(const QString szLanguage)
{
    if(m_Initialized)
        return;

    QStringList permissions;
    permissions << "android.permission.WRITE_EXTERNAL_STORAGE"
                << "android.permission.INTERNET"
                << "android.permission.ACCESS_NETWORK_STATE"
                << "android.permission.CHANGE_WIFI_STATE"
                << "android.permission.ACCESS_WIFI_STATE"
                << "android.permission.ACCESS_NETWORK_STATE"
                << "android.permission.CHANGE_NETWORK_STATE";
    AndroidRequestPermission(permissions);
    RabbitCommon::CLog::Instance();
    SetLanguage(szLanguage);
    qInfo(logTranslation) << "Language:" << szLanguage;
    InitResource();
    InstallTranslator("RabbitCommon", TranslationType::Library, szLanguage);
    //Init qt translation
    QString szQtTranslationPath;
#if defined(Q_OS_LINUX)
    szQtTranslationPath = "/usr/share/qt"
                          + QString::number(QT_VERSION_MAJOR)
                          + QDir::separator() + "translations";
    QDir d(szQtTranslationPath);
    if(!d.exists())
    {
        szQtTranslationPath = CDir::Instance()->GetDirApplication()
                              + QDir::separator() + "translations";
    }
#else
    szQtTranslationPath = CDir::Instance()->GetDirApplication()
                          + QDir::separator() + "translations";
#endif
    QStringList qtTranslations;
    qtTranslations << "qt" << "qtbase" << "qtmultimedia" << "qtwebengine"
                   << "qtlocation";
    foreach(auto f, qtTranslations) {
        QString szFile = szQtTranslationPath + QDir::separator()
                 + f + "_" + szLanguage + ".qm";
        QFile file(szFile);
        if(file.exists())
            InstallTranslatorFile(szFile);
        else
            qWarning(logTranslation) << "The file isn't exists:" << szFile;
    }
    
#ifdef HAVE_RABBITCOMMON_GUI
    CStyle::Instance()->LoadStyle();
#endif //HAVE_RABBITCOMMON_GUI
}

void CTools::Clean()
{
    foreach(auto t, m_Translator) {
        QCoreApplication::removeTranslator(t.data());
    }
    m_Translator.clear();
    CleanResource();
}

QSharedPointer<QTranslator> CTools::InstallTranslatorFile(const QString szFile)
{
    QSharedPointer<QTranslator> translator
        = QSharedPointer<QTranslator>(new QTranslator());
    if(!translator) {
        qCritical(logTranslation) << "new QTranslator fail";
        return translator;
    }
    bool bRet = translator->load(szFile);
    if(bRet)
    {
        bRet = QCoreApplication::installTranslator(translator.data());
        if(bRet)
        {
            m_Translator.push_back(translator);
            qDebug(logTranslation) << "Install translator:" << szFile;
            return translator;
        }
        else {
            qCritical(logTranslation) << "Install translator fail:" << szFile;
        }
    } else
        qCritical(logTranslation) << "Load translator file fail:" << szFile;
    return QSharedPointer<QTranslator>();
}

QSharedPointer<QTranslator> CTools::InstallTranslator(
    const QString szName,
    TranslationType type,
    const QString szPluginDir,
    const QString szLanguage)
{
    QString szTranslationName = szName;

    QString szSuffix;
    QString szPath;
    szSuffix = QDir::separator() + szTranslationName + "_" + szLanguage + ".qm";
    switch(type) {
    case TranslationType::Application:
        szPath = CDir::Instance()->GetDirTranslations();
        if(szTranslationName.isEmpty()) {
            szTranslationName = QCoreApplication::applicationName();
            szSuffix = QDir::separator() + szTranslationName + "_" + szLanguage + ".qm";
        }
        break;
    case TranslationType::Library: {
        szPath = CDir::Instance()->GetDirTranslations();
        if(szTranslationName.isEmpty()) {
            qCritical(logTranslation) << "Please set translation name";
            Q_ASSERT(false);
        }
#if defined(Q_OS_LINUX)
        QFile file(szPath + szSuffix);
        if(!file.exists()) {
            szPath = CDir::Instance()->GetDirTranslations("/usr/share");
            file.setFileName(szPath + szSuffix);
            if(!file.exists()) {
                szPath = CDir::Instance()->GetDirTranslations(
                    "/usr/local/share");
            }
        }
#endif
        break;
    }
    case TranslationType::Plugin:
        szPath = CDir::Instance()->GetDirPluginsTranslation(szPluginDir);
        if(szTranslationName.isEmpty()) {
            qCritical(logTranslation) << "Please set translation name";
            Q_ASSERT(false);
        }
        break;
    }

    QString szFile = szPath + szSuffix;
    QFile file(szFile);
    if(!file.exists())
    {
        qCritical(logTranslation) << "File isn't exit:" << szFile;
        return QSharedPointer<QTranslator>();
    }

    return InstallTranslatorFile(szFile);
}

int CTools::RemoveTranslator(QSharedPointer<QTranslator> translator)
{
    foreach(auto t, m_Translator) {
        if(t == translator) {
            QCoreApplication::removeTranslator(t.data());
            m_Translator.removeAll(t);
            return 0;
        }
    }
    return -1;
}

void CTools::InitResource()
{
    g_RabbitCommon_InitResource();
}

void CTools::CleanResource()
{
    g_RabbitCommon_CleanResource();
}

bool CTools::EnableCoreDump(bool bPrompt)
{
    Q_UNUSED(bPrompt);
#ifdef Q_OS_WIN
    //static RabbitCommon::CMiniDumper dumper(bPrompt);
    RabbitCommon::EnableMiniDumper();
    return true;
#endif
    return false;
}

bool CTools::executeByRoot(const QString &program, const QStringList &arguments)
{
#ifdef HAVE_ADMINAUTHORISER
    return CAdminAuthoriser::Instance()->execute(program, arguments);
#else
    return false;
#endif
}

int CTools::InstallStartRun(const QString &szName, const QString &szPath, bool bAllUser)
{
    Q_UNUSED(szName)
    Q_UNUSED(szPath)
    Q_UNUSED(bAllUser)
    
    QString appPath;
    appPath = QCoreApplication::applicationFilePath();

    if(!szPath.isEmpty())
        appPath = szPath;
    if(appPath.isEmpty())
    {
        qCCritical(log) << "szPath is empty";
        return -1;
    }
    if(bAllUser)
        return RabbitCommon::CRegister::InstallStartRun();
    return RabbitCommon::CRegister::InstallStartRunCurrentUser();
    //See: debian/postinst and Install/install.sh
    //Ubuntu use gnome-session-properties
    // - Current user: ~/.config/autostart
    // - All user: /etc/xdg/autostart/
    //https://blog.csdn.net/DinnerHowe/article/details/79025282
    QString szLink = CRegister::GetDesktopFileLink(szName, bAllUser);
    if(QFile::exists(szLink))
        if(RemoveStartRun(szName, bAllUser))
        {
            qCCritical(log) << "RemoveStartRun" << szName << "fail";
            return -1;
        }

    appPath = CRegister::GetDesktopFileName(szPath, szName);
    QFile f(appPath);
    if(!f.exists())
    {
        qCCritical(log) << "The desktop file is not exist." << appPath;
        return -2;
    }
    bool ret = f.link(szLink);
    if(!ret)
    {
        QString szCmd = "ln -s " + appPath + " " + szLink;
        if(!executeByRoot(szCmd))
            qCritical(log) << "CTools::InstallStartRun: file link"
                              << f.fileName() << " to " << szLink << f.error();
        return -1;
    }
    return 0;

}

int CTools::RemoveStartRun(const QString &szName, bool bAllUser)
{
    Q_UNUSED(szName);
    QString appName = QCoreApplication::applicationName();
    if(!szName.isEmpty())
        appName = szName;
    if(bAllUser)
        return RabbitCommon::CRegister::RemoveStartRun();
    return RabbitCommon::CRegister::RemoveStartRunCurrentUser();

    QString szLink = CRegister::GetDesktopFileLink(szName, bAllUser);
    if(!QFile::exists(szLink)) return 0;

    QDir d;
    if(d.remove(szLink)) return 0;
    
    QString szCmd = "rm " + szLink;
    if(executeByRoot(szCmd)) return 0;
    qCritical(log) << "execute" << szCmd << "fail";
    return -1;

}

bool CTools::IsStartRun(const QString &szName, bool bAllUser)
{
    Q_UNUSED(szName);

    if(bAllUser)
        return RabbitCommon::CRegister::IsStartRun();
    return RabbitCommon::CRegister::IsStartRunCurrentUser();

    QString szLink = CRegister::GetDesktopFileLink(szName, bAllUser);
    if(QFile::exists(szLink))
        return true;
    return false;
}

int CTools::GenerateDesktopFile(const QString &szPath,
                                const QString &szName)
{
    int nRet = 0;

    QString szFile = CRegister::GetDesktopFileName(szPath, szName);

    QString szContent;
    szContent = "[Desktop Entry]\n";
    szContent += "Name=" + QCoreApplication::applicationName() + "\n";
    szContent += "Comment=" + QCoreApplication::applicationName() + "\n";
#ifdef HAVE_RABBITCOMMON_GUI
    szContent += "Name[" + QLocale::system().name() + "]=" + QApplication::applicationDisplayName() + "\n";
    szContent += "Comment[" + QLocale::system().name() + "]=" + QApplication::applicationDisplayName() + "\n";
#else
    szContent += "Name[" + QLocale::system().name() + "]=" + QCoreApplication::applicationName() + "\n";
    szContent += "Comment[" + QLocale::system().name() + "]=" + QCoreApplication::applicationName() + "\n";
#endif
    szContent += "Icon=" + QCoreApplication::applicationName() + "\n";
    szContent += "Exec=" + QCoreApplication::applicationFilePath() + "\n";
    szContent += "Categories=Application;Development;\n";
    szContent += "Terminal=false\n";
    szContent += "StartupNotify=true\n";

    QFile f(szFile);
    if(!f.open(QFile::WriteOnly))
    {
        return f.error();
    }
    f.write(szContent.toStdString().c_str());
    f.close();
    return nRet;
}

QString CTools::GetCurrentUser()
{
#ifdef UNIX
    struct passwd *pwd;
    pwd = getpwuid(getuid());
    return pwd->pw_name;
#elif WINDOWS
    CHAR szUserName[MAX_PATH] = {0};
	DWORD dwSize=MAX_PATH;
	::GetUserNameA(szUserName, &dwSize);
    return szUserName;
    //return QString::fromWCharArray(szUserName, dwSize);
#else
    return QDir::home().dirName();
#endif
}

QString CTools::GetHostName()
{
    char buf[255];
    gethostname(buf, sizeof(buf));
    return buf;
}

#ifdef HAVE_RABBITCOMMON_GUI
QAction* CTools::AddStyleMenu(QMenu *pMenu, QWidget *parent)
{
    return pMenu->addAction(QIcon::fromTheme("style"),
                            QObject::tr("Style"),
                            [=](){
                                CFrmStyle* s = new CFrmStyle(parent);
                                if(s)
                                    s->show();
                            });
}

void CTools::InsertStyleMenu(QMenu *pMenu, QAction *before, QWidget *parent)
{
    QAction* pAction = new QAction(QIcon::fromTheme("style"),
                                   QObject::tr("Style"), parent);
    if(!pAction)
        return;
    pAction->setStatusTip(QObject::tr("Style"));
    QObject::connect(pAction, &QAction::triggered, [=](){
        CFrmStyle* s = new CFrmStyle(parent);
        if(s)
            s->show();
    });
    pMenu->insertAction(before, pAction);
}

QMenu* CTools::GetLogMenu(QWidget *parentMainWindow)
{
    QMenu* pMenu = new QMenu(QObject::tr("Log"), parentMainWindow);
    if(!pMenu) return pMenu;
    pMenu->setStatusTip(QObject::tr("Log"));
    pMenu->setIcon(QIcon::fromTheme("folder-open"));
    QAction* pAction = pMenu->addAction(QIcon::fromTheme("emblem-system"),
                     QObject::tr("Settings"),
                     [](){
                         CDlgFilter dlg;
                         QString szInclude, szExclude;
                         CLog::Instance()->GetFilter(szInclude, szExclude);
                         dlg.SetFilter(szInclude, szExclude);
                         if(QDialog::Accepted == dlg.exec())
                         {
                             dlg.GetFilter(szInclude, szExclude);
                             CLog::Instance()->SetFilter(szInclude, szExclude);
                         }
                     });
    pAction->setStatusTip(QObject::tr("Settings"));
    pAction = pMenu->addAction(QIcon::fromTheme("document-open"),
                     QObject::tr("Open Log configure file"),
                     [](){RabbitCommon::OpenLogConfigureFile();});
    pAction->setStatusTip(QObject::tr("Open Log configure file"));
    pAction = pMenu->addAction(QIcon::fromTheme("document-open"),
                     QObject::tr("Open Log file"),
                     [](){RabbitCommon::OpenLogFile();});
    pAction->setStatusTip(QObject::tr("Open Log file"));
    pAction = pMenu->addAction(QIcon::fromTheme("folder-open"),
                     QObject::tr("Open log folder"),
                     [](){RabbitCommon::OpenLogFolder();});
    pAction->setStatusTip(QObject::tr("Open Log folder"));
    QMainWindow* pMainWindow = qobject_cast<QMainWindow*>(parentMainWindow);
    if(pMainWindow) {
        if(!g_pDcokDebugLog)
            g_pDcokDebugLog = new CDockDebugLog(pMainWindow);
        pMainWindow->addDockWidget(Qt::DockWidgetArea::BottomDockWidgetArea,
                                   g_pDcokDebugLog);
        // Must set ObjectName then restore it. See: saveState help document
        g_pDcokDebugLog->setObjectName("dockDebugLog");
        QAction* pDock = g_pDcokDebugLog->toggleViewAction();
        pMenu->addAction(pDock);
        pDock->setText(QObject::tr("Log dock"));
        pDock->setIcon(QIcon::fromTheme("floating"));
        pDock->setStatusTip(QObject::tr("Log dock"));
    } else {
        qWarning(log) << "CTools::GetLogMenu: Don't use dock debug log."
                         <<  "The parent suggest is MainWindow pointer";
    }

    return pMenu;
}

int CTools::RestoreWidget(QWidget *pWidget)
{
    int nRet = 0;
    Q_ASSERT(pWidget);
    if(!pWidget) return -1;
    QSettings set(RabbitCommon::CDir::Instance()->GetFileUserConfigure(),
                  QSettings::IniFormat);
    QByteArray geometry
            = set.value("MainWindow/Status/Geometry").toByteArray();
    if(!geometry.isEmpty()) {
        if(!pWidget->restoreGeometry(geometry))
            qWarning(log) << "Restore geometry fail";
    }
    QMainWindow* pMainWindow = qobject_cast<QMainWindow*>(pWidget);
    if(pMainWindow) {
        QByteArray state = set.value("MainWindow/Status/State").toByteArray();
        if(!state.isEmpty()) {
            if(!pMainWindow->restoreState(state))
                qWarning(log) << "Restore state fail";
        }
    }
    return nRet;
}

int CTools::SaveWidget(QWidget *pWidget)
{
    int nRet = 0;
    Q_ASSERT(pWidget);
    if(!pWidget) return -1;
    QSettings set(RabbitCommon::CDir::Instance()->GetFileUserConfigure(),
                  QSettings::IniFormat);
    set.setValue("MainWindow/Status/Geometry", pWidget->saveGeometry());
    QMainWindow* pMainWindow = qobject_cast<QMainWindow*>(pWidget);
    if(pMainWindow) {
        set.setValue("MainWindow/Status/State", pMainWindow->saveState());
    }
    return nRet;
}

#endif

} //namespace RabbitCommon
