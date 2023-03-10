/** @copyright Copyright (c) Kang Lin studio, All Rights Reserved
 *  @author Kang Lin <kl222@126.com>
 *  @abstract Register
 */

#ifndef RABBITCOMMON_REGISTER_H_KANGLIN_2019_07_05
#define RABBITCOMMON_REGISTER_H_KANGLIN_2019_07_05

#pragma once

#include "rabbitcommon_export.h"

#include <QString>

namespace RabbitCommon {

/*!
 * \brief Start run application on system boot
 * \ingroup INTERNAL_API
 */
class CRegister
{
public:
    CRegister();
    virtual ~CRegister();
    
    //https://zhidao.baidu.com/question/67815593.html
    static int InstallStartRunCurrentUser(QString szName = QString(), QString szPath = QString());
    static int RemoveStartRunCurrentUser(QString szName = QString());
    static bool IsStartRunCurrentUser(QString szName = QString());
    
    static int InstallStartRunOnceCurrentUser(QString szName = QString(), QString szPath = QString());
    static int RemoveStartRunOnceCurrentUser(QString szName = QString());
    static bool IsStartRunOnceCurrentUser(QString szName = QString());
    
    static int InstallStartRunServicesCurrentUser(QString szName = QString(), QString szPath = QString());
    static int RemoveStartRunServicesCurrentUser(QString szName = QString());
    static bool IsStartRunServicesCurrentUser(QString szName = QString());
    
    static int InstallStartRunServicesOnceCurrentUser(QString szName = QString(), QString szPath = QString());
    static int RemoveStartRunServicesOnceCurrentUser(QString szName = QString());
    static bool IsStartRunServicesOnceCurrentUser(QString szName = QString());
    
    static int InstallStartRun(QString szName = QString(), QString szPath = QString());
    static int RemoveStartRun(const QString &szName = QString());
    static bool IsStartRun(const QString &szName = QString());
    
    static int InstallStartRunOnce(QString szName = QString(), QString szPath = QString());
    static int RemoveStartRunOnce(QString szName = QString());
    static bool IsStartRunOnce(QString szName = QString());
    
    static int InstallStartRunServices(QString szName = QString(), QString szPath = QString());
    static int RemoveStartRunServices(QString szName = QString());
    static bool IsStartRunServices(QString szName = QString());
    
    static int InstallStartRunServicesOnce(QString szName = QString(), QString szPath = QString());
    static int RemoveStartRunServicesOnce(QString szName = QString());
    static bool IsStartRunServicesOnce(QString szName = QString());
    
    static int InstallStartRun(const QString &szKey,
                               const QString &szName = QString(),
                               const QString &szPath = QString());
    static int RemoveStartRun(const QString &szKey,
                              const QString &szName = QString());
    static bool IsStartRun(const QString &szKey,
                           const QString &szName = QString());

    static int SetRegister(const QString &reg, const QString &name, const QString &path);
    static int RemoveRegister(const QString &reg, const QString &name);
    static bool IsRegister(const QString &reg, const QString &name);
    
    /*!
     * \brief Generate the name of desktop file
     * \param szPath: the path of desktop file.
     * \param szName: the name of desktop file.
     *            Default: "org.Rabbit." + qApp->applicationName() + ".desktop"
     */
    static QString GetDesktopFileName(const QString &szPath = QString(),
                                      const QString &szName = QString());
    /*!
     * \brief GetDesktopFileLink
     * \param szName: the link of desktop file.
     *            Default: "org.Rabbit." + qApp->applicationName() + ".desktop"
     * \param bAllUser:
     *          - true: all user
     *          - false: current user
     */
    static QString GetDesktopFileLink(const QString &szName = QString(),
                                      bool bAllUser = false);
};

} //namespace RabbitCommon
#endif // RABBITCOMMON_REGISTER_H_KANGLIN_2019_07_05
