// Copyright Copyright (c) Kang Lin studio, All Rights Reserved
// Author Kang Lin <kl222@126.com>

#include "RabbitCommonRegister.h"
#include "RabbitCommonDir.h"
#include "Log.h"
#include "RabbitCommonTools.h"

#include <QSettings>
#include <QCoreApplication>
#include <QDir>
#include <QStandardPaths>

namespace RabbitCommon {

const QString gKeyCurrentUserRun = "HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run";
const QString gKeyCurrentUserRunOnce = "HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\RunOnce";
const QString gKeyCurrentUserRunServices = "HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\RunServices";
const QString gKeyCurrentUserRunServicesOnce = "HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\RunServicesOnce";

const QString gKeyRun = "HKEY_LOCAL_MACHINE\\Software\\Microsoft\\Windows\\CurrentVersion\\Run";
const QString gKeyRunOnce = "HKEY_LOCAL_MACHINE\\Software\\Microsoft\\Windows\\CurrentVersion\\RunOnce";
const QString gKeyRunServices = "HKEY_LOCAL_MACHINE\\Software\\Microsoft\\Windows\\CurrentVersion\\RunServices";
const QString gKeyRunServicesOnce = "HKEY_LOCAL_MACHINE\\Software\\Microsoft\\Windows\\CurrentVersion\\RunServicesOnce";

CRegister::CRegister()
{
}

CRegister::~CRegister() {}

int CRegister::InstallStartRunCurrentUser(QString szName, QString szPath)
{
#if defined (Q_OS_WIN)
    return InstallStartRun(gKeyCurrentUserRun, szName, szPath);
#elif defined(Q_OS_ANDROID)
    //TODO: See https://www.cnblogs.com/jetereting/p/4572302.html

    return 0;
#elif defined(Q_OS_UNIX)
    //See: debian/postinst and Install/install.sh
    //Ubuntu use gnome-session-properties
    // - Current user: ~/.config/autostart
    // - All user: /etc/xdg/autostart/
    //https://blog.csdn.net/DinnerHowe/article/details/79025282
    QString szLink = GetDesktopFileLink(szName, false);
    if(QFile::exists(szLink))
        if(CTools::RemoveStartRun(szName, false))
        {
            qCCritical(Logger) << "RemoveStartRun" << szName << "fail";
            return -1;
        }

    QString appPath = GetDesktopFileName(szPath, szName);
    QFile f(appPath);
    bool ret = f.link(szLink);
    if(ret) return 0;

    qDebug(Logger) << "f.link fail:" << f.error() << f.errorString();
    if(QFileDevice::RenameError == f.error()) return 0;

    QString szCmd = "ln -s " + appPath + " " + szLink;
    if(CTools::executeByRoot(szCmd)) return 0;
    qCritical(Logger) << "execute" << szCmd << "fail";
    return -1;
#endif
}

int CRegister::RemoveStartRunCurrentUser(QString szName)
{
#if defined (Q_OS_WIN)
    return RemoveStartRun(gKeyCurrentUserRun, szName);
#elif defined(Q_OS_ANDROID)
    
    return 0;
#elif defined(Q_OS_UNIX)
    QString szLink = GetDesktopFileLink(szName, false);
    if(!QFile::exists(szLink)) return 0;

    QDir d;
    if(d.remove(szLink)) return 0;
    
    QString szCmd = "rm " + szLink;
    if(CTools::executeByRoot(szCmd)) return 0;
    qCritical(Logger) << "execute" << szCmd << "fail";
    return -1;
#endif
}

bool CRegister::IsStartRunCurrentUser(QString szName)
{
#if defined (Q_OS_WIN)
    return IsStartRun(gKeyCurrentUserRun, szName);
#elif defined(Q_OS_ANDROID)
    
    return false;
#elif defined(Q_OS_UNIX)
    QString szLink = GetDesktopFileLink(szName);
    if(QFile::exists(szLink))
        return true;
    return false;
#endif
}

int CRegister::InstallStartRunOnceCurrentUser(QString szName, QString szPath)
{
    return InstallStartRun(gKeyCurrentUserRunOnce, szName, szPath);
}

int CRegister::RemoveStartRunOnceCurrentUser(QString szName)
{
    return RemoveStartRun(gKeyCurrentUserRunOnce, szName);
}

bool CRegister::IsStartRunOnceCurrentUser(QString szName)
{
    return IsStartRun(gKeyCurrentUserRunOnce, szName);
}

int CRegister::InstallStartRunServicesCurrentUser(QString szName, QString szPath)
{
    return InstallStartRun(gKeyCurrentUserRunServices, szName, szPath);
}

int CRegister::RemoveStartRunServicesCurrentUser(QString szName)
{
    return RemoveStartRun(gKeyCurrentUserRunServices, szName);
}

bool CRegister::IsStartRunServicesCurrentUser(QString szName)
{
    return IsStartRun(gKeyCurrentUserRunServices, szName);
}

int CRegister::InstallStartRunServicesOnceCurrentUser(QString szName, QString szPath)
{
    return InstallStartRun(gKeyCurrentUserRunServicesOnce, szName, szPath);
}

int CRegister::RemoveStartRunServicesOnceCurrentUser(QString szName)
{
    return RemoveStartRun(gKeyCurrentUserRunServicesOnce, szName);
}

bool CRegister::IsStartRunServicesOnceCurrentUser(QString szName)
{
    return IsStartRun(gKeyCurrentUserRunServicesOnce, szName);
}

int CRegister::InstallStartRun(QString szName, QString szPath)
{
#if defined (Q_OS_WIN)
    return InstallStartRun(gKeyRun, szName, szPath);
#elif defined(Q_OS_ANDROID)
    //TODO: See https://www.cnblogs.com/jetereting/p/4572302.html

    return 0;
#elif defined(Q_OS_UNIX)
    //See: debian/postinst and Install/install.sh
    //Ubuntu use gnome-session-properties
    // - Current user: ~/.config/autostart
    // - All user: /etc/xdg/autostart/
    //https://blog.csdn.net/DinnerHowe/article/details/79025282
    QString szLink = GetDesktopFileLink(szName, true);
    if(QFile::exists(szLink))
        if(CTools::RemoveStartRun(szName, true))
        {
            qCCritical(Logger) << "RemoveStartRun" << szName << "fail";
            return -1;
        }

    QString appPath = GetDesktopFileName(szPath, szName);
    QFile f(appPath);
    bool ret = f.link(szLink);
    if(!ret)
    {
        QString szCmd = "ln -s " + appPath + " " + szLink;
        if(!CTools::executeByRoot(szCmd))
            qCritical(Logger) << "CTools::InstallStartRun: file link"
                              << f.fileName() << " to " << szLink << f.error();
        return -1;
    }
    return 0;
#endif
}

int CRegister::RemoveStartRun(const QString &szName)
{
#if defined (Q_OS_WIN)
    return RemoveStartRun(gKeyRun, szName);
#elif defined(Q_OS_ANDROID)
    
    return 0;
#elif defined(Q_OS_UNIX)
    QString szLink = CRegister::GetDesktopFileLink(szName, true);
    if(!QFile::exists(szLink)) return 0;

    QDir d;
    if(d.remove(szLink)) return 0;
    
    QString szCmd = "rm " + szLink;
    if(CTools::executeByRoot(szCmd)) return 0;
    qCritical(Logger) << "execute" << szCmd << "fail";
    return -1;
#endif
}

bool CRegister::IsStartRun(const QString &szName)
{
#if defined (Q_OS_WIN)
    return IsStartRun(gKeyRun, szName);
#elif defined(Q_OS_ANDROID)
    
    return false;
#elif defined(Q_OS_UNIX)
    QString szLink = GetDesktopFileLink(szName, true);
    if(QFile::exists(szLink))
        return true;
    return false;
#endif
}

int CRegister::InstallStartRunOnce(QString szName, QString szPath)
{
    return InstallStartRun(gKeyRunOnce, szName, szPath);
}

int CRegister::RemoveStartRunOnce(QString szName)
{
    return RemoveStartRun(gKeyRunOnce, szName);
}

bool CRegister::IsStartRunOnce(QString szName)
{
    return IsStartRun(gKeyRunOnce, szName);
}

int CRegister::InstallStartRunServices(QString szName, QString szPath)
{
    return InstallStartRun(gKeyRunServices, szName, szPath);
}

int CRegister::RemoveStartRunServices(QString szName)
{
    return RemoveStartRun(gKeyRunServices, szName);
}

bool CRegister::IsStartRunServices(QString szName)
{
    return IsStartRun(gKeyRunServices, szName);
}

int CRegister::InstallStartRunServicesOnce(QString szName, QString szPath)
{
    return InstallStartRun(gKeyRunServicesOnce, szName, szPath);
}

int CRegister::RemoveStartRunServicesOnce(QString szName)
{
    return RemoveStartRun(gKeyRunServicesOnce, szName);
}

bool CRegister::IsStartRunServicesOnce(QString szName)
{
    return IsStartRun(gKeyRunServicesOnce, szName);
}

int CRegister::InstallStartRun(const QString &szKey, const QString &szName, const QString &szPath)
{
    Q_UNUSED(szKey);
    Q_UNUSED(szName);
    Q_UNUSED(szPath);

    QString appName = QCoreApplication::applicationName();
    QString appPath = QCoreApplication::applicationFilePath();
    if(!szName.isEmpty())
        appName = szName;
    if(!szPath.isEmpty())
        appPath = szPath;
    appPath = appPath.replace("/", "\\");
    return SetRegister(szKey, appName, appPath);
}

int CRegister::RemoveStartRun(const QString &szKey, const QString &szName)
{
    Q_UNUSED(szKey);
    Q_UNUSED(szName);
    QString appName = QCoreApplication::applicationName();
    if(!szName.isEmpty())
        appName = szName;
    return RemoveRegister(szKey, appName);
}

bool CRegister::IsStartRun(const QString &szKey, const QString &szName)
{
    Q_UNUSED(szKey);
    Q_UNUSED(szName);
    QString appName = QCoreApplication::applicationName();

    if(!szName.isEmpty())
        appName = szName;
    return IsRegister(szKey, appName);
}

int CRegister::SetRegister(const QString &reg, const QString &name, const QString &path)
{
    QSettings r(reg, QSettings::NativeFormat);
    QString val = r.value(name).toString();
    if(val != path)
        r.setValue(name, path);
    return 0;
}

int CRegister::RemoveRegister(const QString &reg, const QString &name)
{
    QSettings r(reg, QSettings::NativeFormat);
    QString val = r.value(name).toString();
    if(!val.isEmpty())
        r.remove(name);
    return 0;
}

bool CRegister::IsRegister(const QString &reg, const QString &name)
{
    QSettings r(reg, QSettings::NativeFormat);
    QString val = r.value(name).toString();
    return !val.isEmpty();
}

QString CRegister::GetDesktopFileName(const QString &szPath,
                                   const QString &szName)
{
    QString appName = "org.Rabbit." + qApp->applicationName() + ".desktop";
    if(!szName.isEmpty())
        appName = szName;

    QString appPath = CDir::Instance()->GetDirApplicationInstallRoot()
            + "/share/applications";
    if(!szPath.isEmpty())
        appPath = szPath;

    return appPath + QDir::separator() + appName;
}

QString CRegister::GetDesktopFileLink(const QString &szName, bool bAllUser)
{
    QString appName 
            = "org.Rabbit." + QCoreApplication::applicationName() +".desktop";
    if(!szName.isEmpty()) appName = szName;
    QString szLink;
    if(bAllUser)
    {
        szLink = "/etc/xdg";
    } else {
        szLink = QStandardPaths::writableLocation(QStandardPaths::HomeLocation)
                    + "/.config";
    }
    szLink += "/autostart/" + appName;
    return szLink;
}


} //namespace RabbitCommon 
