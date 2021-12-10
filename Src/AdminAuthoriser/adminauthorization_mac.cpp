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

#include <QtCore/QStringList>
#include <QtCore/QVector>

#include <Security/Authorization.h>
#include <Security/AuthorizationTags.h>
#include <unistd.h>

using namespace RabbitCommon;

bool CAdminAuthorization::hasAdminRights()
{
	return geteuid() == 0;
}

bool CAdminAuthorization::executeAsAdmin(const QString &program, const QStringList &arguments)
{
	AuthorizationRef authorizationRef;
	OSStatus status = AuthorizationCreate(NULL, kAuthorizationEmptyEnvironment,
		kAuthorizationFlagDefaults, &authorizationRef);
	if (status != errAuthorizationSuccess)
		return false;

	AuthorizationItem item = { kAuthorizationRightExecute, 0, 0, 0 };
	AuthorizationRights rights = { 1, &item };
	const AuthorizationFlags flags = kAuthorizationFlagDefaults | kAuthorizationFlagInteractionAllowed
		| kAuthorizationFlagPreAuthorize | kAuthorizationFlagExtendRights;

	status = AuthorizationCopyRights(authorizationRef, &rights, kAuthorizationEmptyEnvironment,
		flags, 0);
	if (status != errAuthorizationSuccess)
		return false;

	QVector<char *> args;
	QVector<QByteArray> utf8Args;
	for(auto argument : arguments) {
		utf8Args.push_back(argument.toUtf8());
		args.push_back(utf8Args.last().data());
	}
	args.push_back(0);

	const QByteArray utf8Program = program.toUtf8();
	status = AuthorizationExecuteWithPrivileges(authorizationRef, utf8Program.data(),
		kAuthorizationFlagDefaults, args.data(), 0);

	AuthorizationFree(authorizationRef, kAuthorizationFlagDestroyRights);
	return status == errAuthorizationSuccess;
}
