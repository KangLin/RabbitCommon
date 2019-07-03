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
#include <QTranslator>
#include <QDir>
#include "RabbitCommonTools.h"
#include "RabbitCommonDir.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    
    QTranslator tApp, tTasks;
    tApp.load(RabbitCommon::CDir::Instance()->GetDirTranslations()
              + "/RabbitCommonApp_" + QLocale::system().name() + ".qm");
    a.installTranslator(&tApp);

    RabbitCommon::CTools::Instance()->Init();
#ifdef HAVE_UPDATE
    CFrmUpdater update;
    update.setAttribute(Qt::WA_QuitOnClose, true);
    update.SetTitle(QPixmap(":/icon/RabbitCommon/App"));
    update.show();
#endif

#ifdef HAVE_ABOUT
    CDlgAbout dlg;
    dlg.setAttribute(Qt::WA_QuitOnClose, true);
    dlg.show();
#endif

#ifdef HAVE_ADMINAUTHORISER
#ifdef defined(Q_OS_LINUX) && !defined(Q_OS_ANDROID)
    QString szCmd = "mkdir";
    QStringList paras;
    paras << "-p" << "/opt/RabbitCommonAdminAuthoriseTest";
    qDebug() << "RabbitCommon::CTools::execute(szCmd, paras):"
             << RabbitCommon::CTools::execute(szCmd, paras);
#elif defined(Q_OS_WINDOWS)
    RabbitCommon::CTools::execute("regedit", QStringList());
#endif
#endif
    return a.exec();
}
