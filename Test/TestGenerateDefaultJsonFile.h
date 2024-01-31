#ifndef CTESTGENERATEDEFAULTJSONFILE_H
#define CTESTGENERATEDEFAULTJSONFILE_H

#include <QObject>

class CTestGenerateDefaultJsonFile : public QObject
{
    Q_OBJECT
public:
    explicit CTestGenerateDefaultJsonFile(QObject *parent = nullptr);
    
private Q_SLOTS:
    void test_generate_update_json_file();
    void test_default_update_json_file();
};

#endif // CTESTGENERATEDEFAULTJSONFILE_H
