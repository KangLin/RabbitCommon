/** \copyright Copyright (c) Kang Lin studio, All Rights Reserved
 *  \author Kang Lin <kl222@126.com>
 *  \abstract log
 *  The log interface is deprecated.
 *  The file will be deleted from version 2.0.0
 *  Please use qDebug etc instead
 */

#ifndef RABBIT_COMMON_LOG_H
#define RABBIT_COMMON_LOG_H

#pragma once
#include <QObject>
#include <QLoggingCategory>
#include <QMutex>
#include <QFile>
#include <QTextStream>
#include <QTimer>

#include "rabbitcommon_export.h"

namespace RabbitCommon {

#define LM_DEBUG 0
#define LM_INFO 1
#define LM_WARNING 2
#define LM_ERROR 3

#ifdef DEBUG
    #define LOG_MODEL_DEBUG(model, ...) RabbitCommon::CLog::Instance()->Print(__FILE__, __LINE__, Q_FUNC_INFO, LM_DEBUG, model, __VA_ARGS__)
#else
    #define LOG_MODEL_DEBUG(model, ...)
#endif //#ifdef DEBUG

#define LOG_MODEL_ERROR(model, ...) RabbitCommon::CLog::Instance()->Print(__FILE__, __LINE__, Q_FUNC_INFO, LM_ERROR, model, __VA_ARGS__)
#define LOG_MODEL_WARNING(model, ...) RabbitCommon::CLog::Instance()->Print(__FILE__, __LINE__, Q_FUNC_INFO, LM_WARNING, model, __VA_ARGS__)
#define LOG_MODEL_INFO(model, ...) RabbitCommon::CLog::Instance()->Print(__FILE__, __LINE__, Q_FUNC_INFO, LM_INFO, model, __VA_ARGS__)

#ifdef HAVE_GUI
    //Q_DECL_DEPRECATED_X("Please use RabbitCommon::CTools::OpenLogConfigureFile instead")
    RABBITCOMMON_EXPORT void OpenLogConfigureFile();
    //Q_DECL_DEPRECATED_X("Please use RabbitCommon::CTools::OpenLogFile instead")
    RABBITCOMMON_EXPORT void OpenLogFile();
    //Q_DECL_DEPRECATED_X("Please use RabbitCommon::CTools::OpenLogFolder instead")
    RABBITCOMMON_EXPORT void OpenLogFolder();
#endif

/*!
 * \note USER DON'T USE CLog!!!
 */
class RABBITCOMMON_EXPORT CLog : QObject
{
    Q_OBJECT

public:
    static CLog* Instance();
    int EnablePrintThread(bool bPrint);

    QString OpenLogConfigureFile();
    QString GetLogFile();
    QString GetLogDir();

    /**
     * @brief 日志输出
     * @param pszFile:打印日志处文件名
     * @param nLine:打印日志处行号
     * @param nLevel:打印日志错误级别
     * @param pszModelName:打印日志的模块范围
     * @param pFormatString:格式化字符串
     * @return 
     */
    Q_DECL_DEPRECATED_X("Please use qDebug and log4Qt instead.")
    int Print(const char *pszFile, int nLine, const char* pszFunction, int nLevel,
            const char* pszModelName, const char *pFormatString, ...);

private:
    CLog();
    virtual ~CLog();

    bool m_bEnablePrintThread;
    QString m_szConfigureFile;

    QString m_szPath;
    QString m_szFileFormat;
    QFile m_File;
    QMutex m_Mutex;
    qint64 m_nLength;   // Unit: byte
    int m_nCount;
    QTimer m_Timer;
    #if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
        static void myMessageOutput(QtMsgType type,
                                const QMessageLogContext &context,
                                const QString &msg);
    #else
        static void myMessageOutput(QtMsgType, const char *);
    #endif
    void checkFileCount();
    int checkFileLength();
    QString getFileName();
    QString getNextFileName(const QString szFile);
private Q_SLOTS:
    void slotTimeout();
};

extern RABBITCOMMON_EXPORT QLoggingCategory Logger;

} // End namespace RabbitCommon

#endif // RABBIT_COMMON_LOG_H
