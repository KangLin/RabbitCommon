/** @copyright Copyright (c) Kang Lin studio, All Rights Reserved
 *  @author Kang Lin(kl222@126.com)
 */

#include <QApplication>

#ifdef BUILD_QUIWidget
    #include "QUIWidget/QUIWidget.h"
#endif
#include <QTranslator>
#include <QDir>
#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QDebug>
#include <QThread>

#include "RabbitCommonTools.h"
#include "RabbitCommonDir.h"
#include "RabbitCommonStyle.h"
#include "RabbitCommonLog.h"
#include "MainWindow.h"

#include "RabbitCommonService.h"

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
            QThread::msleep(500);
        LOG_MODEL_DEBUG("CServiceExample", "CServiceExample exit");
        qApp->quit();
        return 0;
    }
    
private:
    bool m_bExit;
};

int main(int argc, char *argv[])
{
#if defined (_DEBUG)
    Q_INIT_RESOURCE(translations_RabbitCommonApp);
#endif
    QApplication a(argc, argv);
    a.setApplicationVersion(BUILD_VERSION);
    a.setApplicationName("RabbitCommonApp");
    
    QCommandLineParser parser;
    parser.addHelpOption();
    parser.addVersionOption();

    QCommandLineOption noexample(QStringList() << "e" << "no-examples",
                             "no example",
                             "no example");
    parser.addOption(noexample);
    
    parser.process(QApplication::arguments());

    qDebug() << "GetDirApplication:" << RabbitCommon::CDir::Instance()->GetDirApplication();
    qDebug() << "GetDirApplicationInstallRoot:" << RabbitCommon::CDir::Instance()->GetDirApplicationInstallRoot();
    qDebug() << "GetDirUserDocument" << RabbitCommon::CDir::Instance()->GetDirUserDocument();
    
    QTranslator tApp;
    tApp.load(RabbitCommon::CDir::Instance()->GetDirTranslations()
              + "/RabbitCommonApp_" + QLocale::system().name() + ".qm");
    a.installTranslator(&tApp);
    RabbitCommon::CTools::Instance()->Init();
    
    a.setApplicationDisplayName(QObject::tr("RabbitCommon"));

    RabbitCommon::CStyle::Instance()->LoadStyle();
    
    RabbitCommon::CServiceManage::Instance(new CServiceExample())->Main(argc, argv);
    return -1;
    
    MainWindow *m = new MainWindow();
    m->setWindowIcon(QIcon(":/icon/RabbitCommon/App"));
    m->setWindowTitle(a.applicationDisplayName());
    
#ifdef BUILD_QUIWidget
    QUIWidget quiwidget;
   // quiwidget.setPixmap(QUIWidget::Lab_Ico, ":/icon/RabbitCommon/App");
   // quiwidget.setTitle(QObject::tr("Rabbit Common - QUIWidget"));
    quiwidget.setMainWidget(m);
    quiwidget.setAlignment(Qt::AlignCenter);
    quiwidget.setVisible(QUIWidget::BtnMenu, true);
    quiwidget.show();
#else
    m->show();
#endif
     
    int nRet = a.exec();
    
#ifndef  BUILD_QUIWidget
     delete m;
#endif
    a.removeTranslator(&tApp);
#if defined (_DEBUG)
    Q_CLEANUP_RESOURCE(translations_RabbitCommonApp);
#endif
    
    return nRet;
}
