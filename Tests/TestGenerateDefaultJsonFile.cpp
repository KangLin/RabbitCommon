#include "TestGenerateDefaultJsonFile.h"
#include <QApplication>
#include <QtTest>

#include "FrmUpdater.h"

CTestGenerateDefaultJsonFile::CTestGenerateDefaultJsonFile(QObject *parent)
    : QObject{parent}
{}

void CTestGenerateDefaultJsonFile::test_generate_update_json_file()
{
    CFrmUpdater updater;
    updater.test_generate_update_json_file();
}

void CTestGenerateDefaultJsonFile::test_default_update_json_file()
{
    qDebug() << "CUnitTests::test_default_update_json_file()";
    CFrmUpdater updater;
    updater.test_default_update_json_file();
}

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    CTestGenerateDefaultJsonFile obj;
    // QTEST_MAIN() 会自动增加命令行参数。
    // 所以这里我们直接用 QTest::qExec ，不传递参数给 QApplication
    // See: https://doc.qt.io/qt-6/qtest-overview.html#qt-test-command-line-arguments
    return QTest::qExec(&obj);
}
