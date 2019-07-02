#include "adminauthoriser.h"
#include "adminauthorization_p.h"
#include <QFileInfo>
#include <QApplication>
#include <QProcess>

RabbitCommon::CAdminAuthoriser::CAdminAuthoriser() = default;

RabbitCommon::CAdminAuthoriser::~CAdminAuthoriser() = default;

RabbitCommon::CAdminAuthoriser* RabbitCommon::CAdminAuthoriser::Instance()
{
    RabbitCommon::CAdminAuthoriser* p = nullptr;
    if(!p)
        p = new RabbitCommon::CAdminAuthorization();
    return p;
}

bool RabbitCommon::CAdminAuthoriser::execute(const QString &program, const QStringList &arguments)
{
    if(hasAdminRights())
    {
        QFileInfo fi(program);
        return QProcess::startDetached(fi.absoluteFilePath(),
                                       arguments,
                                       fi.absolutePath());
    } else {
        return executeAsAdmin(program, arguments);
    }
}
