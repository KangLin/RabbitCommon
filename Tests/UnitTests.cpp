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
