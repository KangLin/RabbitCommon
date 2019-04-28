#include "RabbitCommonGlobalDir.h"
#include <QStandardPaths>
#include <QDir>
#include <QApplication>
#include <QDebug>

CRabbitCommonGlobalDir::CRabbitCommonGlobalDir()
{
    //注意这个必须的在最前  
    m_szDocumentPath =  QStandardPaths::writableLocation(
                QStandardPaths::DocumentsLocation);
    QDir d;
    if(!d.exists(m_szDocumentPath))
        d.mkpath(m_szDocumentPath);
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
    return qApp->applicationDirPath() + QDir::separator() + "..";
}

QString CRabbitCommonGlobalDir::GetDirConfig()
{
    QString szPath = GetDirApplication() + QDir::separator() + "etc";
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

QString CRabbitCommonGlobalDir::GetDirDocument()
{
    QString szPath;
    if(m_szDocumentPath.right(1) == "/"
            || m_szDocumentPath.right(1) == "\\")
        szPath = m_szDocumentPath.left(m_szDocumentPath.size() - 1);
    else
        szPath = m_szDocumentPath;
    szPath += QDir::separator();
    szPath = szPath + "Rabbit"
             + QDir::separator() + QApplication::applicationName();
    QDir d;
    if(!d.exists(szPath))
        d.mkpath(szPath);
    return szPath;
}

int CRabbitCommonGlobalDir::SetDirDocument(QString szPath)
{
    m_szDocumentPath = szPath + QDir::separator() + "Rabbit"
            + QDir::separator() + QApplication::applicationName();
    return 0;
}

QString CRabbitCommonGlobalDir::GetDirData()
{
    QString szPath = GetDirDocument() + QDir::separator() + "Data";
    QDir d;
    if(!d.exists(szPath))
        d.mkpath(szPath);
    return szPath;
}

QString CRabbitCommonGlobalDir::GetDirImage()
{
    QString szPath = GetDirData() + QDir::separator() + "Image";
    QDir d;
    if(!d.exists(szPath))
        d.mkpath(szPath);
    return szPath;
}

QString CRabbitCommonGlobalDir::GetDirTranslations()
{
#if  defined(Q_OS_ANDROID)
    //TODO:android下应该在安装包中装好语言  
    return GetDirDocument() + QDir::separator() + "translations";
#endif
    return GetDirApplication() + QDir::separator() + "translations";
}

QString CRabbitCommonGlobalDir::GetApplicationConfigureFile()
{
    return GetDirConfig() + QDir::separator() + QApplication::applicationName() + ".conf";
}

QString CRabbitCommonGlobalDir::GetUserConfigureFile()
{
    QString szName = GetDirDocument() + QDir::separator() + QApplication::applicationName() + ".conf";
    return szName;
}

