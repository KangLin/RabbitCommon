// Copyright Copyright (c) Kang Lin studio, All Rights Reserved
// Author Kang Lin <kl222@126.com>

#include "RabbitCommonTools.h"
#include "RabbitCommonDir.h"
#ifdef HAVE_ADMINAUTHORISER
#include "AdminAuthoriser/adminauthoriser.h"
#endif
#include "RabbitCommonRegister.h"
#include "RabbitCommonLog.h"

#include <QSettings>
#ifdef HAVE_RABBITCOMMON_GUI
#include <QApplication>
#include <QIcon>
#include <QMainWindow>
#include "RabbitCommonStyle.h"
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
    return RabbitCommon_VERSION;
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

    QString appPath = QApplication::applicationFilePath();
    if(!szPath.isEmpty())
        appPath = szPath;
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

void CTools::OpenLogConfigureFile()
{
#ifdef HAVE_RABBITCOMMON_GUI
    RabbitCommon::OpenLogConfigureFile();
#endif
}

void CTools::OpenLogFile()
{
#ifdef HAVE_RABBITCOMMON_GUI
    RabbitCommon::OpenLogFile();
#endif
}

void CTools::OpenLogFolder()
{
#ifdef HAVE_RABBITCOMMON_GUI
    RabbitCommon::OpenLogFolder();
#endif
}

#ifdef HAVE_RABBITCOMMON_GUI
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
