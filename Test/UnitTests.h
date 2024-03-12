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

    void testDownloadFileNoExistLocalFile();
    void testDownloadFileExistLocalFile();

    void testCFrmUpdaterCompareVersion();

    void test_os();
    void test_architecture();
    void test_joson();
    
    void test_redirect_json_file();
};

#endif // CUNITTESTS_H
