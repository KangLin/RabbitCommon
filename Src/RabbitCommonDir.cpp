// Copyright Copyright (c) Kang Lin studio, All Rights Reserved
// Author Kang Lin <kl222@126.com>

#include "Log/Log.h"
#include "RabbitCommonDir.h"
#include <QStandardPaths>
#include <QDir>
#include <QCoreApplication>
#include <QLoggingCategory>

namespace RabbitCommon {

static Q_LOGGING_CATEGORY(log, "RabbitCommon.Dir")

CDir::CDir()
{
    //注意这个必须的在最前  
    m_szDocumentPath = QStandardPaths::writableLocation(
             QStandardPaths::DocumentsLocation)
             + QDir::separator() + "Rabbit"
             + QDir::separator() + QCoreApplication::applicationName();
    qInfo(log) << "Document path:" << m_szDocumentPath;
    QDir d;
    if(!d.exists(m_szDocumentPath))
        if(!d.mkpath(m_szDocumentPath))
            qCritical(log) << "mkdir documents dir fail:" << m_szDocumentPath;
    
    m_szApplicationDir =  QCoreApplication::applicationDirPath();
    qInfo(log) << "Application dir:" << m_szApplicationDir;
#if defined (Q_OS_ANDROID)
    m_szApplicationInstallRootDir = "assets:";
#else
    if(m_szApplicationDir.isEmpty())
        m_szApplicationInstallRootDir = "..";
    else
        m_szApplicationInstallRootDir = m_szApplicationDir + QDir::separator() + "..";
    QDir r(m_szApplicationInstallRootDir);
    m_szApplicationInstallRootDir = r.absolutePath();
#endif
    qInfo(log) << "Application root dir:" << m_szApplicationInstallRootDir;
}

CDir* CDir::Instance()
{
    static CDir* p = nullptr;
    if(!p)
        p = new CDir;
    return p;
}

QString CDir::GetDirApplication()
{
    //qDebug(log) << "GetDirApplication:" << qApp->applicationDirPath();
    return m_szApplicationDir;
}

int CDir::SetDirApplication(const QString &szPath)
{
    m_szApplicationDir = szPath;
    return 0;
}

QString CDir::GetDirApplicationInstallRoot()
{
    return m_szApplicationInstallRootDir;
}

int CDir::SetDirApplicationInstallRoot(const QString &szPath)
{
    m_szApplicationInstallRootDir = szPath;
    return 0;
}

QString CDir::GetDirConfig(bool bReadOnly)
{
    Q_UNUSED(bReadOnly)
    QString szPath;
#if defined (Q_OS_ANDROID)
    if(bReadOnly)
        return "assets:/etc";

    szPath = GetDirUserDocument() + QDir::separator() + "root" + QDir::separator() + "etc";
    QDir d;
    if(!d.exists(szPath))
    {
        d.mkpath(szPath);
        //Todo: Copy assets:/etc to here.
        //CopyDirectory("assets:/etc", szPath);
    }
#else
    szPath = GetDirApplicationInstallRoot() + QDir::separator() + "etc";
    QDir d;
    if(!d.exists(szPath))
        d.mkpath(szPath);
#endif
    return szPath;
}

QString CDir::GetDirLog()
{
    QString szPath;
    szPath = QStandardPaths::writableLocation(QStandardPaths::TempLocation)
             + QDir::separator() + "log"
             + QDir::separator() + "Rabbit"
             + QDir::separator() + QCoreApplication::applicationName();
/*#if defined (Q_OS_ANDROID)
    szPath = GetDirUserDocument() + QDir::separator() + "root" + QDir::separator() + "log";
#elif defined(Q_OS_LINUX) || defined(Q_OS_UNIX)
    szPath = "/var/log/" + QCoreApplication::applicationName();
#else
    szPath = GetDirApplicationInstallRoot() + QDir::separator() + "log";
#endif*/
    QDir d;
    if(!d.exists(szPath))
        if(!d.mkpath(szPath))
            qCritical(log) << "Make path fail:" << szPath;
    return szPath;
}

QString CDir::GetDirData(bool bReadOnly)
{
    QString szPath;
    Q_UNUSED(bReadOnly);
#if defined (Q_OS_ANDROID)
    if(bReadOnly)
        return "assets:/share";
    szPath = GetDirUserDocument() + QDir::separator() + "root" + QDir::separator() + "share";
    QDir d;
    if(!d.exists(szPath))
    {
        d.mkpath(szPath);
        //TODO: Copy assets:/etc to here.
        //CopyDirectory("assets:/share", szPath);
    }
#else
    szPath = GetDirApplicationInstallRoot() + QDir::separator() + "share";
    QDir d;
    if(!d.exists(szPath))
        d.mkpath(szPath);
#endif
    return szPath;
}

QString CDir::GetDirDocument(QString szProjectName, bool bReadOnly)
{
    QString szPath = GetDirData(bReadOnly) + QDir::separator() + "doc";
    if(!szProjectName.isEmpty())
        szPath += QDir::separator() + szProjectName;
    return szPath;
}

QString CDir::GetDirDatabase(bool bReadOnly)
{
    QString szPath = GetDirData(bReadOnly) + QDir::separator() + "db";
    QDir d;
    if(!d.exists(szPath))
        d.mkpath(szPath);
    return szPath;
}

QString CDir::GetDirDatabaseFile(const QString &szFile, bool bReadOnly)
{
    if(szFile.isEmpty())
        return GetDirDatabase(bReadOnly) + QDir::separator() + "database.db";
    return GetDirDatabase(bReadOnly) + QDir::separator() + szFile;
}

QString CDir::GetDirApplicationXml(bool bReadOnly)
{
    QString szPath = GetDirConfig(bReadOnly) + QDir::separator() + "xml";
    QDir d;
    if(!d.exists(szPath))
        d.mkpath(szPath);
    return szPath;
}

QString CDir::GetDirPlugins(const QString &szDir)
{
    QString szPath;
#if defined (Q_OS_ANDROID)
    szPath = GetDirApplication();
#else
    szPath = GetDirApplicationInstallRoot() + QDir::separator() + "plugins";
#endif
    if(!szDir.isEmpty())
        szPath += QDir::separator() + szDir;
    return szPath;
}

QString CDir::GetDirUserDocument()
{
    return m_szDocumentPath;
}

int CDir::SetDirUserDocument(QString szPath)
{
    m_szDocumentPath = szPath;
    if(m_szDocumentPath.isEmpty())
    {
        m_szDocumentPath = QStandardPaths::writableLocation(
            QStandardPaths::DocumentsLocation)
            + QDir::separator() + "Rabbit"
            + QDir::separator() + QCoreApplication::applicationName();
    }

    QDir d;
    if(!d.exists(szPath))
        d.mkpath(szPath);
    return 0;
}

QString CDir::GetDirUserConfig()
{
    QString szPath = GetDirUserDocument() + QDir::separator() + "etc";
    
    QDir d;
    if(!d.exists(szPath))
    {
        d.mkpath(szPath);
    }
    return szPath;
}

QString CDir::GetDirUserData()
{
    QString szPath = GetDirUserDocument() + QDir::separator() + "share";
    
    QDir d;
    if(!d.exists(szPath))
    {
        d.mkpath(szPath);
    }
    return szPath;
}

QString CDir::GetDirUserDatabase()
{
    QString szPath = GetDirUserData() + QDir::separator() + "db";
    QDir d;
    if(!d.exists(szPath))
        d.mkpath(szPath);
    return szPath;
}

QString CDir::GetDirUserDatabaseFile(const QString &szFile)
{
    if(szFile.isEmpty())
        return GetDirUserDatabase() + QDir::separator() + "database.db";
    return GetDirUserDatabase() + QDir::separator() + szFile;
}

QString CDir::GetDirUserXml()
{
    QString szPath = GetDirUserData() + QDir::separator() + "xml";
    QDir d;
    if(!d.exists(szPath))
        d.mkpath(szPath);
    return szPath;
}

QString CDir::GetDirUserImage()
{
    QString szPath = GetDirUserData() + QDir::separator() + "image";
    QDir d;
    if(!d.exists(szPath))
        d.mkpath(szPath);
    return szPath;
}

QString CDir::GetDirTranslations(QString szPrefix)
{
#if defined(_DEBUG) || defined(DEBUG)
    return ":/share/translations";
#endif
    QString szPath(szPrefix);
    if(szPath.isEmpty())
    {
        szPath = GetDirData(true);
    }
    return szPath + QDir::separator() + "translations";
}

QString CDir::GetDirIcons(bool bReadOnly)
{
    return GetDirData(bReadOnly) + QDir::separator() + "icons";
}

QString CDir::GetDirPluginsTranslation(QString szDir)
{
#if defined(_DEBUG) || defined(DEBUG)
    return ":/" + szDir + "/translations";
#elif defined (Q_OS_ANDROID)
    return "assets:/" + szDir + "/translations";
#endif
    
    return GetDirApplicationInstallRoot() + QDir::separator()
            + szDir + QDir::separator() + "translations";
}

QString CDir::GetFileApplicationConfigure(bool bReadOnly)
{
    return GetDirConfig(bReadOnly) + QDir::separator()
            + QCoreApplication::applicationName() + ".conf";
}

QString CDir::GetFileUserConfigure()
{
    QString szName = GetDirUserDocument() + QDir::separator()
            + QCoreApplication::applicationName() + ".conf";
    return szName;
}

int CDir::CopyDirectory(const QString &fromDir, const QString &toDir, bool bCoverIfFileExists)
{
    QDir formDir_(fromDir);
    QDir toDir_(toDir);
 
    if(!toDir_.exists())
    {
        if(!toDir_.mkpath(toDir_.absolutePath()))
            return false;
    }
 
    QFileInfoList fileInfoList = formDir_.entryInfoList();
    foreach(QFileInfo fileInfo, fileInfoList)
    {
        if(fileInfo.fileName() == "."|| fileInfo.fileName() == "..")
            continue;
 
        if(fileInfo.isDir())
        {
            if(!CopyDirectory(fileInfo.filePath(),
                              toDir_.filePath(fileInfo.fileName()),
                              true))
                return false;
        }
        else
        {
            if(bCoverIfFileExists && toDir_.exists(fileInfo.fileName()))
            {
                toDir_.remove(fileInfo.fileName());
            }
            if(!QFile::copy(fileInfo.filePath(), toDir_.filePath(fileInfo.fileName())))
            {
                return false;
            }
        }
    }
    return true;
}

} //namespace RabbitCommon
