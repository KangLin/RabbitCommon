/** @copyright Copyright (c) Kang Lin studio, All Rights Reserved
 *  @author Kang Lin <kl222@126.com>
 *  @abstract Admin authorization
 */

#ifndef RABBITCOMMON_ADMINAUTHORIZATION_P_H
#define RABBITCOMMON_ADMINAUTHORIZATION_P_H

#include "adminauthoriser.h"

namespace RabbitCommon
{

class CAdminAuthorization : public CAdminAuthoriser
{
public:
	bool hasAdminRights() override;
	bool executeAsAdmin(const QString &program, const QStringList &arguments) override;
};

}

#endif // RABBITCOMMON_ADMINAUTHORIZATION_P_H
