#include "RabbitCommonTools.h"
#include "RabbitCommonDir.h"
#include "AdminAuthoriser/adminauthoriser.h"
#include "RabbitCommonRegister.h"

#include <QApplication>
#include <QLocale>
#include <QDir>
#include <QDebug>
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

inline void g_RabbitCommon_InitResource()
{
    Q_INIT_RESOURCE(ResourceRabbitCommon);
#if _DEBUG
    Q_INIT_RESOURCE(translations_RabbitCommon);
#endif
}

inline void g_RabbitCommon_CleanResource()
{
    Q_CLEANUP_RESOURCE(ResourceRabbitCommon);
#if _DEBUG
    Q_CLEANUP_RESOURCE(translations_RabbitCommon);
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

void CTools::InitTranslator(const QString szLanguage)
{
    m_Translator.load(CDir::Instance()->GetDirTranslations()
                      + "/RabbitCommon_" + szLanguage + ".qm");
    qApp->installTranslator(&m_Translator);
}

void CTools::CleanTranslator()
{
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

bool CTools::executeByRoot(const QString &program, const QStringList &arguments)
{
    return CAdminAuthoriser::Instance()->execute(program, arguments);
}


int CTools::InstallStartRun(const QString &szName, const QString &szPath, bool bAllUser)
{
    Q_UNUSED(szName)
    Q_UNUSED(szPath)
    Q_UNUSED(bAllUser)
    QString appName = QApplication::applicationName();
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
            qCritical() << "CTools::InstallStartRun: file link " << f.fileName() << "to " << szLink << f.error();
        return -1;
    }
    return 0;
#endif
}

int CTools::RemoveStartRun(const QString &szName, bool bAllUser)
{
    Q_UNUSED(szName);
    QString appName = QApplication::applicationName();
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
            qCritical() << "CTools::RemoveStartRun: Remove" << szLink << "fail";
        return -1;
    }
    return 0;
#endif
}

bool CTools::IsStartRun(const QString &szName, bool bAllUser)
{
    Q_UNUSED(szName);
    QString appName = QApplication::applicationName();
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
    
    //qDebug() << "CTools::IsStartRun: Open" << f.fileName() <<  "file fail" << f.error() << f.errorString();
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
    szContent += "Name[" + QLocale::system().name() + "]=" + qApp->applicationDisplayName() + "\n";
    szContent += "Comment[" + QLocale::system().name() + "]=" + qApp->applicationDisplayName() + "\n";
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

} //namespace RabbitCommon
