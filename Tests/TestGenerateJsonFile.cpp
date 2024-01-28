#include <QApplication>
#include <QtTest>

#include "FrmUpdater.h"
#include "TestGenerateJsonFile.h"

CTestGenerateJsonFile::CTestGenerateJsonFile() : QObject()
{}

void CTestGenerateJsonFile::test_generate_update_json_file()
{
    CFrmUpdater updater;
    updater.test_generate_update_json_file();
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
    // QTEST_MAIN() 会自动增加命令行参数。
    // 所以这里我们直接用 QTest::qExec ，不传递参数给 QApplication
    // See: https://doc.qt.io/qt-6/qtest-overview.html#qt-test-command-line-arguments
    return QTest::qExec(&obj);
}
