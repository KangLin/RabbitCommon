#include <QApplication>
#include <QtTest>

#include "FrmUpdater.h"
#include "TestGenerateJsonFile.h"

CTestGenerateJsonFile::CTestGenerateJsonFile() : QObject()
{}

void CTestGenerateJsonFile::test_generate_json_file()
{
    CFrmUpdater updater;
    QVERIFY(0 == updater.test_generate_json_file());
}

void CTestGenerateJsonFile::test_json_file()
{
    CFrmUpdater updater;
    updater.test_json_file();
}

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    CTestGenerateJsonFile obj;
    return QTest::qExec(&obj);
}
