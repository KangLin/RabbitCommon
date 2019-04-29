#include "RabbitCommonGlobalDir.h"
#include <QStandardPaths>
#include <QDir>
#include <QApplication>
#include <QDebug>

CRabbitCommonGlobalDir::CRabbitCommonGlobalDir()
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

CRabbitCommonGlobalDir* CRabbitCommonGlobalDir::Instance()
{
    static CRabbitCommonGlobalDir* p = nullptr;
    if(!p)
        p = new CRabbitCommonGlobalDir;
    return p;
}

QString CRabbitCommonGlobalDir::GetDirApplication()
{
    qDebug() << "GetDirApplication:" << qApp->applicationDirPath().toStdString().c_str();
    return m_szApplicationDir;
}

int CRabbitCommonGlobalDir::SetDirApplication(const QString &szPath)
{
    m_szApplicationDir = szPath;
    return 0;
}

QString CRabbitCommonGlobalDir::GetDirApplicationRoot()
{
    return m_szApplicationRootDir;
}

int CRabbitCommonGlobalDir::SetDirApplicationRoot(const QString &szPath)
{
    m_szApplicationRootDir = szPath;
    return 0;
}

QString CRabbitCommonGlobalDir::GetDirConfig()
{
    QString szPath = GetDirApplicationRoot() + QDir::separator() + "etc";
    QDir d;
    if(!d.exists(szPath))
        d.mkpath(szPath);
    return szPath;
}

QString CRabbitCommonGlobalDir::GetDirApplicationXml()
{
    QString szPath = GetDirConfig() + QDir::separator() + "xml";
    QDir d;
    if(!d.exists(szPath))
        d.mkpath(szPath);
    return szPath;
}

QString CRabbitCommonGlobalDir::GetDirUserDocument()
{
    return m_szDocumentPath;
}

int CRabbitCommonGlobalDir::SetDirUserDocument(QString szPath)
{
    m_szDocumentPath = szPath;
    QDir d;
    if(!d.exists(szPath))
        d.mkpath(szPath);
    return 0;
}

QString CRabbitCommonGlobalDir::GetDirUserData()
{
    QString szPath = GetDirUserDocument() + QDir::separator() + "data";
    QDir d;
    if(!d.exists(szPath))
        d.mkpath(szPath);
    return szPath;
}

QString CRabbitCommonGlobalDir::GetDirUserImage()
{
    QString szPath = GetDirUserData() + QDir::separator() + "image";
    QDir d;
    if(!d.exists(szPath))
        d.mkpath(szPath);
    return szPath;
}

QString CRabbitCommonGlobalDir::GetDirTranslations()
{
#if defined(Q_OS_ANDROID) || _DEBUG
    return ":/Translations";
#endif
    return GetDirApplicationRoot() + QDir::separator() + "translations";
}

QString CRabbitCommonGlobalDir::GetApplicationConfigureFile()
{
    return GetDirConfig() + QDir::separator() + QApplication::applicationName() + ".conf";
}

QString CRabbitCommonGlobalDir::GetFileUserConfigure()
{
    QString szName = GetDirUserDocument() + QDir::separator() + QApplication::applicationName() + ".conf";
    return szName;
}

