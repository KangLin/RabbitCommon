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

void CUnitTests::test_updater_joson()
{
    CFrmUpdater updater;
    QVERIFY(0 == updater.test_json());
}

QTEST_MAIN(CUnitTests)
