#include <QApplication>

#ifdef BUILD_QUIWidget
    #include "QUIWidget/QUIWidget.h"
#endif
#include <QTranslator>
#include <QDir>
#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QDebug>

#include "RabbitCommonTools.h"
#include "RabbitCommonDir.h"
#include "RabbitCommonStyle.h"

#include "MainWindow.h"

int main(int argc, char *argv[])
{
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

    RabbitCommon::CStyle style;
    style.LoadStyle();
    
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
     
    a.exec();
    
#ifndef  BUILD_QUIWidget
     delete m;
#endif
}
