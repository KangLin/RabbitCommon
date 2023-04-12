#include "FolderBrowser.h"
#include "ui_FolderBrowser.h"
#include "TitleBar.h"
#include "RabbitCommonDir.h"

#include <QFileSystemModel>
#include <QMenu>
#include <QDir>
#include <QSettings>

CFolderBrowser::CFolderBrowser(const QString &title,
                             QWidget *parent,
                             Qt::WindowFlags flags)
    : QDockWidget(title, parent, flags),
      ui(new Ui::CFolderBrowser),
      m_pModel(new QFileSystemModel(this)),
      m_pHideFile(nullptr),
      m_pDetails(nullptr),
      m_bDetails(false)
{
    QSettings set(RabbitCommon::CDir::Instance()->GetFileUserConfigure(),
                  QSettings::IniFormat);
    
    ui->setupUi(this);

    ui->treeView->setModel(m_pModel);

    connect(ui->treeView, &QTreeView::doubleClicked,
            this, [&](const QModelIndex &index) {
        if (m_pModel) {
            emit sigDoubleClicked(m_pModel->filePath(index),
                                  m_pModel->isDir(index));
        }
    });

    setRootPath("");

    // [Use CTitleBar]
    RabbitCommon::CTitleBar* pDockTitleBar = new RabbitCommon::CTitleBar(this);
    if(pDockTitleBar) {
        // Set title widget
        setTitleBarWidget(pDockTitleBar);

        // Create filter pushbutton in title bar
        QMenu* pMenu = new QMenu(tr("Filter"), pDockTitleBar);
        QPushButton* pFilter = pDockTitleBar->CreateSmallPushButton(
                    QIcon::fromTheme("filter"), pDockTitleBar);
        pFilter->setToolTip(tr("Filter"));
        pFilter->setMenu(pMenu);
        QList<QWidget*> lstWidget;
        lstWidget << pFilter;
        pDockTitleBar->AddWidgets(lstWidget);
        
        QString szTitle;

        // Add set filter action in menu
        uint filter = m_pModel->filter();
        filter = set.value("FolderBrowser/Filters", filter).toUInt();
        QDir::Filters f = QDir::Filters(filter);
        m_pModel->setFilter(f);
        if(QDir::Hidden & f)
        {
            szTitle = tr("Disable hide files");
        } else {
            szTitle = tr("Show hide files");
        }
        m_pHideFile = pMenu->addAction(szTitle, this, [&](){
            if(!(m_pHideFile && m_pModel)) return;
            QDir::Filters f = m_pModel->filter();
            if(QDir::Hidden & f)
            {
                m_pHideFile->setText(tr("Show hide files"));
                m_pModel->setFilter(f & (~QDir::Hidden));
            } else {
                m_pHideFile->setText(tr("Disable hide files"));
                m_pModel->setFilter(f | QDir::Hidden);
            }
        });
        
        // Add show details action in menu
        m_bDetails = set.value("FolderBrowser/Details", false).toBool();
        if(m_bDetails) {
            szTitle = tr("Show brief");
            ui->treeView->header()->showSection(1);
            ui->treeView->header()->showSection(2);
            ui->treeView->header()->showSection(3);
        } else {
            szTitle = tr("Show details");
            ui->treeView->header()->hideSection(1);
            ui->treeView->header()->hideSection(2);
            ui->treeView->header()->hideSection(3);
        }
        m_pDetails = pMenu->addAction(szTitle, this, [&](){
            if(!(m_pDetails && m_pModel)) return;
            m_bDetails = !m_bDetails;
            QString szTitle;
            if(m_bDetails) {
                szTitle = tr("Show brief");
                ui->treeView->header()->showSection(1);
                ui->treeView->header()->showSection(2);
                ui->treeView->header()->showSection(3);
            } else {
                szTitle = tr("Show details");
                ui->treeView->header()->hideSection(1);
                ui->treeView->header()->hideSection(2);
                ui->treeView->header()->hideSection(3);
            }
            m_pDetails->setText(szTitle);
        });
    }
    // [Use CTitleBar]
}

CFolderBrowser::CFolderBrowser(QWidget *parent, Qt::WindowFlags flags) :
    CFolderBrowser(tr("Browse folder"), parent, flags)
{}

CFolderBrowser::~CFolderBrowser()
{
    QSettings set(RabbitCommon::CDir::Instance()->GetFileUserConfigure(),
                  QSettings::IniFormat);
    uint flitersFlag = m_pModel->filter();
    set.setValue("FolderBrowser/Filters", flitersFlag);
    set.setValue("FolderBrowser/Details", m_bDetails);
    delete ui;
}

void CFolderBrowser::setRootPath(const QString dir)
{
    m_pModel->setRootPath(dir);
    ui->treeView->setRootIndex(m_pModel->index(dir));
}

QString CFolderBrowser::rootPath() const
{
    return m_pModel->rootPath();
}
