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
#include "FrmUpdater/FrmUpdater.h"
#include <string>

int main(int argc, char *argv[])
{
    RabbitCommon::CTools::EnableCoreDump(true);

    QApplication a(argc, argv);
#ifdef RabbitCommon_VERSION
    a.setApplicationVersion(RabbitCommon_VERSION);
#endif
    a.setApplicationName("RabbitCommonTests");
    
    LOG_MODEL_DEBUG("main", "GetDirApplication:%s",
                    RabbitCommon::CDir::Instance()->GetDirApplication().toStdString().c_str());
    LOG_MODEL_DEBUG("main", "GetDirApplicationInstallRoot:%s",
                    RabbitCommon::CDir::Instance()->GetDirApplicationInstallRoot().toStdString().c_str());
    qDebug() << "GetDirUserDocument" << RabbitCommon::CDir::Instance()->GetDirUserDocument();
    
    QTranslator tApp;
    tApp.load(RabbitCommon::CDir::Instance()->GetDirTranslations()
              + "/RabbitCommonTests_" + QLocale::system().name() + ".qm");
    a.installTranslator(&tApp);
    RabbitCommon::CTools::Instance()->Init();

    a.setApplicationDisplayName(QObject::tr("RabbitCommon"));
#ifdef HAVE_GUI
    RabbitCommon::CStyle::Instance()->LoadStyle();
#endif

    QCommandLineParser parser;
    parser.addHelpOption();
    parser.addVersionOption();

    QCommandLineOption noexample(QStringList() << "e" << "no-examples",
                             "no example",
                             "no example");
    parser.addOption(noexample);

#ifdef HAVE_UPDATE
    CFrmUpdater updater;
    updater.GenerateUpdateXml(parser);
#endif
    parser.parse(QApplication::arguments());
    
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
    
    return nRet;
}
