/** @copyright Copyright (c) Kang Lin studio, All Rights Reserved
 *  @author Kang Lin(kl222@126.com)
 *  @abstract log
 */

#ifndef RABBIT_COMMON_LOG_H
#define RABBIT_COMMON_LOG_H

#pragma once

#include <QObject>
#include "rabbitcommon_export.h"

//TODO: https://github.com/MEONMedical/Log4Qt
//https://github.com/orocos-toolchain/log4cpp
//https://logging.apache.org/log4cxx
// https://github.com/log4cplus/log4cplus
#ifdef HAVE_LOG4CPLUS
#include "log4cplus/log4cplus.h"
#endif

namespace RabbitCommon {

class RABBITCOMMON_EXPORT CLog
{
public:
    CLog();

    static CLog* Instance();
    int EnablePrintThread(bool bPrint);
    
    /**
     * @brief 日志输出
     * @param pszFile:打印日志处文件名
     * @param nLine:打印日志处行号
     * @param nLevel:打印日志错误级别
     * @param pszModelName:打印日志的模块范围
     * @param pFormatString:格式化字符串
     * @return 
     */
    int Print(const char *pszFile, int nLine, int nLevel,
            const char* pszModelName, const char *pFormatString, ...);
    
private:
    bool m_bEnablePrintThread;
};

#define LM_DEBUG 0
#define LM_INFO 1
#define LM_WARNING 2
#define LM_ERROR 3

#undef LOG_DEBUG
#undef LOG_MODEL_DEBUG
#undef LOG_ERROR
#undef LOG_WARNING
#undef LOG_INFO

#ifdef DEBUG
    #define LOG_DEBUG(...) 
    #define LOG_MODEL_DEBUG(model, ...)
#else
    #ifdef HAVE_LOG4CPLUS
        #define LOG_DEBUG(...) LOG4CPLUS_DEBUG_FMT(log4cplus::Logger::getRoot(), __VA_ARGS__)
        #define LOG_MODEL_DEBUG(model, ...) LOG4CPLUS_DEBUG_FMT(log4cplus::Logger::getInstance(model), __VA_ARGS__)
    #else
        #define LOG_DEBUG(...) RabbitCommon::CLog::Instance()->Print(__FILE__, __LINE__, LM_DEBUG, "", __VA_ARGS__)
        #define LOG_MODEL_DEBUG(model, ...) RabbitCommon::CLog::Instance()->Print(__FILE__, __LINE__, LM_DEBUG, model, __VA_ARGS__)
    #endif
#endif //#ifdef DEBUG

#ifdef HAVE_LOG4CPLUS
    #define LOG_ERROR(...) LOG4CPLUS_ERROR_FMT(log4cplus::Logger::getRoot(), __VA_ARGS__)
    #define LOG_WARNING(...) LOG4CPLUS_WARN_FMT(log4cplus::Logger::getRoot(), __VA_ARGS__)
    #define LOG_INFO(...) LOG4CPLUS_INFO_FMT(log4cplus::Logger::getRoot(), __VA_ARGS__)
    
    #define LOG_MODEL_ERROR(model, ...) LOG4CPLUS_ERROR_FMT(log4cplus::Logger::getInstance(model), __VA_ARGS__)
    #define LOG_MODEL_WARNING(model, ...) LOG4CPLUS_WARN_FMT(log4cplus::Logger::getInstance(model), __VA_ARGS__)
    #define LOG_MODEL_INFO(model, ...) LOG4CPLUS_INFO_FMT(log4cplus::Logger::getInstance(model), __VA_ARGS__)
#else
    #define LOG_ERROR(...) RabbitCommon::CLog::Instance()->Print(__FILE__, __LINE__, LM_ERROR, "", __VA_ARGS__)
    #define LOG_WARNING(...) RabbitCommon::CLog::Instance()->Print(__FILE__, __LINE__, LM_WARNING, "", __VA_ARGS__)
    #define LOG_INFO(...) RabbitCommon::CLog::Instance()->Print(__FILE__, __LINE__, LM_INFO, "", __VA_ARGS__)
    
    #define LOG_MODEL_ERROR(model, ...) RabbitCommon::CLog::Instance()->Print(__FILE__, __LINE__, LM_ERROR, model, __VA_ARGS__)
    #define LOG_MODEL_WARNING(model, ...) RabbitCommon::CLog::Instance()->Print(__FILE__, __LINE__, LM_WARNING, model, __VA_ARGS__)
    #define LOG_MODEL_INFO(model, ...) RabbitCommon::CLog::Instance()->Print(__FILE__, __LINE__, LM_INFO, model, __VA_ARGS__)
#endif //HAVE_LOG4CPLUS

} // End namespace RabbitCommon

#endif // RABBIT_COMMON_LOG_H
