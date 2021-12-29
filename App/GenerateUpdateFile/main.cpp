#include <QCoreApplication>
#include "GenerateUpdateFile.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    
    CGenerateUpdateFile update;
    update.GenerateUpdateXml();

    return a.exec();
}
