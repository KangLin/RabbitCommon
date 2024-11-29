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
#include "DockFolderBrowser.h"
#include "FileBrowser.h"
#endif
#include "RabbitCommonDir.h"

#include <QDir>
#include <QDesktopServices>
#include <QDockWidget>
#include <QFileDialog>
#include <QLoggingCategory>
#include <QThread>
//#include <source_location>

/*! \see Use Q_FUNC_INFO
 *  - https://en.cppreference.com/w/cpp/utility/source_location
 *  - https://cplusplus.com/forum/general/179020/
 *  - https://www.gnu.org/software/c-intro-and-ref/manual/html_node/Predefined-Macros.html
 *  - https://gcc.gnu.org/onlinedocs/cpp/Standard-Predefined-Macros.html
 */
#if defined(Q_OS_WIN)
// See: https://learn.microsoft.com/zh-cn/cpp/preprocessor/predefined-macros?view=msvc-170
#define __FUN__ Q_FUNC_INFO
#else
#define __FUN__ __PRETTY_FUNCTION__
#endif

Q_LOGGING_CATEGORY(log, "RabbitCommon.MainWindow")

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_pDownload(nullptr)
{
    ui->setupUi(this);

    //https://en.cppreference.com/w/cpp/utility/source_location
    qDebug(log) << "\n"
                << "Q_FUNC_INFO         " << Q_FUNC_INFO << "\n"
                << "__func__            " << __func__ << "\n"
                << "Q_FUNC_INFO        " << Q_FUNC_INFO << "\n"
#if defined(Q_OS_WIN)
                << "__FUNCDNAME__       " << __FUNCDNAME__ << "\n"
                << "__FUNCSIG__         " << __FUNCSIG__ << "\n"
#else
                << "__PRETTY_FUNCTION__ " << __PRETTY_FUNCTION__ << "\n"
#endif
        ;
        // std::source_location location = std::source_location::current();
        //         << "std::source_location" << location.function_name()
        // ;

#ifdef HAVE_RABBITCOMMON_GUI
    RabbitCommon::CTools::AddStyleMenu(ui->menuTools);
    ui->menuTools->addMenu(RabbitCommon::CTools::GetLogMenu(this));
    ui->tbLog->setMenu(RabbitCommon::CTools::GetLogMenu(this));


    // [Use CDockFolderBrowser]
    CDockFolderBrowser* pDock = new CDockFolderBrowser(tr("Folder browser"), this);
    pDock->setRootPath(RabbitCommon::CDir::Instance()->GetDirIcons(true));
    qDebug() << pDock->rootPath();
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
    RC_SHOW_WINDOW(dlg);
#endif
}

void MainWindow::on_actionUpdate_triggered()
{
#ifdef HAVE_UPDATE
    // [Use CFrmUpdater]
    CFrmUpdater* m_pfrmUpdater = new CFrmUpdater();
    QIcon icon = windowIcon();
    if(!icon.isNull()) {
        auto sizeList = icon.availableSizes();
        if(!sizeList.isEmpty()) {
            QPixmap p = icon.pixmap(*sizeList.begin());
            m_pfrmUpdater->SetTitle(p.toImage());
        }
    }
    m_pfrmUpdater->SetInstallAutoStartup();
    RC_SHOW_WINDOW(m_pfrmUpdater);
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
    m_pDownload = new RabbitCommon::CDownload();
    bool check = connect(m_pDownload, SIGNAL(sigFinished(const QString)),
                         this, SLOT(slotDownloadFile(const QString)));
    Q_ASSERT(check);
    check = connect(m_pDownload, SIGNAL(sigError(int, const QString)),
                    this, SLOT(slotDownloadError(int, const QString)));
    Q_ASSERT(check);
    m_pDownload->Start(urls);
}

void MainWindow::slotDownloadFile(const QString szFile)
{
    qDebug(log) << "Download file:" << szFile;
    QDesktopServices::openUrl(QUrl::fromLocalFile(szFile));
}

void MainWindow::slotDownloadError(int nErr, const QString szErr)
{
    qDebug(log) << "Download file error:" << nErr << szErr;
}

void MainWindow::on_actionFolder_browser_triggered()
{
    CDlgFileBrowser fb;
    RC_SHOW_WINDOW(&fb);
}

class CCoreThread : public QThread
{
public:
    CCoreThread(QObject *parent = nullptr) : QThread(parent)
    {
        connect(this, SIGNAL(destroyed(QObject*)),
                this, SLOT(deleteLater()));
    }
    virtual ~CCoreThread()
    {
        qDebug(log) << "CCoreThread::~CCoreThread()";
    }

    // QThread interface
protected:
    virtual void run() override;
};

void CCoreThread::run()
{
#if defined(Q_OS_WIN)
    strcpy(0,0);
#else
    free((void*)11);
#endif
}

void MainWindow::on_pbThreadCore_clicked()
{
    CCoreThread* pThread = new CCoreThread(this);
    pThread->start();
}

void MainWindow::on_pbGenerateCoreFile_clicked()
{
#if defined(Q_OS_WIN)
    strcpy(0,0);
#else
    free((void*)11);
#endif
}
