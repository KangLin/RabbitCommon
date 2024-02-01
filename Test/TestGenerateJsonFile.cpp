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
    QVERIFY(info.version.szMinUpdateVersion == "1.9.0");
    QVERIFY(info.version.szTime == "time");
    QVERIFY(info.version.szInfomation == "info");
    QVERIFY(info.version.szHome ==  "home");

    CFrmUpdater::CONFIG_FILE conf_file = info.files[0];
    QVERIFY(conf_file.szSystem == "ubuntu");
    QVERIFY(conf_file.szArchitecture == "x86_64");
    QVERIFY(conf_file.szMd5sum == "aab854ec7cc6831d591237e609a8bf36");
    QVERIFY(conf_file.urls[0] == QUrl("https://github.com/KangLin/RabbitRemoteControl/releases/download/v0.0.26/rabbitremotecontrol_0.0.26_amd64.deb"));
    QVERIFY(conf_file.urls[1] == QUrl("https://sourceforge.net/projects/rabbitremotecontrol/files/v0.0.26/rabbitremotecontrol_0.0.26_amd64.deb/download"));
    QVERIFY(conf_file.szFileName == "rabbitremotecontrol_0.0.26_amd64.deb");
}

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    QCoreApplication::setApplicationName("RabbitCommon");
    CTestGenerateJsonFile obj;
    // QTEST_MAIN() 会自动增加命令行参数。
    // 所以这里我们直接用 QTest::qExec ，不传递参数给 QApplication
    // See: https://doc.qt.io/qt-6/qtest-overview.html#qt-test-command-line-arguments
    return QTest::qExec(&obj);
}
