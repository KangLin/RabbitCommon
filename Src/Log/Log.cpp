// Copyright Copyright (c) Kang Lin studio, All Rights Reserved
// Author Kang Lin <kl222@126.com>

#include "Log.h"
#include "RabbitCommonDir.h"
#include <string>
#include <QDebug>
#include <QSettings>
#include <QDir>
#include <QFileInfo>
#include <QUrl>
#ifdef HAVE_RABBITCOMMON_GUI
    #include <QDesktopServices>
#endif
#include <QRegularExpression>
#include <QDateTime>
#include <QtGlobal>
#include <QMutex>
#include <QCoreApplication>
#ifdef HAVE_RABBITCOMMON_GUI
#include "DockDebugLog.h"
extern CDockDebugLog* g_pDcokDebugLog;
#endif

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    QtMessageHandler g_originalMessageHandler = Q_NULLPTR;
#else
    QtMsgHandler g_originalMessageHandler = Q_NULLPTR;
#endif
    

namespace RabbitCommon {

QLoggingCategory Logger("RabbitCommon");
QRegularExpression g_reInclude;
QRegularExpression g_reExclude;

CLog::CLog() : QObject(),
    m_szFileFormat("yyyy-MM-dd"),
    m_nLength(0),
    m_nCount(0)
{    
    QSettings set(RabbitCommon::CDir::Instance()->GetFileUserConfigure(),
                  QSettings::IniFormat);
    
    SetFilter(set.value("Log/Filter/include").toString(),
              set.value("Log/Filter/Exclude").toString());
    
    QString szPattern = "[%{time hh:mm:ss.zzz} %{pid}|%{threadid} %{if-debug}D%{endif}%{if-info}I%{endif}%{if-warning}W%{endif}%{if-critical}E%{endif}%{if-fatal}F%{endif}] %{category} - %{message}";
#ifdef QT_MESSAGELOGCONTEXT
    // Use qt message log context(__FILE__, __LIEN__, __FUNCTION__) in release
    szPattern += " [%{file}:%{line}, %{function}]";
#elif defined(DEBUG) || defined(_DEBUG)
    szPattern += " [%{file}:%{line}, %{function}]";
#endif
    QString szFilterRules;
    quint64 nInterval = 60; // Unit: second
#if !(defined(DEBUG) || defined(_DEBUG) || ANDROID)
    szFilterRules = "*.debug = false";
#endif

    m_szPath = CDir::Instance()->GetDirLog();
    QString szConfFile = RabbitCommon::CDir::Instance()->GetDirConfig()
            + QDir::separator() + qApp->applicationName() + "_logqt.ini";
    szConfFile = set.value("Log/ConfigFile", szConfFile).toString();
    if (QFile::exists(szConfFile))
    {
        m_szConfigureFile = szConfFile;
        qInfo(Logger) << "Load log configure file:" << m_szConfigureFile;
        QSettings set(m_szConfigureFile, QSettings::IniFormat);
        set.beginGroup("Log");
        m_szPath = set.value("Path", m_szPath).toString();
        m_szFileFormat = set.value("Name", m_szFileFormat).toString();
        szPattern = set.value("Pattern", szPattern).toString();
        nInterval = set.value("Interval", nInterval).toUInt();
        m_nCount = set.value("Count", 0).toULongLong();
        QString szLength = set.value("Length", 0).toString();
        if(!szLength.isEmpty()) {
            QRegularExpression e("(\\d+)([mkg]?)",
                                 QRegularExpression::CaseInsensitiveOption);
            QRegularExpressionMatch m;
            if(szLength.contains(e, &m) && m.hasMatch())
            {
                quint64 len = 0;
                if(m.capturedLength(1) > 0)
                    len = m.captured(1).toULong();
                if(m.capturedLength(2) > 0)
                {
                    QString u = m.captured(2).toUpper();
                    if("K" == u)
                        len <<= 10;
                    else if("M" == u)
                        len <<= 20;
                    else if("G" == u)
                        len <<= 30;
                }
                m_nLength = len;
            }
        }
        set.endGroup();

        set.beginGroup("Rules");
        auto keys = set.childKeys();
        //qDebug(Logger) << "keys" << keys;
        szFilterRules.clear();
        foreach(auto k, keys) {
            QString v = set.value(k).toString();
            szFilterRules += k + "=" + v + "\n";
        }
        set.endGroup();
    } else {
        qWarning(Logger) << "Log configure file is not exist:" << szConfFile
                         << ". Use default settings.";
    }

    qDebug(Logger) << "Log configure:"
                   << "\n    Path:" << m_szPath
                   << "\n    FileNameFormat:" << m_szFileFormat
                   << "\n    szPattern:" << szPattern
                   << "\n    Interval:" << nInterval
                   << "\n    Count:" << m_nCount
                   << "\n    Length:" << m_nLength
                   << "\n    Rules:" << szFilterRules;

    if(!szFilterRules.isEmpty())
        QLoggingCategory::setFilterRules(szFilterRules);

    #if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
        qSetMessagePattern(szPattern);
        g_originalMessageHandler = qInstallMessageHandler(myMessageOutput);
    #else
        g_originalMessageHandler = qInstallMsgHandler(myMessageOutput);
    #endif

    QDir d;
    if(!d.exists(m_szPath))
    {
        if(!d.mkpath(m_szPath))
            fprintf(stderr, "Create log directory fail. %s\n",
                    m_szPath.toStdString().c_str());
    }

    slotTimeout();

    bool check = connect(&m_Timer, SIGNAL(timeout()),
                         this, SLOT(slotTimeout()));
    Q_ASSERT(check);
    m_Timer.start(nInterval * 1000);
}

CLog::~CLog()
{
    if(m_File.isOpen())
        m_File.close();
    if(m_Timer.isActive())
        m_Timer.stop();
}

CLog* CLog::Instance()
{
    static CLog* p = NULL;
    if(!p)
    {
        p = new CLog();
    }
    return p;
}

QString CLog::GetLogFile()
{
    return m_File.fileName();
}

QString CLog::OpenLogConfigureFile()
{
    return m_szConfigureFile;
}

QString CLog::GetLogDir()
{
    QString f = GetLogFile();
    if(f.isEmpty()) return f;
    
    QFileInfo fi(f);
    return fi.absolutePath();
}

int CLog::SetFilter(const QString &szInclude, const QString &szExclude)
{
    g_reInclude = QRegularExpression(szInclude);
    g_reExclude = QRegularExpression(szExclude);
    
    QSettings set(RabbitCommon::CDir::Instance()->GetFileUserConfigure(),
                  QSettings::IniFormat);
    set.setValue("Log/Filter/include", szInclude);
    set.setValue("Log/Filter/Exclude", szExclude);
    return 0;
}

int CLog::GetFilter(QString &szInclude, QString &szExclude)
{
    szInclude = g_reInclude.pattern();
    szExclude = g_reExclude.pattern();
    return 0;
}

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
void CLog::myMessageOutput(QtMsgType type,
                           const QMessageLogContext &context,
                           const QString &msg)
{
    QString szMsg;
    #if QT_VERSION >= QT_VERSION_CHECK(5, 4, 0)
        szMsg = qFormatLogMessage(type, context, msg);
    #else
        szMsg = msg;
    #endif
        
    if(g_reInclude.isValid() && !g_reInclude.pattern().isEmpty()) {
        QRegularExpressionMatch m = g_reInclude.match(szMsg);
        if(!m.hasMatch()) {
            return;
        }
    }
    if(g_reExclude.isValid() && !g_reExclude.pattern().isEmpty()) {
        QRegularExpressionMatch m = g_reExclude.match(szMsg);
        if(m.hasMatch()) {
            return;
        }
    }

#ifdef HAVE_RABBITCOMMON_GUI
        if(g_pDcokDebugLog)
            emit g_pDcokDebugLog->sigAddLog(szMsg);
#endif

    /*
    QFile f(CLog::Instance()->GetLogFile());
    if(!f.open(QFile::WriteOnly | QFile::Append))
    {
        fprintf(stderr, "Open log file fail. %s\n",
                f.fileName().toStdString().c_str());
        return;
    }//*/

    if(CLog::Instance()->m_File.isOpen())
    {
        QTextStream s(&CLog::Instance()->m_File);
        CLog::Instance()->m_Mutex.lock();
        s << szMsg << "\r\n";
        //s.flush();
        CLog::Instance()->m_Mutex.unlock();
    }

    //f.close();

    if(g_originalMessageHandler)
        g_originalMessageHandler(type, context, msg);
}
#else
void CLog::myMessageOutput(QtMsgType type, const char* msg)
{
    QString szMsg(msg);
    
    if(g_reInclude.isValid() && !g_reInclude.pattern().isEmpty()) {
        QRegularExpressionMatch m = g_reInclude.match(szMsg);
        if(!m.hasMatch()) {
            return;
        }
    }
    if(g_reExclude.isValid() && !g_reExclude.pattern().isEmpty()) {
        QRegularExpressionMatch m = g_reExclude.match(szMsg);
        if(m.hasMatch()) {
            return;
        }
    }
    
    /*
    QFile f(CLog::Instance()->GetLogFile());
    if(!f.open(QFile::WriteOnly | QFile::Append))
    {
        fprintf(stderr, "Open log file fail. %s\n",
                f.fileName().toStdString().c_str());
        return;
    } //*/
    
    if(CLog::Instance()->m_File.isOpen())
    {
        QTextStream s(&CLog::Instance()->m_File);
        CLog::Instance()->m_Mutex.lock();
        s << szMsg << "\r\n";
        CLog::Instance()->m_Mutex.unlock();
    }

    //f.close();

    if(g_originalMessageHandler)
        g_originalMessageHandler(type, msg);
}
#endif

void CLog::checkFileCount()
{
    if(m_nCount < 2) return;
    
    QDir d(m_szPath);
    auto lstFiles = d.entryInfoList(QDir::Files, QDir::Time);
    if(lstFiles.size() < m_nCount) return;

    if(lstFiles.first().lastModified() < lstFiles.back().lastModified())
    {
        if(d.remove(lstFiles.first().absoluteFilePath()))
            qInfo(Logger) << "Remove file:" << lstFiles.first().absoluteFilePath();
        else
            qCritical(Logger) << "Remove file fail:"
                              << lstFiles.first().absoluteFilePath();
    } else {
        if(d.remove(lstFiles.back().absoluteFilePath()))
            qInfo(Logger) << "Remove file:" << lstFiles.back().absoluteFilePath();
        else
            qCritical(Logger) << "Remove file fail:"
                              << lstFiles.back().absoluteFilePath();
    }
}

QString CLog::getFileName()
{
    QChar fill('0');
    QString szNo = QString("%1").arg(1, 4, 10, fill);
    QString szSep("_");
    QString szName;
    QString szFile;
    QDir d(m_szPath);

    szFile = m_File.fileName();
    if(!szFile.isEmpty()) return szFile;

    szName = QDate::currentDate().toString(m_szFileFormat);
    d.setNameFilters(QStringList() << szName + "*");
    auto lstFiles = d.entryInfoList(QDir::Files, QDir::Name);
    //qDebug(Logger) << "Log files:" << lstFiles;
    if(lstFiles.isEmpty())
        szFile = m_szPath + QDir::separator() + szName + szSep + szNo + ".log";
    else
        szFile = lstFiles.back().absoluteFilePath();

    return szFile;
}

QString CLog::getNextFileName(const QString szFile)
{
    if(szFile.isEmpty())
        return QString();

    QFileInfo fi(szFile);
    QChar fill('0');
    QString szNo = QString("%1").arg(1, 4, 10, fill);
    QString szSep("_");
    QString szName = fi.baseName();

    auto s = szName.split(szSep);
    if(s.size() > 0
            && QDate::currentDate().toString(m_szFileFormat) == s[0])
    {
        if(s.size() == 2) {
            szNo = s[1];
            szNo = QString("%1").arg(szNo.toInt() + 1, 4, 10, fill);
        }

        szName = s[0];
    } else {
        szName = QDate::currentDate().toString(m_szFileFormat);
    }

    szName += szSep + szNo;
    return m_szPath + QDir::separator() + szName + ".log";
}

bool CLog::checkFileLength()
{
    if(m_nLength == 0) return false;

    if(m_File.fileName().isEmpty()) return false;

    QFileInfo fi(m_File.fileName());
    if(fi.exists()) {
        if(fi.size() < m_nLength) return false;
    } else {
        return false;
    }

    return true;
}

void CLog::slotTimeout()
{
    QString szFile;
    if(m_File.fileName().isEmpty())
    {
        szFile = getFileName();
        m_File.setFileName(szFile);
    }

    do {
        if(checkFileLength())
            szFile = getNextFileName(m_File.fileName());
        else
            if(m_File.isOpen()) break;
#ifdef Q_OS_ANDROID
        if(szFile.isEmpty()) return;
#endif
        Q_ASSERT(!szFile.isEmpty());

        m_Mutex.lock();
        if(m_File.isOpen())
            m_File.close();

        m_File.setFileName(szFile);
        if(!m_File.open(QFile::WriteOnly | QFile::Append))
        {
            //NOTE: Do not use qDebug or qCritical. Causes a deadlock.
            fprintf(stderr, "Open log file fail. %s\n",
                    m_File.fileName().toStdString().c_str());
        }
        m_Mutex.unlock();
    } while(0);
    
    checkFileCount();
}

#ifdef HAVE_RABBITCOMMON_GUI

void OpenLogConfigureFile()
{
    QString f = RabbitCommon::CLog::Instance()->OpenLogConfigureFile();
    if(f.isEmpty())
    {
        qCritical(Logger) << "Configure file is empty";
        return;
    }
    QDesktopServices::openUrl(QUrl::fromLocalFile(f));
}

void OpenLogFile()
{
    QString d = RabbitCommon::CLog::Instance()->GetLogFile();
    if(d.isEmpty())
    {
        qCritical(Logger) << "Log file is empty";
        return;
    }
    QDesktopServices::openUrl(d);
}

void OpenLogFolder()
{
    QString f = RabbitCommon::CLog::Instance()->GetLogDir();
    if(f.isEmpty())
    {
        qCritical(Logger) << "Log folder is empty";
        return;
    }
    QDesktopServices::openUrl(QUrl::fromLocalFile(f));
}

#endif

} // namespace RabbitCommon
