#include <QApplication>
#ifdef HAVE_ABOUT
    #include "DlgAbout/DlgAbout.h"
#endif
#ifdef HAVE_UPDATE
    #include "FrmUpdater/FrmUpdater.h"
#endif
#ifdef HAVE_ADMINAUTHORISER
    #include "AdminAuthoriser/adminauthoriser.h"
#endif
#ifdef BUILD_QUIWidget
    #include "QUIWidget/QUIWidget.h"
#endif
#include <QTranslator>
#include <QDir>
#include <QCommandLineOption>
#include <QCommandLineParser>

#include "RabbitCommonTools.h"
#include "RabbitCommonDir.h"
#include "RabbitCommonStyle.h"

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
    style.slotStyle();
    
#ifdef BUILD_QUIWidget
    QUIWidget quiwidget;
    quiwidget.setPixmap(QUIWidget::Lab_Ico, ":/icon/RabbitCommon/App");
    quiwidget.setTitle(QObject::tr("Rabbit Common - QUIWidget"));
    quiwidget.setAlignment(Qt::AlignCenter);
    quiwidget.setVisible(QUIWidget::BtnMenu, true);
    quiwidget.exec();
#endif
    
#ifdef HAVE_UPDATE
    CFrmUpdater update;
    update.setAttribute(Qt::WA_QuitOnClose, true);
    update.SetTitle(QImage(":/icon/RabbitCommon/App"));
    if(!update.GenerateUpdateXml())
        return 0;
#if defined (Q_OS_ANDROID)
    update.showMaximized();
#else
    update.show();
#endif
#endif
    
#ifdef HAVE_ABOUT
    CDlgAbout dlg;
    dlg.setAttribute(Qt::WA_QuitOnClose, true);
#if defined (Q_OS_ANDROID)
    dlg.showMaximized();
#else
    dlg.show();
#endif
#endif    

#ifdef HAVE_ADMINAUTHORISER
#if defined(Q_OS_LINUX) && !defined(Q_OS_ANDROID)
    QString szCmd = "mkdir";
    QStringList paras;
    paras << "-p" << "/opt/RabbitCommonAdminAuthoriseTest";
    qDebug() << "RabbitCommon::CTools::executeByRoot(szCmd, paras):"
             << RabbitCommon::CTools::executeByRoot(szCmd, paras);
    RabbitCommon::CTools::GenerateDesktopFile(QDir::currentPath());
#elif defined(Q_OS_WINDOWS)
    RabbitCommon::CTools::executeByRoot("regedit", QStringList());
#endif
#endif
    
    return a.exec();
}
