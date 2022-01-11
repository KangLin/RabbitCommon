#include <QCoreApplication>
#include "GenerateUpdateFile.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

#ifdef BUILD_VERSION
    a.setApplicationVersion(BUILD_VERSION);
#endif
    a.setApplicationName("GenerateUpdateFile");

    CGenerateUpdateFile update;
    update.GenerateUpdateXml();

    a.exec();
    return 0;
}
