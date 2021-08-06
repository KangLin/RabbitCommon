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

/**
 * @brief Start run application on reboot
 * @ingroup API
 */
class CRegister
{
public:
    CRegister();
    virtual ~CRegister();
    
    //https://zhidao.baidu.com/question/67815593.html
    static int InstallStartRunCurrentUser();
    static int RemoveStartRunCurrentUser();
    static bool IsStartRunCurrentUser();
    
    static int InstallStartRunOnceCurrentUser();
    static int RemoveStartRunOnceCurrentUser();
    static bool IsStartRunOnceCurrentUser();
    
    static int InstallStartRunServicesCurrentUser();
    static int RemoveStartRunServicesCurrentUser();
    static bool IsStartRunServicesCurrentUser();
    
    static int InstallStartRunServicesOnceCurrentUser();
    static int RemoveStartRunServicesOnceCurrentUser();
    static bool IsStartRunServicesOnceCurrentUser();
    
    static int InstallStartRun();
    static int RemoveStartRun();
    static bool IsStartRun();
    
    static int InstallStartRunOnce();
    static int RemoveStartRunOnce();
    static bool IsStartRunOnce();
    
    static int InstallStartRunServices();
    static int RemoveStartRunServices();
    static bool IsStartRunServices();
    
    static int InstallStartRunServicesOnce();
    static int RemoveStartRunServicesOnce();
    static bool IsStartRunServicesOnce();
    
    static int InstallStartRun(const QString &szReg,
                               const QString &szName = QString(),
                               const QString &szPath = QString());
    static int RemoveStartRun(const QString &szReg,
                              const QString &szName = QString());
    static bool IsStartRun(const QString &szReg,
                           const QString &szName = QString());

    static int SetRegister(const QString &reg, const QString &name, const QString &path);
    static int RemoveRegister(const QString &reg, const QString &name);
    static bool IsRegister(const QString &reg, const QString &name);
};

} //namespace RabbitCommon
#endif // RABBITCOMMON_REGISTER_H_KANGLIN_2019_07_05
