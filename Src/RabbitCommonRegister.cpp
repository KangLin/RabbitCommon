//! @copyright Copyright (c) Kang Lin studio, All Rights Reserved
//! @author Kang Lin(kl222@126.com)

#include "RabbitCommonRegister.h"

#include <QSettings>
#include <QApplication>
#include <QDir>
#include <QDebug>

namespace RabbitCommon {

const QString gCurrentUserRun = "HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run";
const QString gCurrentUserRunOnce = "HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\RunOnce";
const QString gCurrentUserRunServices = "HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\RunServices";
const QString gCurrentUserRunServicesOnce = "HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\RunServicesOnce";

const QString gRun = "HKEY_LOCAL_MACHINE\\Software\\Microsoft\\Windows\\CurrentVersion\\Run";
const QString gRunOnce = "HKEY_LOCAL_MACHINE\\Software\\Microsoft\\Windows\\CurrentVersion\\RunOnce";
const QString gRunServices = "HKEY_LOCAL_MACHINE\\Software\\Microsoft\\Windows\\CurrentVersion\\RunServices";
const QString gRunServicesOnce = "HKEY_LOCAL_MACHINE\\Software\\Microsoft\\Windows\\CurrentVersion\\RunServicesOnce";

CRegister::CRegister()
{
}

CRegister::~CRegister() {}

int CRegister::InstallStartRunCurrentUser()
{
    return InstallStartRun(gCurrentUserRun);
}

int CRegister::RemoveStartRunCurrentUser()
{
    return RemoveStartRun(gCurrentUserRun);
}

bool CRegister::IsStartRunCurrentUser()
{
    return IsStartRun(gCurrentUserRun);
}

int CRegister::InstallStartRunOnceCurrentUser()
{
    return InstallStartRun(gCurrentUserRunOnce);
}

int CRegister::RemoveStartRunOnceCurrentUser()
{
    return RemoveStartRun(gCurrentUserRunOnce);
}

bool CRegister::IsStartRunOnceCurrentUser()
{
    return IsStartRun(gCurrentUserRunOnce);
}

int CRegister::InstallStartRunServicesCurrentUser()
{
    return InstallStartRun(gCurrentUserRunServices);
}

int CRegister::RemoveStartRunServicesCurrentUser()
{
    return RemoveStartRun(gCurrentUserRunServices);
}

bool CRegister::IsStartRunServicesCurrentUser()
{
    return IsStartRun(gCurrentUserRunServices);
}

int CRegister::InstallStartRunServicesOnceCurrentUser()
{
    return InstallStartRun(gCurrentUserRunServicesOnce);
}

int CRegister::RemoveStartRunServicesOnceCurrentUser()
{
    return RemoveStartRun(gCurrentUserRunServicesOnce);
}

bool CRegister::IsStartRunServicesOnceCurrentUser()
{
    return IsStartRun(gCurrentUserRunServicesOnce);
}

int CRegister::InstallStartRun()
{
    return InstallStartRun(gRun);
}

int CRegister::RemoveStartRun()
{
    return RemoveStartRun(gRun);
}

bool CRegister::IsStartRun()
{
    return IsStartRun(gRun);
}

int CRegister::InstallStartRunOnce()
{
    return InstallStartRun(gRunOnce);
}

int CRegister::RemoveStartRunOnce()
{
    return RemoveStartRun(gRunOnce);
}

bool CRegister::IsStartRunOnce()
{
    return IsStartRun(gRunOnce);
}

int CRegister::InstallStartRunServices()
{
    return InstallStartRun(gRunServices);
}

int CRegister::RemoveStartRunServices()
{
    return RemoveStartRun(gRunServices);
}

bool CRegister::IsStartRunServices()
{
    return IsStartRun(gRunServices);
}

int CRegister::InstallStartRunServicesOnce()
{
    return InstallStartRun(gRunServicesOnce);
}

int CRegister::RemoveStartRunServicesOnce()
{
    return RemoveStartRun(gRunServicesOnce);
}

bool CRegister::IsStartRunServicesOnce()
{
    return IsStartRun(gRunServicesOnce);
}

int CRegister::InstallStartRun(const QString &szReg, const QString &szName, const QString &szPath)
{
    Q_UNUSED(szReg);
    Q_UNUSED(szName);
    Q_UNUSED(szPath);
    
    QString appName = QApplication::applicationName();
#if defined (Q_OS_WIN)
    QString appPath = QApplication::applicationFilePath();
    if(!szName.isEmpty())
        appName = szName;
    if(!szPath.isEmpty())
        appPath = szPath;
    appPath = appPath.replace("/", "\\");
    return SetRegister(szReg, appName, appPath);
#elif defined(Q_OS_ANDROID)
    
    return 0;
#elif defined(Q_OS_UNIX)
    //See: debian/postinst
    QFile f("/opt/" + appName + "/share/applications/" + appName +".desktop");
    bool ret = f.link("/etc/xdg/autostart/" + appName + ".desktop");
    if(!ret)
    {
        qDebug() << "file link " << f.fileName() << "to /etc/xdg/autostart/" + appName + ".desktop:" << f.error();
        return -1;
    }
    return 0;
#endif
}

int CRegister::RemoveStartRun(const QString &szReg, const QString &szName)
{
    Q_UNUSED(szReg);
    Q_UNUSED(szName);
    QString appName = QApplication::applicationName();
#if defined (Q_OS_WIN)
    if(!szName.isEmpty())
        appName = szName;
    return RemoveRegister(szReg, appName);
#elif defined(Q_OS_ANDROID)
    
    return 0;
#elif defined(Q_OS_UNIX)
    QDir d;
    if(d.remove("/etc/xdg/autostart/" + appName + ".desktop"))
        return 0;
    return -1;
#endif
}

bool CRegister::IsStartRun(const QString &szReg, const QString &szName)
{
    Q_UNUSED(szReg);
    Q_UNUSED(szName);
    QString appName = QApplication::applicationName();
#if defined (Q_OS_WIN)
    if(!szName.isEmpty())
        appName = szName;
    return IsRegister(szReg, appName);
#elif defined(Q_OS_ANDROID)
    
    return false;
#elif defined(Q_OS_UNIX)
    QFile f("/etc/xdg/autostart/" + appName + ".desktop");
    if(f.open(QFile::ReadOnly))
    {
        f.close();
        return true;
    }
    return false;
#endif
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

} //namespace RabbitCommon 
