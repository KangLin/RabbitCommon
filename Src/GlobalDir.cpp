#include "GlobalDir.h"
#include <QStandardPaths>
#include <QDir>
#include <QApplication>

CGlobalDir::CGlobalDir()
{
    //注意这个必须的在最前  
    m_szDocumentPath =  QStandardPaths::writableLocation(
                QStandardPaths::DocumentsLocation);
    QDir d;
    if(!d.exists(m_szDocumentPath))
        d.mkpath(m_szDocumentPath);
}

CGlobalDir* CGlobalDir::Instance()
{
    static CGlobalDir* p = nullptr;
    if(!p)
        p = new CGlobalDir;
    return p;
}

QString CGlobalDir::GetDirApplication()
{
    //LOG_MODEL_DEBUG("CGlobalDir", "GetDirApplication:%s", qApp->applicationDirPath().toStdString().c_str());
    return qApp->applicationDirPath() + QDir::separator() + "..";
}

QString CGlobalDir::GetDirConfig()
{
    QString szPath = GetDirApplication() + QDir::separator() + "etc";
    QDir d;
    if(!d.exists(szPath))
        d.mkpath(szPath);
    return szPath;
}

QString CGlobalDir::GetDirApplicationXml()
{
    QString szPath = GetDirConfig() + QDir::separator() + "xml";
    QDir d;
    if(!d.exists(szPath))
        d.mkpath(szPath);
    return szPath;
}

QString CGlobalDir::GetDirDocument()
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

int CGlobalDir::SetDirDocument(QString szPath)
{
    m_szDocumentPath = szPath + QDir::separator() + "Rabbit"
            + QDir::separator() + QApplication::applicationName();
    return 0;
}

QString CGlobalDir::GetDirData()
{
    QString szPath = GetDirDocument() + QDir::separator() + "Data";
    QDir d;
    if(!d.exists(szPath))
        d.mkpath(szPath);
    return szPath;
}

QString CGlobalDir::GetDirImage()
{
    QString szPath = GetDirData() + QDir::separator() + "Image";
    QDir d;
    if(!d.exists(szPath))
        d.mkpath(szPath);
    return szPath;
}

QString CGlobalDir::GetDirTranslations()
{
#if  defined(Q_OS_ANDROID)
    //TODO:android下应该在安装包中装好语言  
    return GetDirDocument() + QDir::separator() + "translations";
#endif
    return GetDirApplication() + QDir::separator() + "translations";
}

QString CGlobalDir::GetApplicationConfigureFile()
{
    return GetDirConfig() + QDir::separator() + QApplication::applicationName() + ".conf";
}

QString CGlobalDir::GetUserConfigureFile()
{
    QString szName = GetDirDocument() + QDir::separator() + QApplication::applicationName() + ".conf";
    return szName;
}
