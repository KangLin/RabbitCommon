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
#ifdef HAVE_GUI
#include <QApplication>
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
    // Must set cmake paramter: -DCMAKE_BUILD_TYPE=Debug
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

void CTools::Init(const QString szLanguage)
{
    SetLanguage(szLanguage);
    InitResource();
    InitTranslator(szLanguage);
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
        qCritical(Logger) << "translator load fail: %s" << szFile;

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
        qCritical(Logger) << "Qt translator load fail: %s" << szFile;
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
    QString appName = QCoreApplication::applicationName();
    if(!szName.isEmpty())
        appName = szName;
#if defined (Q_OS_WIN)
    QString appPath = QApplication::applicationFilePath();
    if(!szPath.isEmpty())
        appPath = szPath;
    if(bAllUser)
        return RabbitCommon::CRegister::InstallStartRun();
    return RabbitCommon::CRegister::InstallStartRunCurrentUser();
#elif defined(Q_OS_ANDROID)
    //TODO: See https://www.cnblogs.com/jetereting/p/4572302.html
    
    return 0;
#elif defined(Q_OS_UNIX)
    //See: debian/postinst and Install/install.sh
    //Ubuntu use gnome-session-properties
    // ~/.config/autostart 和 /etc/xdg/autostart/
    //https://blog.csdn.net/DinnerHowe/article/details/79025282
    QString szDesktop = "/opt/" + appName + "/share/applications/" + appName +".desktop";
    QString szLink;
    if(bAllUser)
    {
        szLink = "/etc/xdg";
    } else {
        szLink = QStandardPaths::writableLocation(QStandardPaths::HomeLocation)
                    + "/.config";
    }
    szLink += "/autostart/" + appName + ".desktop";

    QDir d;
    if(d.exists(szLink))
        RemoveStartRun(szName, bAllUser);

    QFile f(szDesktop);
    bool ret = f.link(szLink);
    if(!ret)
    {
        QString szCmd = "ln -s " + szDesktop + " " + szLink;
        if(!executeByRoot(szCmd))
            qCritical(Logger) << "CTools::InstallStartRun: file link"
                              << f.fileName() << " to " << szLink << f.error();
        return -1;
    }
    return 0;
#endif
}

int CTools::RemoveStartRun(const QString &szName, bool bAllUser)
{
    Q_UNUSED(szName);
    QString appName = QCoreApplication::applicationName();
    if(!szName.isEmpty())
        appName = szName;
#if defined (Q_OS_WIN)
    if(bAllUser)
        return RabbitCommon::CRegister::RemoveStartRun();
    return RabbitCommon::CRegister::RemoveStartRunCurrentUser();
#elif defined(Q_OS_ANDROID)
    
    return 0;
#elif defined(Q_OS_UNIX)
    QString szLink;
    if(bAllUser)
    {
        szLink = "/etc/xdg";
    } else {
        szLink = QStandardPaths::writableLocation(QStandardPaths::HomeLocation)
                    + "/.config";
    }
    szLink += "/autostart/" + appName + ".desktop";
    QDir d;
    if(d.exists(szLink))
    {
        if(d.remove(szLink))
            return 0;

        QString szCmd = "rm " + szLink;
        if(!executeByRoot(szCmd))
            qCritical(Logger) << "CTools::RemoveStartRun: Remove" << szLink << "fail";
        return -1;
    }
    return 0;
#endif
}

bool CTools::IsStartRun(const QString &szName, bool bAllUser)
{
    Q_UNUSED(szName);
    QString appName = QCoreApplication::applicationName();
    if(!szName.isEmpty())
        appName = szName;

#if defined (Q_OS_WIN)
    if(bAllUser)
        return RabbitCommon::CRegister::IsStartRun();
    return RabbitCommon::CRegister::IsStartRunCurrentUser();
#elif defined(Q_OS_ANDROID)
    
    return false;
#elif defined(Q_OS_UNIX)
    QString szLink;
    if(bAllUser)
    {
        szLink = "/etc/xdg";
    } else {
        szLink = QStandardPaths::writableLocation(QStandardPaths::HomeLocation)
                    + "/.config";
    }
    szLink += "/autostart/" + appName + ".desktop";
    QFileInfo fi(szLink);
    QFile f(fi.absoluteFilePath());
    if(f.open(QFile::ReadOnly))
    {
        f.close();
        return true;
    }
    
    //qDebug(Logger) << "CTools::IsStartRun: Open" << f.fileName() <<  "file fail" << f.error() << f.errorString();
    return false;
#endif
}

int CTools::GenerateDesktopFile(const QString &szPath,
                                const QString &szAppName)
{
    int nRet = 0;
    QString szFile = "/usr/share/applications";
    if(!szPath.isEmpty())
        szFile = szPath;

    QString szName = qApp->applicationName();
    if(!szAppName.isEmpty())
        szName = szAppName;

    szFile += QDir::separator() + szName + ".desktop";

    QString szContent;
    szContent = "[Desktop Entry]\n";
    szContent += "Name=" + qApp->applicationName() + "\n";
    szContent += "Comment=" + qApp->applicationName() + "\n";
#ifdef HAVE_GUI
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

} //namespace RabbitCommon
