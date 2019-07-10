#include "RabbitCommonDir.h"
#include <QStandardPaths>
#include <QDir>
#include <QApplication>
#include <QDebug>

namespace RabbitCommon {

CDir::CDir()
{
    //注意这个必须的在最前  
    m_szDocumentPath =  QStandardPaths::writableLocation(
             QStandardPaths::DocumentsLocation) 
             + QDir::separator() + "Rabbit"
             + QDir::separator() + QApplication::applicationName();
    QDir d;
    if(!d.exists(m_szDocumentPath))
        d.mkpath(m_szDocumentPath);
    
    m_szApplicationDir =  qApp->applicationDirPath();
    m_szApplicationRootDir = m_szApplicationDir + QDir::separator() + "..";
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
    qDebug() << "GetDirApplication:" << qApp->applicationDirPath();
    return m_szApplicationDir;
}

int CDir::SetDirApplication(const QString &szPath)
{
    m_szApplicationDir = szPath;
    return 0;
}

QString CDir::GetDirApplicationInstallRoot()
{
    return m_szApplicationRootDir;
}

int CDir::SetDirApplicationInstallRoot(const QString &szPath)
{
    m_szApplicationRootDir = szPath;
    return 0;
}

QString CDir::GetDirConfig()
{
    QString szPath;
#if defined (Q_OS_ANDROID)
    szPath = GetDirUserDocument() + QDir::separator() + "etc";
    QDir d;
    if(!d.exists(szPath))
    {
        d.mkpath(szPath);
        //Copy assets:/etc to here.
        CopyDirectory("assets:/etc", szPath);
    }
#else
    szPath = GetDirApplicationInstallRoot() + QDir::separator() + "etc";
    QDir d;
    if(!d.exists(szPath))
        d.mkpath(szPath);
#endif
    return szPath;
}

QString CDir::GetDirApplicationXml()
{
    QString szPath = GetDirConfig() + QDir::separator() + "xml";
    QDir d;
    if(!d.exists(szPath))
        d.mkpath(szPath);
    return szPath;
}

QString CDir::GetDirUserDocument()
{
    return m_szDocumentPath;
}

int CDir::SetDirUserDocument(QString szPath)
{
    m_szDocumentPath = szPath;
    QDir d;
    if(!d.exists(szPath))
        d.mkpath(szPath);
    return 0;
}

QString CDir::GetDirUserData()
{
    QString szPath = GetDirUserDocument() + QDir::separator() + "data";
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

QString CDir::GetDirTranslations()
{
#if _DEBUG
    return ":/translations";
#elif defined(Q_OS_ANDROID)
    return "assets:/translations";
#endif
    return GetDirApplicationInstallRoot() + QDir::separator() + "translations";
}

QString CDir::GetFileApplicationConfigure()
{
    return GetDirConfig() + QDir::separator() + QApplication::applicationName() + ".conf";
}

QString CDir::GetFileUserConfigure()
{
    QString szName = GetDirUserDocument() + QDir::separator() + QApplication::applicationName() + ".conf";
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
