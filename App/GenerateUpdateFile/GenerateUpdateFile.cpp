#include "GenerateUpdateFile.h"
#include <QCoreApplication>
#include <QFile>
#include <QCryptographicHash>
#include <QDomComment>
#include <QCoreApplication>
#include <QDebug>
#include <QDateTime>
#include <QFileInfo>
#include "RabbitCommonLog.h"

CGenerateUpdateFile::CGenerateUpdateFile()
{
    SetArch(BUILD_ARCH);
#if defined (Q_OS_WIN)
        m_szPlatform += "x86";
#elif defined(Q_OS_ANDROID)
        m_szPlatform += "arm";        
#elif defined (Q_OS_LINUX)
        m_szPlatform += "x86_64";
#endif
        
    QString szVerion = qApp->applicationVersion();

    if(szVerion.isEmpty())
    {
#ifdef RabbitCommon_VERSION
        szVerion = RabbitCommon_VERSION;
#else
        szVerion = "0.0.1";
#endif
    }

    SetVersion(szVerion);
}

int CGenerateUpdateFile::SetArch(const QString &szArch)
{
    m_szCurrentArch = szArch;
    return 0;
}

int CGenerateUpdateFile::SetVersion(const QString &szVersion)
{
    m_szCurrentVersion = szVersion;
    return 0;
}

int CGenerateUpdateFile::GenerateUpdateXml()
{
    QCommandLineParser parser;
    return GenerateUpdateXml(parser);
}

int CGenerateUpdateFile::GenerateUpdateXml(QCommandLineParser &parser)
{
    int nRet = -1;
    INFO info;
    
    QString szSystem, szUrl;
#if defined (Q_OS_WIN)
    szSystem = "Windows";
    szUrl = "https://github.com/KangLin/"
            + qApp->applicationName()
            + "/releases/download/"
            + m_szCurrentVersion + "/"
            + qApp->applicationName()
            + "-Setup-"
            + m_szCurrentVersion + ".exe";
#elif defined(Q_OS_ANDROID)
    szSystem = "Android";
    szUrl = "https://github.com/KangLin/"
            + qApp->applicationName()
            + "/releases/download/"
            + m_szCurrentVersion + "/"
            + qApp->applicationName().toLower()
            + m_szCurrentVersion + ".apk";
#elif defined(Q_OS_LINUX)
    szSystem = "Linux";
    QFileInfo f(qApp->applicationFilePath());
    if(f.suffix().compare("AppImage", Qt::CaseInsensitive))
    {
        QString szVersion = m_szCurrentVersion;
        szVersion.replace("v", "", Qt::CaseInsensitive);
        szUrl = "https://github.com/KangLin/"
                + qApp->applicationName()
                + "/releases/download/"
                + m_szCurrentVersion + "/"
                + qApp->applicationName().toLower()
                + "_" + szVersion + "_amd64.deb";
    } else {
        szUrl = "https://github.com/KangLin/"
                + qApp->applicationName()
                + "/releases/download/"
                + m_szCurrentVersion + "/"
                + qApp->applicationName()
                + "_" + m_szCurrentVersion + ".tar.gz";
    }
#endif

    parser.addHelpOption();
    parser.addVersionOption();

    QCommandLineOption oFile(QStringList() << "f" << "file",
                             QObject::tr("xml file name"),
                             "xml file name");
    parser.addOption(oFile);
    QCommandLineOption oPackageVersion("pv",
                                QObject::tr("Package version"),
                                "Package version",
                                m_szCurrentVersion);
    parser.addOption(oPackageVersion);
    QCommandLineOption oTime(QStringList() << "t" << "time",
                             QObject::tr("Time"),
                             "Time",
                             QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
    parser.addOption(oTime);
    QCommandLineOption oInfo(QStringList() << "i" << "info",
                             QObject::tr("Information"),
                             "Information",
                             qApp->applicationName() + " " + m_szCurrentVersion);
    parser.addOption(oInfo);

    QCommandLineOption oSystem(QStringList() << "s" << "system",
                             QObject::tr("Operating system"),
                             "Operating system",
                             szSystem);
    parser.addOption(oSystem);
    QCommandLineOption oPlatform(QStringList() << "p" << "platform",
                             QObject::tr("Platform"),
                             "Platform",
                             m_szPlatform);
    parser.addOption(oPlatform);
    QCommandLineOption oArch(QStringList() << "a" << "arch",
                             QObject::tr("Architecture"),
                             "Architecture",
                             m_szCurrentArch);
    parser.addOption(oArch);
    QCommandLineOption oMd5(QStringList() << "c" << "md5",
                             QObject::tr("MD5 checksum"),
                             "MD5 checksum");
    parser.addOption(oMd5);
    QCommandLineOption oPackageFile("pf",
                             QObject::tr("Package file, Is used to calculate md5sum"),
                             "Package file");
    parser.addOption(oPackageFile);
    QCommandLineOption oUrl(QStringList() << "u" << "url",
                             QObject::tr("Package download url"),
                             "Download url",
                             szUrl);
    parser.addOption(oUrl);
    QString szHome = "https://github.com/KangLin/" + qApp->applicationName();
    QCommandLineOption oUrlHome("home",
                             QObject::tr("Project home url"),
                             "Project home url",
                             szHome);
    parser.addOption(oUrlHome);
    QCommandLineOption oMin(QStringList() << "m" << "min",
                             QObject::tr("Min update version"),
                             "Min update version",
                             m_szCurrentVersion);
    parser.addOption(oMin);

    parser.process(*qApp);
    
    QString szFile = parser.value(oFile);
    if(szFile.isEmpty())
        return nRet;
    
    info.szVerion = parser.value(oPackageVersion);
    info.szTime = parser.value(oTime);
    info.szInfomation = parser.value(oInfo);
    info.bForce = false;
    info.szSystem = parser.value(oSystem);
    info.szPlatform = parser.value(oPlatform);
    info.szArchitecture = parser.value(oArch);
    info.szMd5sum = parser.value(oMd5);
    if(info.szMd5sum.isEmpty())
    {
        //计算包的 MD5 和
        QString szFile = parser.value(oPackageFile);
        QCryptographicHash md5sum(QCryptographicHash::Md5);
        QFile app(szFile);
        if(app.open(QIODevice::ReadOnly))
        {
            if(md5sum.addData(&app))
            {
                info.szMd5sum = md5sum.result().toHex();
            }
            app.close();
        } else {
            LOG_MODEL_ERROR("RabbitCommon", "Don't open package file: %s",
                            szFile.toStdString().c_str());
        }
    }
    info.szUrl = parser.value(oUrl);
    info.szUrlHome = parser.value(oUrlHome);
    info.szMinUpdateVersion = parser.value(oMin);
   
    return GenerateUpdateXmlFile(szFile, info);
}

int CGenerateUpdateFile::GenerateUpdateXmlFile(const QString &szFile, const INFO &info)
{    
    QDomDocument doc;
    QDomProcessingInstruction ins;
    //<?xml version='1.0' encoding='UTF-8'?>
    ins = doc.createProcessingInstruction("xml", "version=\'1.0\' encoding=\'UTF-8\'");
    doc.appendChild(ins);
    QDomElement root = doc.createElement("UPDATE");
    doc.appendChild(root);
    
    QDomText version = doc.createTextNode("VERSION");
    version.setData(info.szVerion);
    QDomElement eVersion = doc.createElement("VERSION");
    eVersion.appendChild(version);
    root.appendChild(eVersion);
    
    QDomText time = doc.createTextNode("TIME");
    time.setData(info.szTime);
    QDomElement eTime = doc.createElement("TIME");
    eTime.appendChild(time);
    root.appendChild(eTime);

    QDomText i = doc.createTextNode("INFO");
    i.setData(info.szInfomation);
    QDomElement eInfo = doc.createElement("INFO");
    eInfo.appendChild(i);
    root.appendChild(eInfo);
    
    QDomText force = doc.createTextNode("FORCE");
    force.setData(QString::number(info.bForce));
    QDomElement eForce = doc.createElement("FORCE");
    eForce.appendChild(force);
    root.appendChild(eForce);
    
    QDomText system = doc.createTextNode("SYSTEM");
    system.setData(info.szSystem);
    QDomElement eSystem = doc.createElement("SYSTEM");
    eSystem.appendChild(system);
    root.appendChild(eSystem);
    
    QDomText platform = doc.createTextNode("PLATFORM");
    platform.setData(info.szPlatform);
    QDomElement ePlatform = doc.createElement("PLATFORM");
    ePlatform.appendChild(platform);
    root.appendChild(ePlatform);
    
    QDomText arch = doc.createTextNode("ARCHITECTURE");
    arch.setData(info.szArchitecture);
    QDomElement eArch = doc.createElement("ARCHITECTURE");
    eArch.appendChild(arch);
    root.appendChild(eArch);
    
    QDomText md5 = doc.createTextNode("MD5SUM");
    md5.setData(info.szMd5sum);
    QDomElement eMd5 = doc.createElement("MD5SUM");
    eMd5.appendChild(md5);
    root.appendChild(eMd5);
    
    QDomText url = doc.createTextNode("URL");
    url.setData(info.szUrl);
    QDomElement eUrl = doc.createElement("URL");
    eUrl.appendChild(url);
    root.appendChild(eUrl);
    
    QDomText urlHome = doc.createTextNode("HOME");
    urlHome.setData(info.szUrlHome);
    QDomElement eUrlHome = doc.createElement("HOME");
    eUrlHome.appendChild(urlHome);
    root.appendChild(eUrlHome);
    
    QDomText min = doc.createTextNode("MIN_UPDATE_VERSION");
    min.setData(info.szMinUpdateVersion);
    QDomElement eMin = doc.createElement("MIN_UPDATE_VERSION");
    eMin.appendChild(min);
    root.appendChild(eMin);
    
    QFile f;
    f.setFileName(szFile);
    if(!f.open(QIODevice::WriteOnly))
    {
        LOG_MODEL_ERROR("RabbitCommon",
                       "CFrmUpdater::GenerateUpdateXml file open file fail: %s",
                       f.fileName().toStdString().c_str());
        return -1;
    }
    QTextStream stream(&f);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    stream.setCodec("UTF-8");
#endif
    doc.save(stream, 4);
    f.close();
    return 0;
}
