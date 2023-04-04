// Copyright Copyright (c) Kang Lin studio, All Rights Reserved
// Author Kang Lin <kl222@126.com>

#include "RabbitCommonLog.h"
#include "RabbitCommonDir.h"
#include <string>
#include <stdarg.h>
#include <QDebug>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <QThread>
#include <QSettings>
#include <QDir>
#include <QFileInfo>
#include <QDesktopServices>
#include <QUrl>
#include <QCoreApplication>
#include <QRegularExpression>
#include <QDateTime>
#include <QtGlobal>
#include <QMutex>
#include <QStandardPaths>

// https://github.com/MEONMedical/Log4Qt
#ifdef HAVE_LOG4QT
    #include "log4qt/logger.h"
    #include "log4qt/propertyconfigurator.h"
    #include "log4qt/loggerrepository.h"
    #include "log4qt/consoleappender.h"
    #include "log4qt/ttcclayout.h"
    #include "log4qt/logmanager.h"
    #include "log4qt/fileappender.h"
    #include "log4qt/patternlayout.h"
    #include "log4qt/basicconfigurator.h"
#endif
#ifdef HAVE_LOG4CXX
    #include "log4cxx/logger.h"
    #include "log4cxx/basicconfigurator.h"
    #include "log4cxx/propertyconfigurator.h"
    #include "log4cxx/consoleappender.h"
    #include "log4cxx/fileappender.h"
    #include "log4cxx/patternlayout.h"
    #include "log4cxx/logmanager.h"
    //#include <log4cxx-qt/messagehandler.h>
#endif
//https://github.com/orocos-toolchain/log4cpp
//https://logging.apache.org/log4cxx
// https://github.com/log4cplus/log4cplus
#ifdef HAVE_LOG4CPLUS
    #include "log4cplus/log4cplus.h"
#endif

namespace RabbitCommon {

QLoggingCategory Logger("RabbitCommon");

CLog::CLog() : QObject(),
    m_szFileFormat("yyyy-MM-dd"),
    m_nLength(0),
    m_nCount(0)
{
    m_bEnablePrintThread = true;
    
    QSettings set(RabbitCommon::CDir::Instance()->GetFileUserConfigure(),
                  QSettings::IniFormat);
#ifdef HAVE_LOG4QT

    // Enable handling of Qt messages
    Log4Qt::LogManager::setHandleQtMessages(true);
    QString szConfFile = RabbitCommon::CDir::Instance()->GetDirConfig(true)
            + QDir::separator() + qApp->applicationName() + "_log4qt.conf";
    szConfFile = set.value("Log/ConfigFile", szConfFile).toString();
    if (QFile::exists(szConfFile))
    {
        m_szConfigureFile = szConfFile;
        qInfo(Logger) << "Load log configure file:" << m_szConfigureFile;
        if(!Log4Qt::PropertyConfigurator::configureAndWatch(szConfFile))
            Log4Qt::BasicConfigurator::configure();
        else
            qInfo(Logger) << "Log configure file:" << szConfFile;
    }
    else
    {
        qWarning(Logger) << "Log configure file is not exist:" << szConfFile
                         << ". Use default.";
        // Create a layout
        auto logger = Log4Qt::Logger::rootLogger();
        auto *layout = new Log4Qt::PatternLayout("%d %F(%L) [%t] %p %c: %m%n");
        layout->setName(QStringLiteral("My Layout"));
        layout->activateOptions();
        // Create a console appender
        Log4Qt::ConsoleAppender *consoleAppender =
                new Log4Qt::ConsoleAppender(layout,
                                        Log4Qt::ConsoleAppender::STDOUT_TARGET);
        consoleAppender->setName(QStringLiteral("RabbitCommon Appender"));
        consoleAppender->activateOptions();
        // Add appender on root logger
        logger->addAppender(consoleAppender);
        //logger->setLevel(Log4Qt::Level::DEBUG_INT);
    }

#elif defined(HAVE_LOG4CXX)

    //qInstallMessageHandler( log4cxx::qt::messageHandler );
    QString szConfFile = RabbitCommon::CDir::Instance()->GetDirConfig(true)
            + QDir::separator() + qApp->applicationName() + "_log4cxx.conf";
    szConfFile = set.value("Log/ConfigFile", szConfFile).toString();
    if (QFile::exists(szConfFile))
    {
        m_szConfigureFile = szConfFile;
        qInfo(Logger) << "Load log configure file:" << m_szConfigureFile;
        log4cxx::PropertyConfigurator::configure(szConfFile.toStdString().c_str());
    } else {
        //log4cxx::BasicConfigurator::configure();
        log4cxx::LogManager::getLoggerRepository()->setConfigured(true);
        log4cxx::LoggerPtr root = log4cxx::Logger::getRootLogger();
        static const log4cxx::LogString TTCC_CONVERSION_PATTERN(LOG4CXX_STR("%r %F(%L) [%t] %p %c - %m%n"));
        log4cxx::LayoutPtr layout(new log4cxx::PatternLayout(TTCC_CONVERSION_PATTERN));
        log4cxx::AppenderPtr appender(new log4cxx::ConsoleAppender(layout));
        root->addAppender(appender);
        qWarning(Logger) << "Log configure file is not exist:" << szConfFile
                         << ". Use default.";
    }

#elif defined(HAVE_LOG4CPLUS)

    log4cplus::initialize();
    QString szConfFile = RabbitCommon::CDir::Instance()->GetDirConfig(true)
            + QDir::separator() + qApp->applicationName() + "_log4cplus.conf";
    szConfFile = set.value("Log/ConfigFile", szConfFile).toString();
    if(QFile::exists(szConfFile))
    {
        m_szConfigureFile = szConfFile;
        qInfo(Logger) << "Load log configure file:" << m_szConfigureFile;
        log4cplus::PropertyConfigurator::doConfigure(LOG4CPLUS_STRING_TO_TSTRING(szConfFile.toStdString()));
    } else {
        log4cplus::SharedAppenderPtr appender(new log4cplus::ConsoleAppender(true));
        appender->setName(LOG4CPLUS_TEXT("console"));
        appender->setLayout(std::unique_ptr<log4cplus::Layout>(
                                new log4cplus::PatternLayout(LOG4CPLUS_TEXT("%F(%L) [%t] %p %c: %m%n"))));
        log4cplus::Logger::getRoot().addAppender(appender);
        qWarning(Logger) << "Log configure file is not exist:" << szConfFile
                         << ". Use default.";
    }

#else

    QString szPattern = "%{time hh:mm:ss.zzz} [%{threadid}] %{type} [%{category}] - %{message} [%{file}:%{line}, %{function}]";
    QString szFilterRules;
    quint64 nInterval = 60; // Unit: second
#if !(defined(DEBUG) || defined(_DEBUG))
    szFilterRules = "*.debug = false";
#endif
    m_szPath = CDir::Instance()->GetDirApplicationInstallRoot()
            + QDir::separator() + "log";
    QString szConfFile = RabbitCommon::CDir::Instance()->GetDirConfig(true)
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
    qInstallMessageHandler(myMessageOutput);
#else
    qInstallMsgHandler(myMessageOutput);
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
#endif
}

CLog::~CLog()
{
#ifdef HAVE_LOG4QT
    auto logger = Log4Qt::Logger::rootLogger();
    logger->removeAllAppenders();
    logger->loggerRepository()->shutdown();
#elif defined(HAVE_LOG4CXX)
    log4cxx::LoggerPtr root = log4cxx::Logger::getRootLogger();
    logger->removeAllAppenders();
    logger->loggerRepository()->shutdown();
#else
    if(m_File.isOpen())
        m_File.close();
    if(m_Timer.isActive())
        m_Timer.stop();
#endif
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

int CLog::EnablePrintThread(bool bPrint)
{
    m_bEnablePrintThread = bPrint;
    return 0;
}

#define LOG_BUFFER_LENGTH 1024

#ifdef HAVE_LOG4QT

int CLog::Print(const char *pszFile, int nLine, const char* pszFunction, int nLevel,
                 const char* pszModelName, const char *pFormatString, ...)
{
    char buf[LOG_BUFFER_LENGTH];
    va_list args;
    va_start (args, pFormatString);
    int nRet = vsnprintf(buf, LOG_BUFFER_LENGTH, pFormatString, args);
    va_end (args);
    if(nRet < 0 || nRet >= LOG_BUFFER_LENGTH)
    {
        qCritical(Logger) << "vsprintf buf is short,"
                          << nRet << ">" << LOG_BUFFER_LENGTH
                          << ". Truncated it:" << nRet - LOG_BUFFER_LENGTH;
        buf[LOG_BUFFER_LENGTH - 1] = 0;
        //return nRet;
    }
    
    Log4Qt::Level l;
    switch(nLevel)
    {
    case LM_DEBUG:
        l = Log4Qt::Level::DEBUG_INT;
        break;
    case LM_ERROR:
        l = Log4Qt::Level::ERROR_INT;
        break;
    case LM_INFO:
        l = Log4Qt::Level::INFO_INT;
        break;
    case LM_WARNING:
        l = Log4Qt::Level::WARN_INT;
        break;
    }
    
    Log4Qt::Logger::logger(pszModelName)->logWithLocation(l, pszFile, nLine, pszFunction, buf);
    return 0;
}

QString CLog::GetLogFile()
{
    auto logger = Log4Qt::Logger::rootLogger();
    auto appenders = logger->appenders();
    for (auto &&a : qAsConst(appenders))
    {
        auto f = qobject_cast<Log4Qt::FileAppender*>(a);
        if(f) return f->file();
    }
    return QString();
}

#elif HAVE_LOG4CXX

int CLog::Print(const char *pszFile, int nLine, const char* pszFunction, int nLevel,
                 const char* pszModelName, const char *pFormatString, ...)
{
    char buf[LOG_BUFFER_LENGTH];
    va_list args;
    va_start (args, pFormatString);
    int nRet = vsnprintf(buf, LOG_BUFFER_LENGTH, pFormatString, args);
    va_end (args);
    if(nRet < 0 || nRet >= LOG_BUFFER_LENGTH)
    {
        qCritical(Logger) << "vsprintf buf is short,"
                          << nRet << ">" << LOG_BUFFER_LENGTH
                          << ". Truncated it:" << nRet - LOG_BUFFER_LENGTH;
        buf[LOG_BUFFER_LENGTH - 1] = 0;
        //return nRet;
    }
    log4cxx::LevelPtr l = log4cxx::Level::getDebug();
    switch(nLevel)
    {
    case LM_DEBUG:
        l = log4cxx::Level::getDebug();
        break;
    case LM_ERROR:
        l = log4cxx::Level::getError();
        break;
    case LM_INFO:
        l = log4cxx::Level::getInfo();
        break;
    case LM_WARNING:
        l = log4cxx::Level::getWarn();
        break;
    }
    log4cxx::LoggerPtr log = log4cxx::Logger::getLogger(pszModelName);
    log4cxx::spi::LocationInfo loc(pszFile, pszFunction, nLine);
    log->log(l, buf, loc);
    return 0;
}

QString CLog::GetLogFile()
{
    auto logger = log4cxx::Logger::getRootLogger();
    auto appenders = logger->getAllAppenders();
    for (auto &&a : appenders)
    {
        auto f = dynamic_cast<log4cxx::FileAppender*>(a.get());
        if(f) return QString::fromStdWString(f->getFile());
    }

    return QString();
}

#elif HAVE_LOG4CPLUS

int CLog::Print(const char *pszFile, int nLine, const char* pszFunction, int nLevel,
                 const char* pszModelName, const char *pFormatString, ...)
{
    char buf[LOG_BUFFER_LENGTH];
    va_list args;
    va_start (args, pFormatString);
    int nRet = vsnprintf(buf, LOG_BUFFER_LENGTH, pFormatString, args);
    va_end (args);
    if(nRet < 0 || nRet >= LOG_BUFFER_LENGTH)
    {
        qCritical(Logger) << "vsprintf buf is short,"
                          << nRet << ">" << LOG_BUFFER_LENGTH
                          << ". Truncated it:" << nRet - LOG_BUFFER_LENGTH;
        buf[LOG_BUFFER_LENGTH - 1] = 0;
        //return nRet;
    }
    log4cplus::LogLevel l = log4cplus::DEBUG_LOG_LEVEL;
    switch(nLevel)
    {
    case LM_DEBUG:
        l = log4cplus::DEBUG_LOG_LEVEL;
        break;
    case LM_ERROR:
        l = log4cplus::ERROR_LOG_LEVEL;
        break;
    case LM_INFO:
        l = log4cplus::INFO_LOG_LEVEL;
        break;
    case LM_WARNING:
        l = log4cplus::WARN_LOG_LEVEL;
        break;
    }
    log4cplus::Logger log = log4cplus::Logger::getInstance(LOG4CPLUS_C_STR_TO_TSTRING(pszModelName));
    log.log(l, LOG4CPLUS_C_STR_TO_TSTRING(buf), pszFile, nLine, pszFunction);
    return 0;
}

QString CLog::GetLogFile()
{
    return QString();
}

#else

int CLog::Print(const char *pszFile, int nLine, const char* pszFunction, int nLevel,
                 const char* pszModelName, const char *pFormatString, ...)
{
    Q_UNUSED(pszFunction);
    char buf[LOG_BUFFER_LENGTH];
    std::string szTemp = pszFile;
    szTemp += "(";
    szTemp += std::to_string(nLine);
    szTemp += "):";
#ifndef MINGW
    if(m_bEnablePrintThread)
    {
        szTemp += "[";
#if defined (Q_OS_WINDOWS)
        sprintf_s(buf, "0x%08lX", reinterpret_cast<unsigned long>(QThread::currentThreadId()));
#else
        sprintf(buf, "0x%08lX", (unsigned long)(QThread::currentThreadId()));
#endif
        szTemp += buf;
        szTemp += "]:";
    }
#endif
    switch(nLevel)
    {
    case LM_DEBUG:
        szTemp += "DEBUG";
        break;
    case LM_ERROR:
        szTemp += "ERROR";
        break;
    case LM_INFO:
        szTemp += "INFO";
        break;
    case LM_WARNING:
        szTemp = "WARNING";
        break;
    }
    szTemp += ":";
    szTemp += pszModelName;
    szTemp += ": ";

    va_list args;
    va_start (args, pFormatString);
    int nRet = vsnprintf(buf, LOG_BUFFER_LENGTH, pFormatString, args);
    va_end (args);
    if(nRet < 0 || nRet >= LOG_BUFFER_LENGTH)
    {
        qCritical(Logger) << "vsprintf buf is short,"
                          << nRet << ">" << LOG_BUFFER_LENGTH
                          << ". Truncated it:" << nRet - LOG_BUFFER_LENGTH;
        buf[LOG_BUFFER_LENGTH - 1] = 0;
        //return nRet;
    }
    szTemp += buf;
    
    //std::cout << szTemp;
    
    qDebug() << szTemp.c_str();
    
    return 0;
}

QString CLog::GetLogFile()
{
    return m_File.fileName();
}

#endif

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
    fprintf(stderr, "%s\r\n", szMsg.toStdString().c_str());

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
}
#else
void CLog::myMessageOutput(QtMsgType type, const char* msg)
{
    QString szMsg(msg);
    fprintf(stderr, "%s\r\n", szMsg.toStdString().c_str());

    QFile f(CLog::Instance()->GetLogFile());
    if(!f.open(QFile::WriteOnly | QFile::Append))
    {
        fprintf(stderr, "Open log file fail. %s\n",
                f.fileName().toStdString().c_str());
        return;
    }

    QTextStream s(&f);
    CLog::Instance()->m_Mutex.lock();
    s << szMsg << "\r\n";
    CLog::Instance()->m_Mutex.unlock();

    f.close();
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

int CLog::checkFileLength()
{
    if(m_nLength == 0) return 0;

    if(m_File.fileName().isEmpty()) return 0;

    QFileInfo fi(m_File.fileName());
    if(fi.exists()) {
        if(fi.size() < m_nLength) return 0;
    } else {
        return 0;
    }

    return 1;
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

#ifdef HAVE_GUI

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
