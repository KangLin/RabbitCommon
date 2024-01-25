#include "UnitTests.h"
#include <QCoreApplication>
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

void CUnitTests::test_generate_json_file()
{
    CFrmUpdater updater;
    QVERIFY(0 == updater.test_generate_default_json_file());
}

void CUnitTests::test_default_json_file()
{
    CFrmUpdater updater;
    updater.test_default_json_file();
}

QTEST_MAIN(CUnitTests)
