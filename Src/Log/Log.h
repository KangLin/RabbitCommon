/*! \copyright Copyright (c) Kang Lin studio, All Rights Reserved
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
#include <QFileSystemWatcher>

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
    virtual ~CLog();

    QString GetLogConfigureFile();
    QString GetLogFile();
    QString GetLogDir();
    
    int SetFilter(const QString &szInclude, const QString &szExclude);
    int GetFilter(QString &szInclude, QString &szExclude);

private:
    CLog();

    QString m_szConfigureFile;

    QString m_szPath;
    QString m_szName;
    QString m_szDateFormat;
    qint64 m_nLength;   // Unit: byte
    int m_nCount;
    QTimer m_Timer;
    QFileSystemWatcher m_Watcher;

    #if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
        static void myMessageOutput(QtMsgType type,
                                const QMessageLogContext &context,
                                const QString &msg);
    #else
        static void myMessageOutput(QtMsgType, const char *);
    #endif
    void checkFileCount();
    bool checkFileLength();
    bool checkFileName();
    QString getFileName();
    QString getNextFileName(const QString szFile);
    QString getBaseName();
    int LoadConfigure(const QString &szFile);

private Q_SLOTS:
    void slotTimeout();
    void slotFileChanged(const QString &szPath);
};

} // End namespace RabbitCommon

#endif // RABBIT_COMMON_LOG_H
