#ifndef CUNITTESTS_H
#define CUNITTESTS_H

#include <QObject>

class CUnitTests : public QObject
{
    Q_OBJECT
public:
    explicit CUnitTests(QObject *parent = nullptr);

private Q_SLOTS:
    void initTestCase();
    void initTestCase_data();
    void cleanupTestCase();
    void init();
    void cleanup();

    void test_joson();
    void test_generate_json_file();
    void test_default_json_file();
};

#endif // CUNITTESTS_H
