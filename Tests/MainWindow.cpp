/** @copyright Copyright (c) Kang Lin studio, All Rights Reserved
 *  @author Kang Lin(kl222@126.com)
 */

#include "MainWindow.h"
#include "ui_MainWindow.h"

#ifdef HAVE_ABOUT
    #include "DlgAbout.h"
#endif
#ifdef HAVE_UPDATE
    #include "FrmUpdater.h"
#endif
#ifdef BUILD_QUIWidget
    #include "QUIWidget/QUIWidget.h"
#endif
#include "RabbitCommonTools.h"
#include "RabbitCommonEncrypt.h"
#ifdef HAVE_RABBITCOMMON_GUI
#include "FrmStyle.h"
#include "DockFolderBrowser.h"
#endif
#include "RabbitCommonDir.h"

#include <QDir>
#include <QDebug>
#include <QDesktopServices>
#include <QDockWidget>
#include <QFileDialog>

Q_LOGGING_CATEGORY(windowLog, "RabbitCommon.MainWindow")

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_pDownload(nullptr)
{
    ui->setupUi(this);
    
    ui->menuTools->addMenu(RabbitCommon::CTools::GetLogMenu(ui->menuTools));

#ifdef HAVE_RABBITCOMMON_GUI
    // [Use CDockFolderBrowser]
    CDockFolderBrowser* pDock = new CDockFolderBrowser(tr("Folder browser"), this);
    pDock->setRootPath(RabbitCommon::CDir::Instance()->GetDirIcons(true));
    addDockWidget(Qt::LeftDockWidgetArea, pDock);
    // Add the action of dock to menu
    ui->menuTools->addAction(pDock->toggleViewAction());
    // [Use CDockFolderBrowser]
#endif
}

MainWindow::~MainWindow()
{
    qApp->quit();
    delete ui;
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

#ifdef BUILD_QUIWidget
    QUIWidget::setFormInCenter(dlg);
#endif
}

void MainWindow::on_actionUpdate_triggered()
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

void MainWindow::on_pushButton_clicked()
{
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
}

void MainWindow::on_actionStype_triggered()
{
#ifdef HAVE_RABBITCOMMON_GUI
    CFrmStyle* s = new CFrmStyle();
    s->show();
#endif
}

void MainWindow::on_pbEncrypt_clicked()
{
    RabbitCommon::CEncrypt e;
    e.SetPassword(ui->lePassword->text().toStdString().c_str());
    QByteArray out;
    int nRet = e.Encode(ui->teInput->toPlainText(), out);
    if(!nRet)
    {
        QString szOut;
        nRet = e.Dencode(out, szOut);
        if(nRet)
        {
            szOut = tr("Encrypt:") + QString(out) + "\n" + tr("Source:") + szOut;
            ui->teOutput->setPlainText(szOut);
        }
    }
}

void MainWindow::on_pbGenerateCoreFile_clicked()
{
    strcpy(0,0);
}

void MainWindow::on_pbAddFile_clicked()
{
    ui->cmbDownloadFiles->addItem(ui->cmbDownloadFiles->currentText());
}

void MainWindow::on_pbDownload_clicked()
{
    if(m_pDownload)
        m_pDownload->deleteLater();
    QVector<QUrl> urls;
    for(int i = 0; i < ui->cmbDownloadFiles->count(); i++)
    {
        urls.push_back(QUrl(ui->cmbDownloadFiles->itemText(i)));
    }
    m_pDownload = new RabbitCommon::CDownloadFile(urls);
    bool check = connect(m_pDownload, SIGNAL(sigFinished(const QString)),
            this, SLOT(slotDownloadFile(const QString)));
    Q_ASSERT(check);
    check = connect(m_pDownload, SIGNAL(sigError(int, const QString)),
                    this, SLOT(slotDownloadError(int, const QString)));
    Q_ASSERT(check);
}

void MainWindow::slotDownloadFile(const QString szFile)
{
    qDebug(windowLog) << "Download file:" << szFile;
    QDesktopServices::openUrl(QUrl::fromLocalFile(szFile));
}

void MainWindow::slotDownloadError(int nErr, const QString szErr)
{
    qDebug(windowLog) << "Download file error:" << nErr << szErr;
}

void MainWindow::on_pbOpenFolder_clicked()
{
    QIcon icon = QIcon::fromTheme("filter");
    if(icon.isNull())
        qDebug() << "Load icon fail";

    QFileDialog fileDialog(this);
    fileDialog.setAcceptMode(QFileDialog::AcceptOpen);
    fileDialog.setWindowTitle(tr("Open SVG File"));

    fileDialog.setDirectory(RabbitCommon::CDir::Instance()->GetDirApplicationInstallRoot());
    fileDialog.exec();
}
