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
#include <QApplication>
#include <QIcon>
#include <QMainWindow>
#include "Style.h"
#else
#include <QCoreApplication>
#endif
#include <QLocale>
#include <QDir>
#include <QStandardPaths>

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

CTools::CTools()
{   
}

CTools::~CTools()
{
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
        szReturn = QObject::tr("Version: ") + szVersion + QObject::tr(" (From revision: ")
                + "<a href=\"http://github.com/KangLin/RabbitCommon/tree/" + szRevision + "\">"
                + szRevision + "</a>) ";
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
#endif
#if defined(HAVE_UPDATE)
    szInfo += QObject::tr("  - Have update") + "\n";
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

    return szInfo;
}

void CTools::Init(const QString szLanguage)
{
    SetLanguage(szLanguage);
    RabbitCommon::CLog::Instance();
    InitResource();
    InitTranslator(szLanguage);
    
#ifdef HAVE_RABBITCOMMON_GUI
    CStyle::Instance()->LoadStyle();
#endif //HAVE_RABBITCOMMON_GUI
}

void CTools::Clean()
{
    CleanTranslator();
    CleanResource();
}

void CTools::InitTranslator(const QString szLanguage)
{
    qInfo(Logger) << "Language:" << szLanguage;
    QString szFile = CDir::Instance()->GetDirTranslations()
            + "/RabbitCommon_" + szLanguage + ".qm";
    bool ret = m_Translator.load(szFile);
    if(ret)
    {
        ret = qApp->installTranslator(&m_Translator);
        if(!ret)
            qCritical(Logger) << "translator install translator fail:" << szFile;
    } else
        qCritical(Logger) << "translator load fail:" << szFile;

    szFile = CDir::Instance()->GetDirApplication()
            + QDir::separator() + "translations"
            + QDir::separator() + "qt_" + szLanguage + ".qm";
    ret = m_TranslatorQt.load(szFile);
    if(ret)
    {
        ret = qApp->installTranslator(&m_TranslatorQt);
        if(!ret)
            qCritical(Logger) << "Qt translator install translator fail:" << szFile;
    } else
        qCritical(Logger) << "Qt translator load fail:" << szFile;
}

void CTools::CleanTranslator()
{
    qApp->removeTranslator(&m_TranslatorQt);
    qApp->removeTranslator(&m_Translator);
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
#ifdef HAVE_RABBITCOMMON_GUI
    appPath = QApplication::applicationFilePath();
#endif
    if(!szPath.isEmpty())
        appPath = szPath;
    if(appPath.isEmpty())
    {
        qCCritical(Logger) << "szPath is empty";
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
            qCCritical(Logger) << "RemoveStartRun" << szName << "fail";
            return -1;
        }

    appPath = CRegister::GetDesktopFileName(szPath, szName);
    QFile f(appPath);
    bool ret = f.link(szLink);
    if(!ret)
    {
        QString szCmd = "ln -s " + appPath + " " + szLink;
        if(!executeByRoot(szCmd))
            qCritical(Logger) << "CTools::InstallStartRun: file link"
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
    qCritical(Logger) << "execute" << szCmd << "fail";
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
    szContent += "Name=" + qApp->applicationName() + "\n";
    szContent += "Comment=" + qApp->applicationName() + "\n";
#ifdef HAVE_RABBITCOMMON_GUI
    szContent += "Name[" + QLocale::system().name() + "]=" + qApp->applicationDisplayName() + "\n";
    szContent += "Comment[" + QLocale::system().name() + "]=" + qApp->applicationDisplayName() + "\n";
#else
    szContent += "Name[" + QLocale::system().name() + "]=" + qApp->applicationName() + "\n";
    szContent += "Comment[" + QLocale::system().name() + "]=" + qApp->applicationName() + "\n";
#endif
    szContent += "Icon=" + qApp->applicationName() + "\n";
    szContent += "Exec=" + qApp->applicationFilePath() + "\n";
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
    WCHAR szUserName[MAX_PATH] = {0};
	DWORD dwSize=MAX_PATH;
	::GetUserName(szUserName, &dwSize);
    return QString::fromWCharArray(szUserName, dwSize);
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
QMenu *CTools::GetLogMenu(QWidget *parent)
{
    QMenu* pMenu = new QMenu(QObject::tr("Log"), parent);
    if(!pMenu) return pMenu;
    pMenu->setIcon(QIcon::fromTheme("folder-open"));
    pMenu->addAction(QIcon::fromTheme("document-open"),
                     QObject::tr("Open Log configure file"),
                     [](){RabbitCommon::OpenLogConfigureFile();});
    pMenu->addAction(QIcon::fromTheme("document-open"),
                     QObject::tr("Open Log file"),
                     [](){RabbitCommon::OpenLogFile();});
    pMenu->addAction(QIcon::fromTheme("folder-open"),
                     QObject::tr("Open log folder"),
                     [](){RabbitCommon::OpenLogFolder();});
    QMainWindow* pMainWindow = qobject_cast<QMainWindow*>(parent);
    if(pMainWindow) {
        if(!g_pDcokDebugLog)
            g_pDcokDebugLog = new CDockDebugLog(pMainWindow);

        pMainWindow->addDockWidget(Qt::DockWidgetArea::BottomDockWidgetArea,
                                   g_pDcokDebugLog);
    }
    if(g_pDcokDebugLog) {
        QAction* pDock = g_pDcokDebugLog->toggleViewAction();
        pMenu->addAction(pDock);
        pDock->setText(QObject::tr("Log dock"));
        pDock->setIcon(QIcon::fromTheme("floating"));
    }

    return pMenu;
}

int CTools::RestoreWidget(QWidget *pWidget)
{
    int nRet = 0;
    QSettings set(RabbitCommon::CDir::Instance()->GetFileUserConfigure(),
                  QSettings::IniFormat);
    QByteArray geometry
            = set.value("MainWindow/Status/Geometry").toByteArray();
    if(!geometry.isEmpty())
        pWidget->restoreGeometry(geometry);
    QMainWindow* pMainWindow = qobject_cast<QMainWindow*>(pWidget);
    if(pMainWindow) {
        QByteArray state = set.value("MainWindow/Status/State").toByteArray();
        if(!state.isEmpty())
            pMainWindow->restoreState(state);
    }
    return nRet;
}

int CTools::SaveWidget(QWidget *pWidget)
{
    int nRet = 0;
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
