#ifndef RabbitCommon_ADMINAUTHORISER_H
#define RabbitCommon_ADMINAUTHORISER_H

#include <QtCore/qstring.h>
#include <QtCore/qstringlist.h>
#include "rabbitcommon_export.h"

namespace RabbitCommon
{

//! An interface to run programs with elevated rights
class RABBITCOMMON_EXPORT CAdminAuthoriser
{
    Q_DISABLE_COPY(CAdminAuthoriser)
public:
    CAdminAuthoriser();
    virtual ~CAdminAuthoriser();

    static CAdminAuthoriser* Instance();
    virtual bool execute(const QString &program, const QStringList &arguments);

protected:
	//! Tests whether this program already has elevated rights or not
	virtual bool hasAdminRights() = 0;
	//! Runs a program with the given arguments with elevated rights
	virtual bool executeAsAdmin(const QString &program, const QStringList &arguments) = 0;
};

}

#endif // RabbitCommon_ADMINAUTHORISER_H
