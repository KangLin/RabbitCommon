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
#include "RabbitCommonGlobalDir.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    
    QTranslator tApp, tTasks;
    tApp.load(CRabbitCommonGlobalDir::Instance()->GetDirTranslations()
              + "/RabbitCommonApp_" + QLocale::system().name() + ".qm");
    a.installTranslator(&tApp);

    CRabbitCommonTools::Instance()->Init();
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
    return a.exec();
}
