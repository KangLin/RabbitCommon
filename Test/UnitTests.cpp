#include "UnitTests.h"
#include <QApplication>
#include <QtTest>

#include "FrmUpdater.h"

static Q_LOGGING_CATEGORY(log, "RabbitCommon.Test.Updater")

CUnitTests::CUnitTests(QObject *parent)
    : QObject{parent}
{
    QCoreApplication::setApplicationName("RabbitCommon");
}

void CUnitTests::initTestCase()
{}

void CUnitTests::initTestCase_data()
{}

void CUnitTests::cleanupTestCase()
{}

void CUnitTests::init()
{}

void CUnitTests::cleanup()
{}

void CUnitTests::test_os()
{
    QString szOS = QSysInfo::productType();
    qDebug(log) << "QSysInfo::productType():" << szOS;
#if defined(Q_OS_WINDOWS) || defined(Q_OS_CYGWIN)
    QVERIFY("windows" == szOS);
#elif defined(Q_OS_ANDROID)
    QVERIFY("android" == szOS);
#elif defined(Q_OS_MACOS)
    #if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        QVERIFY("macos" == szOS);
    #else
        QVERIFY("osx" == szOS);
    #endif
#elif defined(Q_OS_LINUX)
    QVERIFY("ubuntu" == szOS || "debian" == szOS);
#elif defined(Q_OS_UNIX)
    QVERIFY("osx" == szOS);
#endif
}

void CUnitTests::test_architecture()
{
    qDebug(log) << "Build architecture:" << QSysInfo::buildCpuArchitecture()
             << "Current architercture:" << QSysInfo::currentCpuArchitecture();
    QVERIFY(QSysInfo::buildCpuArchitecture()
            == QSysInfo::currentCpuArchitecture());
}

void CUnitTests::test_joson()
{
    QJsonArray url_windows;
    url_windows.append("github.com/windows");
    url_windows.append("gitlab.com/windows");
    
    QJsonObject file_windows;
    file_windows.insert("os", "windows");
    file_windows.insert("os_min_version", "7");
    file_windows.insert("arch", "x86");
    file_windows.insert("arch_version", "1");
    file_windows.insert("url", url_windows);
    
    QJsonArray url_linux;
    url_linux.append("gitlab.com/linux");
    url_linux.append("github.com/linux");
    
    QJsonObject file_linux;
    file_linux.insert("os", "linux");
    file_linux.insert("arch", "x64");
    file_linux.insert("url", url_linux);
    
    QJsonArray files;
    files.append(file_windows);
    files.append(file_linux);
    
    QJsonObject version;
    version.insert("version", "2.0.0");
    version.insert("min_update_version", "1.0.0");
    version.insert("info", "RabbitCommon");
    version.insert("time", QDateTime::currentDateTime().toString());
    version.insert("home", "github.com/kanglin/rabbitcommon");
    version.insert("force", false);
    version.insert("files", files);
    
    QJsonDocument updater, doc1;
    
    updater.setObject(version);
    qDebug(log) << updater.toJson();
    QJsonParseError err;
    doc1 = QJsonDocument::fromJson(updater.toJson(), &err);
    qDebug(log) << doc1;
    QVERIFY(updater == doc1);
}

// See: data/redirect.json
void CUnitTests::test_redirect_json_file()
{
    CFrmUpdater updater;
    QVector<CFrmUpdater::CONFIG_REDIRECT> conf;
    int nRet = 0;
    nRet = updater.GetRedirectFromFile("update.json", conf);
    QVERIFY(1 == nRet);
    nRet = updater.GetRedirectFromFile("data/redirect.json", conf);
    QVERIFY(0 == nRet);
    QVERIFY(conf.length() == 4);

    QVERIFY(conf[0].szVersion == "v2.0.0");
    QVERIFY(conf[0].szMinUpdateVersion == "v0.0.26");
    QVERIFY(conf[0].files.isEmpty());

    QVERIFY(conf[1].szVersion == "v0.0.26");
    QVERIFY(conf[1].szMinUpdateVersion == "v0.0.20");
    QVERIFY(conf[1].files.length() == 1);
    QVERIFY(conf[1].files[0].szSystem.isEmpty());
    QVERIFY(conf[1].files[0].szArchitecture.isEmpty());
    QVERIFY(conf[1].files[0].urls.size() == 1);
    QVERIFY(conf[1].files[0].urls[0] == QUrl::fromLocalFile("data/update.json"));

    QVERIFY(conf[2].szVersion == "v0.0.20");
    QVERIFY(conf[2].szMinUpdateVersion == "v0.0.10");
    QVERIFY(conf[2].files.length() == 2);
    QVERIFY(conf[2].files[0].szSystem == "windows");
    QVERIFY(conf[2].files[0].szArchitecture == "x86");
    QVERIFY(conf[2].files[0].urls.size() == 1);
    QVERIFY(conf[2].files[0].urls[0] == QUrl::fromLocalFile("data/update_windows.json"));
    QVERIFY(conf[2].files[1].szSystem == "ubuntu");
    QVERIFY(conf[2].files[1].szArchitecture == "x86_64");
    QVERIFY(conf[2].files[1].urls.size() == 1);
    QVERIFY(conf[2].files[1].urls[0] == QUrl::fromLocalFile("data/update_ubuntu.json"));

    QVERIFY(conf[3].szVersion == "v0.0.10");
    QVERIFY(conf[3].szMinUpdateVersion.isEmpty());
    QVERIFY(conf[3].files.length() == 2);
    QVERIFY(conf[3].files[0].szSystem == "windows");
    QVERIFY(conf[3].files[0].szArchitecture == "x86");
    QVERIFY(conf[3].files[0].urls.size() == 2);
    QVERIFY(conf[3].files[1].szSystem == "ubuntu");
    QVERIFY(conf[3].files[1].szArchitecture == "x86_64");
    QVERIFY(conf[3].files[1].urls.size() == 2);
}

QTEST_MAIN(CUnitTests)
/*
int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    CUnitTests obj;
    // See: https://doc.qt.io/qt-6/qtest-overview.html#qt-test-command-line-arguments
    return QTest::qExec(&obj, QStringList() << "UnitTests" << "-xml");
}//*/