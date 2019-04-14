#include <QApplication>
#ifdef HAVE_ABOUT
    #include "DlgAbout/DlgAbout.h"
#endif
#ifdef HAVE_UPDATE
    #include "FrmUpdater/FrmUpdater.h"
#endif
#include <QTranslator>
#include <QDir>
#include "RabbitCommonTools.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    
    QString szPre;    
#if defined(Q_OS_ANDROID) || _DEBUG
    szPre = ":/Translations";
#else
    szPre = qApp->applicationDirPath() + QDir::separator() + ".." + QDir::separator() + "translations";
#endif
    
    QTranslator tApp, tTasks;
    tApp.load(szPre + "/RabbitCommonApp_" + QLocale::system().name() + ".qm");
    a.installTranslator(&tApp);

    CRabbitCommonTools::Instance()->InitTranslator();
#ifdef HAVE_UPDATE
    CFrmUpdater update;
    update.setAttribute(Qt::WA_QuitOnClose, true);
    update.SetTitle(qApp->applicationDisplayName(), QPixmap(":/icon/RabbitCommon/App"));
    update.show();
#endif

#ifdef HAVE_ABOUT
    CDlgAbout dlg;
    dlg.setAttribute(Qt::WA_QuitOnClose, true);
    dlg.show();
#endif
    return a.exec();
}
