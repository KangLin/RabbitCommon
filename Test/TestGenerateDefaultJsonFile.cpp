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
    QVERIFY(0 == updater.GenerateUpdateJson());
}

void CTestGenerateDefaultJsonFile::test_default_update_json_file()
{
    qDebug() << "CUnitTests::test_default_update_json_file()";
    CFrmUpdater updater;
    QFile file("update.json");
    QVERIFY(file.exists());
    
    CFrmUpdater::CONFIG_INFO info;
    QVERIFY(0 == updater.GetConfigFromFile("update.json", info));
    QVERIFY(info.version.szVerion == updater.m_szCurrentVersion);
    QVERIFY(info.version.szMinUpdateVersion == updater.m_szCurrentVersion);
    QVERIFY(info.version.szInfomation == qApp->applicationName() + " " + updater.m_szCurrentVersion);
    QVERIFY(info.version.szHome ==  "https://github.com/KangLin/" + qApp->applicationName());
    
    CFrmUpdater::CONFIG_FILE conf_file = info.files[0];
    QVERIFY(conf_file.szSystem == QSysInfo::productType());
    QVERIFY(conf_file.szArchitecture == QSysInfo::buildCpuArchitecture());
    QVERIFY(conf_file.szMd5sum.isEmpty());
    
    QString szFileName;
#if defined (Q_OS_WIN)
    szFileName = qApp->applicationName() + "_" + updater.m_szCurrentVersion + "_Setup" + ".exe";
#elif defined(Q_OS_ANDROID)
    szFileName = qApp->applicationName().toLower() + "_" + updater.m_szCurrentVersion + ".apk";
#elif defined(Q_OS_LINUX)
    QFileInfo f(qApp->applicationFilePath());
    if(f.suffix().compare("AppImage", Qt::CaseInsensitive))
    {
        QString szVersion = updater.m_szCurrentVersion;
        szVersion.replace("v", "", Qt::CaseInsensitive);
        szFileName = qApp->applicationName().toLower()
                     + "_" + szVersion + "_amd64.deb";
    } else {
        szFileName = qApp->applicationName()
                     + "_" + updater.m_szCurrentVersion + ".tar.gz";
    }
#endif
    QVERIFY(conf_file.szFileName == szFileName);
}

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    QCoreApplication::setApplicationName("RabbitCommon");
    CTestGenerateDefaultJsonFile obj;
    // QTEST_MAIN() 会自动增加命令行参数。
    // 所以这里我们直接用 QTest::qExec ，不传递参数给 QApplication
    // See: https://doc.qt.io/qt-6/qtest-overview.html#qt-test-command-line-arguments
    return QTest::qExec(&obj);
}
