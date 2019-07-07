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
    QString szPath = GetDirApplicationInstallRoot() + QDir::separator() + "etc";
    QDir d;
    if(!d.exists(szPath))
        d.mkpath(szPath);
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
#if defined(Q_OS_ANDROID) || _DEBUG
    return ":/Translations";
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

} //namespace RabbitCommon
