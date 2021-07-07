//! @copyright Copyright (c) Kang Lin studio, All Rights Reserved
//! @author Kang Lin(kl222@126.com)

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
//TODO: https://github.com/MEONMedical/Log4Qt
#ifdef HAVE_LOG4QT
    #include "log4qt/logger.h"
    #include "log4qt/propertyconfigurator.h"
    #include "log4qt/loggerrepository.h"
    #include "log4qt/consoleappender.h"
    #include "log4qt/ttcclayout.h"
    #include "log4qt/logmanager.h"
    #include "log4qt/fileappender.h"
    #include "log4qt/patternlayout.h"
#endif
//https://github.com/orocos-toolchain/log4cpp
//https://logging.apache.org/log4cxx
// https://github.com/log4cplus/log4cplus
#ifdef HAVE_LOG4CPLUS
    #include "log4cplus/log4cplus.h"
#endif

namespace RabbitCommon {

CLog::CLog()
{
    m_bEnablePrintThread = true;
    
    QSettings set(RabbitCommon::CDir::Instance()->GetFileUserConfigure(),
                  QSettings::IniFormat);
#ifdef HAVE_LOG4QT
    
    QString configFile = RabbitCommon::CDir::Instance()->GetDirConfig(true)
            + QDir::separator() + "log4qt.properties";
    configFile = set.value("Log/ConfigFile", configFile).toString();
    if (QFile::exists(configFile))
        Log4Qt::PropertyConfigurator::configureAndWatch(configFile);
    else
    {
        // Create a layout
        auto logger = Log4Qt::Logger::rootLogger();
        auto *layout = new Log4Qt::PatternLayout("%F(%L) [%t] %p: %m%n");
        layout->setName(QStringLiteral("My Layout"));
        layout->activateOptions();
        // Create a console appender
        Log4Qt::ConsoleAppender *consoleAppender = new Log4Qt::ConsoleAppender(layout, Log4Qt::ConsoleAppender::STDOUT_TARGET);
        consoleAppender->setName(QStringLiteral("My Appender"));
        consoleAppender->activateOptions();
        // Add appender on root logger
        logger->addAppender(consoleAppender);
    }

    // Enable handling of Qt messages
    Log4Qt::LogManager::setHandleQtMessages(true);
    
#elif defined(HAVE_LOG4CPLUS)
log4cplus::initialize();
QString szLogConfig = RabbitCommon::CDir::Instance()->GetDirConfig(true)
        + QDir::separator() + "log4config.conf";
szLogConfig = set.value("Log/ConfigFile", szLogConfig).toString();
log4cplus::PropertyConfigurator::doConfigure(LOG4CPLUS_TEXT(szLogConfig.toStdString().c_str()));        
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

int CLog::Print(const char *pszFile, int nLine, int nLevel,
                 const char* pszModelName, const char *pFormatString, ...)
{
    Log4Qt::Logger* pLog = Log4Qt::Logger::logger(pszModelName);
    char buf[LOG_BUFFER_LENGTH];
    va_list args;
    va_start (args, pFormatString);
    int nRet = vsnprintf(buf, LOG_BUFFER_LENGTH, pFormatString, args);
    va_end (args);
    if(nRet < 0 || nRet >= LOG_BUFFER_LENGTH)
    {
        LOG_MODEL_ERROR("Global",
                        "vsprintf buf is short, %d > %d. Truncated it:%d",
                        nRet, LOG_BUFFER_LENGTH, nRet - LOG_BUFFER_LENGTH);
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
    
    pLog->logWithLocation(l, pszFile, nLine, NULL, buf);
    return 0;
}

#else

int CLog::Print(const char *pszFile, int nLine, int nLevel,
                 const char* pszModelName, const char *pFormatString, ...)
{
    char buf[LOG_BUFFER_LENGTH];
    std::string szTemp = pszFile;
    szTemp += "(";
    szTemp += std::to_string(nLine);
    szTemp += "):";
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
        LOG_MODEL_ERROR("Global",
                        "vsprintf buf is short, %d > %d. Truncated it:%d",
                        nRet, LOG_BUFFER_LENGTH, nRet - LOG_BUFFER_LENGTH);
        buf[LOG_BUFFER_LENGTH - 1] = 0;
        //return nRet;
    }
    szTemp += buf;
    
    //std::cout << szTemp;
    
    qDebug() << szTemp.c_str();
    
    return 0;
}

#endif

} // namespace RabbitCommon 
