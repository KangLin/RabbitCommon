#include "TestUpdater.h"
#include "FrmUpdater.h"
#include "RabbitCommonTools.h"
#include <QApplication>
#include <QTest>

CTestUpdater::CTestUpdater(QObject *parent)
    : QObject{parent}
{}

int CTestUpdater::test_check_redirect_json_file()
{
    QVector<QUrl> urls;
    urls << QUrl::fromLocalFile("data/redirect.json");
    m_pUpdater = new CFrmUpdater(urls);
    connect(m_pUpdater, SIGNAL(sigFinalState()), this, SLOT(slotClose()));
    m_pUpdater->show();
    return 0;
}

void CTestUpdater::slotClose()
{
    CFrmUpdater* p = dynamic_cast<CFrmUpdater*>(this->sender());
    if(p)
        p->close();
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setQuitOnLastWindowClosed(true);

    RabbitCommon::CTools::Instance()->Init();

    CTestUpdater test;
    
    int nRet = test.test_check_redirect_json_file();
    if(nRet)
        return nRet;

    return app.exec();
}
