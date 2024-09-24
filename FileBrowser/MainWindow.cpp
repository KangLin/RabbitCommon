#include <QLoggingCategory>
#include <QSettings>
#include "RabbitCommonTools.h"
#include "MainWindow.h"
#include "./ui_MainWindow.h"
#include "FileBrowser.h"
#include "RabbitCommonDir.h"
#include "FrmUpdater.h"
#include "DlgAbout.h"

static Q_LOGGING_CATEGORY(log, "FileBrowser.MainWindow")
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    bool check = false;
    ui->setupUi(this);

#ifdef HAVE_RABBITCOMMON_GUI
    RabbitCommon::CTools::InsertStyleMenu(ui->menuTools, ui->actionStatus_S);
    ui->menuTools->insertMenu(ui->actionStatus_S, RabbitCommon::CTools::GetLogMenu(this));
    ui->menuTools->insertSeparator(ui->actionStatus_S);
#endif

    CFileBrowser* pFB = new CFileBrowser(this);
    setCentralWidget(pFB);
    setWindowIcon(pFB->windowIcon());
    setWindowTitle(pFB->windowTitle());
    check = connect(pFB, SIGNAL(destroyed(QObject*)), this, SLOT(close()));
    Q_ASSERT(check);
    check = connect(pFB, &CFileBrowser::sigChanged, this,
                    [&](const QString& szDir, bool bDir){
        qDebug(log) << "CFileBrowser::sigChanged" << szDir << bDir;
    });
    Q_ASSERT(check);

    QSettings set(RabbitCommon::CDir::Instance()->GetFileUserConfigure(),
                  QSettings::IniFormat);
    QByteArray geometry
        = set.value("MainWindow/Status/Geometry").toByteArray();
    if(!geometry.isEmpty())
        restoreGeometry(geometry);
    QByteArray state = set.value("MainWindow/Status/State").toByteArray();
    if(!state.isEmpty())
        restoreState(state);

    bool bStatusBar = set.value("MainWindow/Status/Bar",
                                !statusBar()->isHidden()).toBool();
    ui->actionStatus_S->setChecked(bStatusBar);
    statusBar()->setVisible(bStatusBar);
}

MainWindow::~MainWindow()
{
    QSettings set(RabbitCommon::CDir::Instance()->GetFileUserConfigure(),
                  QSettings::IniFormat);
    set.setValue("MainWindow/Status/Geometry", saveGeometry());
    set.setValue("MainWindow/Status/State", saveState());
    set.setValue("MainWindow/Status/Bar", !statusBar()->isHidden());

    delete ui;
    qDebug(log) << "MainWindow::~MainWindow()";
}

void MainWindow::on_actionStatus_S_triggered(bool checked)
{
    statusBar()->setVisible(checked);
}

void MainWindow::on_actionAbout_A_triggered()
{
#ifdef HAVE_ABOUT
    CDlgAbout* dlg = new CDlgAbout();
    dlg->setAttribute(Qt::WA_QuitOnClose, true);
#if defined (Q_OS_ANDROID)
    dlg->showMaximized();
#endif
    dlg->exec();
#endif

#ifdef BUILD_QUIWidget
    QUIWidget::setFormInCenter(dlg);
#endif
}

void MainWindow::on_actionUpdate_U_triggered()
{
#ifdef HAVE_UPDATE
    // [Use CFrmUpdater]
    CFrmUpdater* m_pfrmUpdater = new CFrmUpdater();
    QIcon icon = windowIcon();
    if(icon.isNull()) return;
    auto sizeList = icon.availableSizes();
    if(sizeList.isEmpty()) return;
    QPixmap p = icon.pixmap(*sizeList.begin());
    m_pfrmUpdater->SetTitle(p.toImage());
    m_pfrmUpdater->SetInstallAutoStartup();
#if defined (Q_OS_ANDROID)
    m_pfrmUpdater->showMaximized();
#else
    m_pfrmUpdater->show();
#endif
    // [Use CFrmUpdater]
#endif
#ifdef BUILD_QUIWidget
    QUIWidget::setFormInCenter(m_pfrmUpdater);
#endif
}
