#include <QCoreApplication>
#include "GenerateUpdateFile.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

#ifdef RabbitCommon_VERSION
    a.setApplicationVersion(RabbitCommon_VERSION);
#endif
    a.setApplicationName("GenerateUpdateFile");

    CGenerateUpdateFile update;
    update.GenerateUpdateXml();

    a.exec();
    return 0;
}
