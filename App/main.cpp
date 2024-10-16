/** @copyright Copyright (c) Kang Lin studio, All Rights Reserved
 *  @author Kang Lin(kl222@126.com)
 */

#include <QLoggingCategory>
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
#include "MainWindow.h"
#ifdef HAVE_UPDATE
    #include "FrmUpdater.h"
#endif

static Q_LOGGING_CATEGORY(log, "RabbitCommon.main")

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
#ifdef RabbitCommon_VERSION
    a.setApplicationVersion(RabbitCommon_VERSION);
#endif
    a.setApplicationName("RabbitCommon");

    RabbitCommon::CTools::Instance()->Init();
    RabbitCommon::CTools::Instance()->InstallTranslator("RabbitCommonApp");

    qDebug(log) << "GetDirApplication:"
                    << RabbitCommon::CDir::Instance()->GetDirApplication();
    qDebug(log) << "GetDirApplicationInstallRoot:"
              << RabbitCommon::CDir::Instance()->GetDirApplicationInstallRoot();
    qInfo(log) << "GetDirUserDocument"
                   << RabbitCommon::CDir::Instance()->GetDirUserDocument();

    // Must after install translator
    a.setApplicationDisplayName(QObject::tr("RabbitCommon"));

    // [Use CFrmUpdater GenerateUpdateJson]
    QCommandLineParser parser;
    parser.addHelpOption();
    parser.addVersionOption();

    QCommandLineOption noexample(QStringList() << "e" << "no-examples",
                             "no example",
                             "no example");
    parser.addOption(noexample);

#ifdef HAVE_UPDATE
    CFrmUpdater* updater = new CFrmUpdater();
    if(a.arguments().length() > 1) {
        updater->GenerateUpdateJson(parser);
        parser.process(QApplication::arguments());
        return 0;
    }
#endif

    // [Use CFrmUpdater GenerateUpdateJson]

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

    RabbitCommon::CTools::Instance()->Clean();
    return nRet;
}
