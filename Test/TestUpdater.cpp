#include "TestUpdater.h"
#include "FrmUpdater.h"
#include "RabbitCommonTools.h"
#include <QApplication>
#include <QTest>
#include <QLoggingCategory>

static Q_LOGGING_CATEGORY(log, "RabbitCommon.Test.Updater")

CTestUpdater::CTestUpdater(QObject *parent)
    : QObject{parent}
{}

int CTestUpdater::TestCheckRedirectJson()
{
    QVector<QUrl> urls;
    urls << QUrl::fromLocalFile("data/redirect.json");
    CFrmUpdater* pUpdater = new CFrmUpdater(urls);
    pUpdater->SetVersion("1.15.0");
    connect(pUpdater, SIGNAL(sigFinalState()), this, SLOT(slotClose()));
    pUpdater->show();
    return 0;
}

int CTestUpdater::TestCheckUpdateJson()
{
    QVector<QUrl> urls;
    urls << QUrl::fromLocalFile("data/redirect.json");
    CFrmUpdater* pUpdater = new CFrmUpdater(urls);
    pUpdater->SetVersion("1.9.0");
    connect(pUpdater, SIGNAL(sigFinalState()), this, SLOT(slotClose()));
    pUpdater->show();
    return 0;
}

void CTestUpdater::slotClose()
{
    CFrmUpdater* p = dynamic_cast<CFrmUpdater*>(this->sender());
    //*
    if(p)
        p->close();//*/
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setQuitOnLastWindowClosed(true);

    RabbitCommon::CTools::Instance()->Init();

    CTestUpdater test;
    
    int nRet = test.TestCheckRedirectJson();
    if(nRet)
        return nRet;
    
    nRet = test.TestCheckUpdateJson();
    if(nRet)
        return nRet;

    return app.exec();
}
