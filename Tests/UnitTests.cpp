#include "UnitTests.h"
#include <QApplication>
#include <QtTest>

#include "FrmUpdater.h"

CUnitTests::CUnitTests(QObject *parent)
    : QObject{parent}
{}

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
    qDebug() << "QSysInfo::productType():" << szOS;
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
    qDebug() << "Build architecture:" << QSysInfo::buildCpuArchitecture()
             << "Current architercture:" << QSysInfo::currentCpuArchitecture();
    QVERIFY(QSysInfo::buildCpuArchitecture()
            == QSysInfo::currentCpuArchitecture());
}

void CUnitTests::test_joson()
{
    CFrmUpdater updater;
    QVERIFY(0 == updater.test_json());
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
