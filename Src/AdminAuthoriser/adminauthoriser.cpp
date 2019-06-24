#include "adminauthoriser.h"
#include "adminauthorization_p.h"
#include <QFileInfo>
#include <QApplication>
#include <QProcess>

RabbitCommon::AdminAuthoriser::AdminAuthoriser() = default;

RabbitCommon::AdminAuthoriser::~AdminAuthoriser() = default;

RabbitCommon::AdminAuthoriser* RabbitCommon::AdminAuthoriser::Instance()
{
    RabbitCommon::AdminAuthoriser* p = nullptr;
    if(!p)
        p = new RabbitCommon::AdminAuthorization();
    return p;
}

bool RabbitCommon::AdminAuthoriser::execute(const QString &program, const QStringList &arguments)
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
