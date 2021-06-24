/** @copyright Copyright (c) Kang Lin studio, All Rights Reserved
 *  @author Kang Lin(kl222@126.com)
 */

#include "MainWindow.h"
#include "ui_MainWindow.h"

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
#include "RabbitCommonTools.h"
#include "RabbitCommonStyle.h"
#include "RabbitCommonService.h"

#include <QDir>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
    qApp->quit();
}

void MainWindow::on_actionAbout_triggered()
{
#ifdef HAVE_ABOUT
    CDlgAbout* dlg = new CDlgAbout();
    dlg->setAttribute(Qt::WA_QuitOnClose, true);
#if defined (Q_OS_ANDROID)
    dlg->showMaximized();
#else
    dlg->show();
#endif
#endif
    QUIWidget::setFormInCenter(dlg);
}

void MainWindow::on_actionUpdate_triggered()
{
#ifdef HAVE_UPDATE
    CFrmUpdater *update = new CFrmUpdater();
    update->setAttribute(Qt::WA_QuitOnClose, true);
    update->SetTitle(QImage(":/icon/RabbitCommon/App"));
    if(!update->GenerateUpdateXml())
        return;
#if defined (Q_OS_ANDROID)
    update->showMaximized();
#else
    update->show();
#endif
#endif
    QUIWidget::setFormInCenter(update);
}

void MainWindow::on_pushButton_clicked()
{
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
}

void MainWindow::on_actionStype_triggered()
{
    RabbitCommon::CStyle::Instance()->slotStyle();
}

void MainWindow::on_pbInstallService_clicked()
{
    RabbitCommon::CServiceManage::Instance()->Install();
}

void MainWindow::on_pbUninstallService_clicked()
{
    RabbitCommon::CServiceManage::Instance()->Uninstall();
}

void MainWindow::on_pbStartService_clicked()
{
    RabbitCommon::CServiceManage::Instance()->Start();
}

void MainWindow::on_pbStop_clicked()
{
    RabbitCommon::CServiceManage::Instance()->Stop();
}

void MainWindow::on_pbPauseService_clicked()
{
    RabbitCommon::CServiceManage::Instance()->Pause();
}

void MainWindow::on_pbContinueService_clicked()
{
    RabbitCommon::CServiceManage::Instance()->Continue();
}