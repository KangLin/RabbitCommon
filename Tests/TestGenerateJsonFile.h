#ifndef __CTestGenerateJsonFile_H_KL_2024_01_25__
#define __CTestGenerateJsonFile_H_KL_2024_01_25__

#include <QObject>

class CTestGenerateJsonFile : public QObject
{
    Q_OBJECT
    
public:
    explicit CTestGenerateJsonFile();
    
private Q_SLOTS:
    void test_generate_json_file();
    void test_json_file();
};

#endif // __CTestGenerateJsonFile_H_KL_2024_01_25__
