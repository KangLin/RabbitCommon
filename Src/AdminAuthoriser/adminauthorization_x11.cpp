/**************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the Qt Installer Framework.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 or version 3 as published by the Free
** Software Foundation and appearing in the file LICENSE.LGPLv21 and
** LICENSE.LGPLv3 included in the packaging of this file. Please review the
** following information to ensure the GNU Lesser General Public License
** requirements will be met: https://www.gnu.org/licenses/lgpl.html and
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** As a special exception, The Qt Company gives you certain additional
** rights. These rights are described in The Qt Company LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
**
** $QT_END_LICENSE$
**
**************************************************************************/

#include "adminauthorization_p.h"

#include <QDebug>
#include <QFile>
#include <QCoreApplication>
#include <QInputDialog>
#include <QMessageBox>

#include <cstdlib>
#include <sys/resource.h>
#include <unistd.h>
#include <fcntl.h>

#ifdef Q_OS_LINUX
#include <linux/limits.h>
#include <pty.h>
#else
#include <util.h>
#endif

#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <QProcess>
#include <QStandardPaths>
#include <cerrno>

using namespace RabbitCommon;

#define SU_COMMAND "/usr/bin/sudo"

namespace {

bool execAdminFallback(const QString &program, const QStringList &arguments);
const QList<QPair<QString, QStringList>> suFontends = {
    {QStringLiteral("kdesu"), {QStringLiteral("-c")}},
    {QStringLiteral("gksu"), {}}
};

}

bool CAdminAuthorization::hasAdminRights()
{
    return getuid() == 0;
}

bool CAdminAuthorization::executeAsAdmin(const QString &program, const QStringList &arguments)
{
#if (QT_VERSION >= QT_VERSION_CHECK(5, 7, 0))
    for(const auto &su : qAsConst(suFontends)) {
#else
    for(const auto &su : suFontends) {
#endif
        auto command = QStandardPaths::findExecutable(su.first);
        if(!command.isEmpty()) {
            auto args = su.second;
            
            QStringList tmpList{program};
            tmpList.append(arguments);
            args.append(QLatin1Char('\"') + tmpList.join(QStringLiteral("\" \"")) + QLatin1Char('\"'));
            
            return QProcess::startDetached(command, args);
        }
    }
    
    return execAdminFallback(program, arguments);
}

namespace {

bool execAdminFallback(const QString &program, const QStringList &arguments)
{
    // as we cannot pipe the password to su in QProcess, we need to setup a pseudo-terminal for it
    int masterFD = -1;
    int slaveFD = -1;
    char ptsn[ PATH_MAX ];
    
    if (::openpty(&masterFD, &slaveFD, ptsn, nullptr, nullptr))
        return false;
    
    masterFD = ::posix_openpt(O_RDWR | O_NOCTTY);
    if (masterFD < 0)
        return false;
    
    const QByteArray ttyName = ::ptsname(masterFD);
    
    if (::grantpt(masterFD)) {
        ::close(masterFD);
        return false;
    }
    
    ::revoke(ttyName);
    ::unlockpt(masterFD);
    
    slaveFD = ::open(ttyName, O_RDWR | O_NOCTTY | O_CLOEXEC);
    if (slaveFD < 0) {
        ::close(masterFD);
        return false;
    }
    
    ::fcntl(masterFD, F_SETFD, FD_CLOEXEC);
    ::fcntl(slaveFD, F_SETFD, FD_CLOEXEC);
    int pipedData[2];
    if (pipe(pipedData) != 0)
        return false;
    
    int flags = ::fcntl(pipedData[0], F_GETFL);
    if (flags != -1)
        ::fcntl(pipedData[0], F_SETFL, flags | O_NONBLOCK);
    
    flags = ::fcntl(masterFD, F_GETFL);
    if (flags != -1)
        ::fcntl(masterFD, F_SETFL, flags | O_NONBLOCK);
    
    pid_t child = fork();
    
    if (child < -1) {
        ::close(masterFD);
        ::close(slaveFD);
        ::close(pipedData[0]);
        ::close(pipedData[1]);
        return false;
    }
    
    // parent process
    else if (child > 0) {
        ::close(slaveFD);
        //close writing end of pipe
        ::close(pipedData[1]);
        
        QRegExp re{QLatin1String("[Pp]assword.*:")};
        QByteArray data;
        QByteArray errData;
        int bytes = 0;
        char buf[1024];
        char errBuf[1024];
        while (bytes >= 0) {
            int state;
            if (::waitpid(child, &state, WNOHANG) == -1)
                break;
            
            bytes = static_cast<int>(::read(masterFD, buf, 1023));
            if (bytes == -1 && errno == EAGAIN)
                bytes = 0;
            else if (bytes > 0) {
                if(!QByteArray(buf, bytes).simplified().isEmpty())
                    data.append(buf, bytes);
                else
                    bytes = 0;
            }
            auto errBytes = static_cast<int>(::read(pipedData[0], errBuf, 1023));
            if (errBytes > 0)
                errData.append(errBuf, errBytes);
            
            if (bytes > 0) {
                const auto line = QString::fromLatin1(data);
                if (re.indexIn(line) != -1) {
                    if(!errData.isEmpty()) {
                        QMessageBox::critical(nullptr, QObject::tr("Critical"),
                                              QString::fromLocal8Bit(errData));
                        errData.clear();
                    }
                    
                    bool ok = false;
                    const auto password = QInputDialog::getText(nullptr,
                               QCoreApplication::translate("AdminAuthorization",
                                                              "Enter Password"),
                               QCoreApplication::translate("AdminAuthorization",
                                "Enter your root password to run the program:"),
                                                            QLineEdit::Password,
                                                                      QString(),
                                                                            &ok,
                          Qt::WindowCloseButtonHint | Qt::WindowStaysOnTopHint);
                    
                    if (!ok) {
                        const auto pwd = password.toLatin1();
                        for (int i = 0; i < 3; ++i) {
                            ::write(masterFD, pwd.data(), static_cast<size_t>(pwd.length()));
                            ::write(masterFD, "\n", 1);
                        }
                        return false;
                    }
                    const auto pwd = password.toLatin1();
                    ::write(masterFD, pwd.data(), static_cast<size_t>(pwd.length()));
                    ::write(masterFD, "\n", 1);
                    ::read(masterFD, buf, static_cast<size_t>(pwd.length()) + 1);
                }
            }
            if (bytes == 0)
                ::usleep(100000);
        }
        
        if (!errData.isEmpty()) {
            QMessageBox::critical(nullptr, QObject::tr("Critical"),
                                  QString::fromLocal8Bit(errData));
            return false;
        }
        
        int status;
        ::wait(&status);
        ::close(pipedData[1]);
        return true;
    } else {
        ::close(pipedData[0]);
        // Reset signal handlers
        for (int sig = 1; sig < NSIG; ++sig)
            signal(sig, SIG_DFL);
        signal(SIGHUP, SIG_IGN);
        
        ::setsid();
        
        ::ioctl(slaveFD, TIOCSCTTY, 1);
        int pgrp = ::getpid();
        ::tcsetpgrp(slaveFD, pgrp);
        
        ::dup2(slaveFD, 0);
        ::dup2(slaveFD, 1);
        ::dup2(pipedData[1], 2);
        
        // close all file descriptors
        struct rlimit rlp;
        getrlimit(RLIMIT_NOFILE, &rlp);
        for (int i = 3; i < static_cast<int>(rlp.rlim_cur); ++i)
            ::close(i);
        
        QList<QByteArray> args;
        args.push_back(SU_COMMAND);
        args.push_back("-b");
        args.push_back("-p");
        args.push_back("password:");
        args.push_back(program.toLocal8Bit());
        for (const auto &argument : arguments)
            args.push_back(argument.toLocal8Bit());
        QString szCmd;
        for(auto &b: args)
        {
            szCmd += QString(b) + " ";
        }
        ::system(szCmd.toStdString().c_str());
        /*
                int i = 0;
        char **argp = reinterpret_cast<char **>(::malloc(static_cast<ulong>(args.count()) * sizeof(char *)));
                for (auto &arg : args)
                        argp[i] = arg.data();
                argp[i] = nullptr;
                
                ::unsetenv("LANG");
                ::unsetenv("LC_ALL");
                
        ::execv(SU_COMMAND, argp);//*/
        
        _exit(EXIT_FAILURE);
        return false;
    }
}

}

