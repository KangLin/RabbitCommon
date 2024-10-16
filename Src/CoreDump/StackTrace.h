// Copyright Copyright (c) Kang Lin studio, All Rights Reserved
// Author Kang Lin <kl222@126.com>

#ifndef STACKTRACE_H
#define STACKTRACE_H

#pragma once

#include <QObject>

namespace RabbitCommon {

/*!
 * Get call stack
 */
class CCallTrace : public QObject
{
    Q_OBJECT

public:
    /*!
     * \param hMainThread: Main(UI) thread id.
     *        If you are use ShowCoreDialog().
     *        **The call must be in the main(UI) thread.**
     */
    explicit CCallTrace(QObject *parent = nullptr);

    QString GetStack(uint index
#if defined(Q_OS_WIN)
                     = 3
#else
                     = 5
#endif
                     );

#ifdef HAVE_RABBITCOMMON_GUI
public:
    /*!
     * \brief Show core dialog
     * \note When instance the class,
     *       Must be setting the parameter hMainThread of CCallTrace()
     *       to main(UI) thread id.
     */
    void ShowCoreDialog(QString szTitle, QString szContent,
                        QString szDetail, QString szCoreDumpFile);
private Q_SLOTS:
    void slotShowCoreDialog(QString szTitle, QString szContent,
                            QString szDetail, QString szCoreDumpFile);
Q_SIGNALS:
    void sigShowCoreDialog(QString szTitle, QString szContent,
                           QString szDetail, QString szCoreDumpFile);
#endif //#ifdef HAVE_RABBITCOMMON_GUI

private:
    QStringList GetStack(uint index, unsigned int max_frames);

private:
    Qt::HANDLE m_hMainThread;
};

} // namespace RabbitCommon {

#endif // CALLSTACK_H
