#include "RabbitCommonService.h"
#include "RabbitCommonLog.h"
#include <QThread>
#include <QApplication>

class CServiceExample : public RabbitCommon::CService
{
public:
    CServiceExample(){ m_bExit = false; }
    
    int Stop() override {
        m_bExit = true;
        return 0;
    }
    
    // CService interface
protected:
    virtual int OnRun() override {        
        LOG_MODEL_DEBUG("CServiceExample", "CServiceExample run ...");
        while(!m_bExit)
        {
            QThread::msleep(500);
            LOG_MODEL_DEBUG("CServiceExample", "OnRun ......");
        }
        LOG_MODEL_DEBUG("CServiceExample", "CServiceExample exit");
        return 0;
    }
    
private:
    bool m_bExit;
};

int main(int argc, char *argv[])
{
    RabbitCommon::CServiceManage::Instance(new RabbitCommon::CService())->Main(argc, argv);
    return -1;
}
