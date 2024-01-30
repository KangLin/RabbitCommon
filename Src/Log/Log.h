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

namespace RabbitCommon {

#ifdef HAVE_RABBITCOMMON_GUI
    void OpenLogConfigureFile();
    void OpenLogFile();
    void OpenLogFolder();
#endif

/*!
 * \note USER DON'T USE CLog!!!
 * \ingroup INTERNAL_API
 */
class CLog : QObject
{
    Q_OBJECT

public:
    static CLog* Instance();

    QString OpenLogConfigureFile();
    QString GetLogFile();
    QString GetLogDir();
    
    int SetFilter(const QString &szInclude, const QString &szExclude);
    int GetFilter(QString &szInclude, QString &szExclude);

private:
    CLog();
    virtual ~CLog();

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
    bool checkFileLength();
    QString getFileName();
    QString getNextFileName(const QString szFile);
private Q_SLOTS:
    void slotTimeout();
};

} // End namespace RabbitCommon

#endif // RABBIT_COMMON_LOG_H
