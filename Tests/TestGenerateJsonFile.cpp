#include <QApplication>
#include <QtTest>

#include "FrmUpdater.h"
#include "TestGenerateJsonFile.h"

CTestGenerateJsonFile::CTestGenerateJsonFile() : QObject()
{}

// set command line in Tests/CMakeLists.txt
void CTestGenerateJsonFile::test_generate_update_json_file()
{
    CFrmUpdater updater;
    QVERIFY(0 == updater.GenerateUpdateJson());
}

void CTestGenerateJsonFile::test_json_file()
{
    CFrmUpdater updater;
    QFile file("test.json");
    QVERIFY(file.exists());
    CFrmUpdater::CONFIG_INFO info;
    QVERIFY(0 == updater.GetConfigFromFile("test.json", info));
    QVERIFY(info.version.szVerion == "2.0.0");
    QVERIFY(info.version.szMinUpdateVersion == "1.5.7");
    QVERIFY(info.version.szTime == "time");
    QVERIFY(info.version.szInfomation == "info");
    QVERIFY(info.version.szHome ==  "home");
    
    CFrmUpdater::CONFIG_FILE conf_file = info.files[0];
    QVERIFY(conf_file.szSystem == "windows");
    QVERIFY(conf_file.szArchitecture == "x86");
    QVERIFY(conf_file.szMd5sum == "asbdasersdfasdfawer");
    QVERIFY(conf_file.urls[0] == QUrl("github.com"));
    QVERIFY(conf_file.szFileName == "rabbitcommon");
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
