//! @copyright Copyright (c) Kang Lin studio, All Rights Reserved
//! @author Kang Lin(kl222@126.com)

#include "RabbitCommonLog.h"
#include <string>
#include <stdarg.h>
#include <QDebug>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <QThread>

namespace RabbitCommon {

CLog::CLog()
{
    m_bEnablePrintThread = true;
}

CLog* CLog::Instance()
{
    static CLog* p = NULL;
    if(!p)
        p = new CLog;
    return p;
}

int CLog::EnablePrintThread(bool bPrint)
{
    m_bEnablePrintThread = bPrint;
    return 0;
}

#define LOG_BUFFER_LENGTH 1024
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
        sprintf_s(buf, "0x%08X", reinterpret_cast<unsigned long>(QThread::currentThreadId()));
#else
        sprintf(buf, "0x%08X", (unsigned long)(QThread::currentThreadId()));
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

} // namespace RabbitCommon 
