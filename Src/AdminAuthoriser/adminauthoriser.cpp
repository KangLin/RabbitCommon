//! @copyright Copyright (c) Kang Lin studio, All Rights Reserved
//! @author Kang Lin <kl222@126.com>

#include "adminauthoriser.h"
#include "adminauthorization_p.h"
#include <QFileInfo>
#include <QProcess>

RabbitCommon::CAdminAuthoriser::CAdminAuthoriser() : m_bDetached(true)
{}

RabbitCommon::CAdminAuthoriser::~CAdminAuthoriser() = default;

RabbitCommon::CAdminAuthoriser* RabbitCommon::CAdminAuthoriser::Instance()
{
    static RabbitCommon::CAdminAuthoriser* p = nullptr;
    if(!p)
        p = new RabbitCommon::CAdminAuthorization();
    return p;
}

bool RabbitCommon::CAdminAuthoriser::execute(const QString &program, const QStringList &arguments)
{
    if(hasAdminRights())
    {
        QFileInfo fi(program);
        if(GetDetached())
            return QProcess::startDetached(fi.absoluteFilePath(),
                                       arguments,
                                       fi.absolutePath());
        else
            return QProcess::execute(fi.absoluteFilePath(),
                                           arguments);
    } else {
        return executeAsAdmin(program, arguments);
    }
}

void RabbitCommon::CAdminAuthoriser::SetDetached(bool bDetached)
{
    m_bDetached = bDetached;
}

bool RabbitCommon::CAdminAuthoriser::GetDetached()
{
    return m_bDetached;
}
