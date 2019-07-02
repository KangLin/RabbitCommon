#include "adminauthorization_p.h"
using namespace RabbitCommon;

bool CAdminAuthorization::hasAdminRights()
{
	return false;
}

bool CAdminAuthorization::executeAsAdmin(const QString &program, const QStringList &arguments)
{
	Q_UNUSED(program)
	Q_UNUSED(arguments)
	return false;
}
