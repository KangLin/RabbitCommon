// Copyright Copyright (c) Kang Lin studio, All Rights Reserved
// Author Kang Lin <kl222@126.com>

#ifndef RabbitCommon_ADMINAUTHORISER_H
#define RabbitCommon_ADMINAUTHORISER_H

#include <QtCore/qstring.h>
#include <QtCore/qstringlist.h>

namespace RabbitCommon
{

/*!
 * 
 * \~chinese 使用提升的权限运行程序的接口
 * \note 此类仅内部使用
 *
 * \~english An interface to run programs with elevated rights
 * \note This class is intended for internal use only
 * 
 * \~
 * \ingroup INTERNAL_API
 */
class CAdminAuthoriser
{
    Q_DISABLE_COPY(CAdminAuthoriser)
public:
    CAdminAuthoriser();
    virtual ~CAdminAuthoriser();

    static CAdminAuthoriser* Instance();
    virtual bool execute(const QString &program, const QStringList &arguments);
    //! Tests whether this program already has elevated rights or not
    virtual bool hasAdminRights() = 0;
    
    void SetDetached(bool bDetached);
    bool GetDetached();

protected:
	//! Runs a program with the given arguments with elevated rights
	virtual bool executeAsAdmin(const QString &program, const QStringList &arguments) = 0;
    
    bool m_bDetached;
};

}

#endif // RabbitCommon_ADMINAUTHORISER_H
