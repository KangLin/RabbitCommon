// Author: Kang Lin <kl222@126.com>

#include <QSettings>
#include <QPushButton>
#include <QToolButton>
#include <QMenu>
#include <QWidgetAction>
#include <QDesktopServices>
#include <QSplitter>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSpacerItem>
#include <QToolBar>
#include <QLoggingCategory>
#include <QScreen>
#include <QApplication>
#include <QHeaderView>
#include <QMimeDatabase>
#include <QCheckBox>
#include "FileBrowser.h"
#include "RabbitCommonDir.h"

static Q_LOGGING_CATEGORY(log, "RabbitCommon.Browser.File")
CFileBrowser::CFileBrowser(QWidget *parent)
    : QDialog{parent}
    , m_pModel(new QFileSystemModel(this))
    , m_pTree(nullptr)
    , m_pList(nullptr)
    , m_pTable(nullptr)
    , m_pTextEdit(nullptr)
    , m_bAssociated(true)
{
    bool check = false;
    //setAttribute(Qt::WA_DeleteOnClose);
    setWindowTitle(tr("File browser"));

    QSettings set(RabbitCommon::CDir::Instance()->GetFileUserConfigure(),
                  QSettings::IniFormat);

    int nWidth = 600;
    int nHeight = 450;
    QRect rect;
    if(parent) {
        rect = parent->frameGeometry();
    } else {
        QScreen* pScreen = QApplication::primaryScreen();
        if(pScreen)
            rect = pScreen->geometry();
    }
    rect.setLeft((rect.width() - nWidth) / 2);
    rect.setTop((rect.height() - nHeight) / 2);
    rect.setWidth(nWidth);
    rect.setHeight(nHeight);
    setGeometry(rect);

    QVBoxLayout* pLayout = new QVBoxLayout();
    m_pSpliter = new QSplitter(this);
    m_pTree = new QTreeView(m_pSpliter);
    m_pTable = new QTableView(m_pSpliter);
    //m_pList = new QListView(pSpliter);
    m_pTextEdit = new QTextEdit(m_pSpliter);
    if(!pLayout) return;

    do {
        if(!m_pModel) return;
  //      m_pModel->setReadOnly(false);
        /*m_pModel->setFilter(QDir::AllDirs | QDir::Drives
                                | QDir::NoDotAndDotDot);*/
        setLayout(pLayout);
        if(!m_pSpliter) break;

        QToolBar* pToolBar = new QToolBar();
        pToolBar->addAction(QIcon::fromTheme("window-close"), tr("Close"),
                            this, &CFileBrowser::reject);
        pToolBar->addAction(QIcon::fromTheme("folder-new"), tr("New folder"),
                            this, [&](){
                                QModelIndex index = m_pTree->currentIndex();
                                index = m_pModel->mkdir(index, tr("NewFolder"));
                                if(!index.isValid())
                                    qCritical(log) << "New folder fail";
                            });
        pToolBar->addAction(QIcon::fromTheme("edit-delete"), tr("Delete folder"),
                            this, [&](){
                                QString szDir;
                                QModelIndex index = m_pTree->currentIndex();
                                szDir = m_pModel->data(index).toString();
                                bool bRet = m_pModel->rmdir(index);
                                if(bRet)
                                    qDebug(log) << "Delete folder:" << szDir;
                                else
                                    qCritical(log) << "Delete folder fail" << szDir;
                            });
        QToolButton* pButtonOption = new QToolButton(pToolBar);
        pButtonOption->setIcon(QIcon::fromTheme("emblem-system"));
        QMenu* pMenuOption = new QMenu(tr("Option"), pButtonOption);
        pButtonOption->setMenu(pMenuOption);
        pToolBar->addWidget(pButtonOption);
        QCheckBox* pHiddenFile = new QCheckBox(tr("Hidden file"));
        check = set.value("FileBrowser/HiddenFile", check).toBool();
        pHiddenFile->setCheckable(true);
        pHiddenFile->setChecked(check);
        if(check)
            m_pModel->setFilter(m_pModel->filter() | QDir::Hidden);
        else
            m_pModel->setFilter(m_pModel->filter() & (~QDir::Hidden));
        check = connect(pHiddenFile, &QCheckBox::clicked, this,
                [&](bool checked){
                    if(checked)
                        m_pModel->setFilter(m_pModel->filter() | QDir::Hidden);
                    else
                        m_pModel->setFilter(m_pModel->filter() & (~QDir::Hidden));
                    QSettings set(RabbitCommon::CDir::Instance()->GetFileUserConfigure(),
                                  QSettings::IniFormat);
                    set.setValue("FileBrowser/HiddenFile", checked);
        });
        Q_ASSERT(check);
        QWidgetAction *pAction = new QWidgetAction(this);
        pAction->setDefaultWidget(pHiddenFile);
        pMenuOption->addAction(qobject_cast<QAction*>(pAction));
        QCheckBox* pAssociated = new QCheckBox(tr("Open with the System Associated Program"));
        m_bAssociated = set.value("FileBrowser/SystemAssociatedProgram", m_bAssociated).toBool();
        pAssociated->setCheckable(true);
        pAssociated->setChecked(m_bAssociated);
        check = connect(pAssociated, &QCheckBox::clicked, this,
                [&](bool checked){
                            m_bAssociated = checked;
                            QSettings set(RabbitCommon::CDir::Instance()->GetFileUserConfigure(),
                                          QSettings::IniFormat);
                            set.setValue("FileBrowser/SystemAssociatedProgram", checked);
                        });
        Q_ASSERT(check);
        pAction = new QWidgetAction(this);
        pAction->setDefaultWidget(pAssociated);
        pMenuOption->addAction(qobject_cast<QAction*>(pAction));
        QCheckBox* pOrientation = new QCheckBox(tr("Horizontal"));
        pOrientation->setCheckable(true);
#if defined(Q_OS_ANDROID)
        check = false;
#else
        check = true;
#endif
        check = set.value("FileBrowser/Spliter/Horizontal", check).toBool();
        pOrientation->setChecked(check);
        if(check)
            m_pSpliter->setOrientation(Qt::Horizontal);
        else
            m_pSpliter->setOrientation(Qt::Vertical);
        check = connect(pOrientation, &QCheckBox::clicked, this,
                        [&](bool checked){
                            if(checked)
                                m_pSpliter->setOrientation(Qt::Horizontal);
                            else
                                m_pSpliter->setOrientation(Qt::Vertical);
                            QSettings set(RabbitCommon::CDir::Instance()->GetFileUserConfigure(),
                                          QSettings::IniFormat);
                            set.setValue("FileBrowser/Spliter/Horizontal", checked);
        });
        pAction = new QWidgetAction(this);
        pAction->setDefaultWidget(pOrientation);
        pMenuOption->addAction(qobject_cast<QAction*>(pAction));

        pLayout->addWidget(pToolBar);
        pLayout->addWidget(m_pSpliter);
        /*
        QPushButton* pClose = new QPushButton(QIcon::fromTheme("window-close"), tr("Close"), this);
        QHBoxLayout* pCloseLayout = new QHBoxLayout(this);
        pCloseLayout->addStretch();
        check = connect(pClose, &QPushButton::clicked, this, &CFileBrowser::reject);
        pCloseLayout->addWidget(pClose);
        pCloseLayout->addStretch();
        Q_ASSERT(check);
        pLayout->addLayout(pCloseLayout);//*/

        if(m_pTree) {
            m_pTree->setModel(m_pModel);
            m_pTree->setHeaderHidden(true);
            // 注意：必须在 setModel 之后才会生效
            m_pTree->header()->hideSection(1);
            m_pTree->header()->hideSection(2);
            m_pTree->header()->hideSection(3);
            m_pSpliter->addWidget(m_pTree);

            if(m_pTable) {
                check = connect(m_pTree, &QTreeView::clicked,
                                m_pTable, &QTableView::setRootIndex);
                Q_ASSERT(check);
            }
            if(m_pList) {
                check = connect(m_pTree, &QTreeView::clicked,
                                m_pList, &QListView::setRootIndex);
                Q_ASSERT(check);
            }
            check = connect(
                m_pTree, &QTreeView::clicked,
                this, [&](const QModelIndex &index) {
                    if (m_pModel) {
                        if(m_pModel->isDir(index)) {
                            m_pTable->show();
                            m_pTextEdit->hide();
                            return;
                        }
                        m_pTable->hide();
                    }
                });
            Q_ASSERT(check);
            check = connect(
                m_pTree, &QTreeView::doubleClicked,
                this, [&](const QModelIndex &index) {
                    if (m_pModel) {
                        if(m_pModel->isDir(index)) {
                            return;
                        }
                        m_pTable->hide();
                        QString szFile = m_pModel->filePath(index);
                        ShowFile(szFile);
                    }
                });
            Q_ASSERT(check);
        }

        if(m_pList) {
            m_pSpliter->addWidget(m_pList);
            m_pList->setModel(m_pModel);
        }

        if(m_pTable) {
            m_pTable->hide();
            m_pSpliter->addWidget(m_pTable);
            m_pTable->setModel(m_pModel);
            m_pTable->setShowGrid(false);
            m_pTable->verticalHeader()->hide();
            check = connect(
                m_pTable, &QTableView::clicked,
                this, [&](const QModelIndex &index) {
                    if (m_pModel) {
                        m_pTree->setCurrentIndex(index);
                        if(m_pModel->isDir(index)) {
                            QString dir = m_pModel->filePath(index);
                            m_pModel->setRootPath(dir);
                            m_pTable->setRootIndex(m_pModel->index(dir));
                            if(!m_pTextEdit->isHidden())
                                m_pTextEdit->hide();
                        }
                    }
                });
            Q_ASSERT(check);
            check = connect(m_pTable, &QTableView::doubleClicked,
                            this, [&](const QModelIndex &index){
                QString szFile = m_pModel->filePath(index);
                ShowFile(szFile);
            });
            Q_ASSERT(check);
        }

        if(m_pTextEdit) {
            m_pTextEdit->hide();
            m_pSpliter->addWidget(m_pTextEdit);
        }
        setRootPath("");
        return;
    } while(0);
    if(m_pModel)
        delete m_pModel;
    if(m_pModel)
        delete m_pModel;
    if(pLayout)
        delete pLayout;
    if(m_pSpliter)
        delete m_pSpliter;
    if(m_pTree)
        delete m_pTree;
    if(m_pList)
        delete m_pList;
    if(m_pTable)
        delete m_pTable;
}

void CFileBrowser::setRootPath(const QString dir)
{
    m_pModel->setRootPath(dir);
    m_pTree->setRootIndex(m_pModel->index(dir));
}

QString CFileBrowser::rootPath() const
{
    return m_pModel->rootPath();
}

QString CFileBrowser::readFile(const QString &filePath)
{
    // Don't issue errors for an empty path, as the initial binding
    // will result in an empty path, and that's OK.
    if (filePath.isEmpty())
        return {};

    QFile file(filePath);

    if (file.size() >= 2000000)
        return tr("File size is too big.\nYou can read files up to %1 MB.").arg(2);

    static const QMimeDatabase db;
    const QMimeType mime = db.mimeTypeForFile(QFileInfo(file));

    // Check if the mimetype is supported and return the content.
    const auto mimeTypesForFile = mime.parentMimeTypes();
    for (const auto &m : mimeTypesForFile) {
        if (m.contains("text", Qt::CaseInsensitive)
            || mime.comment().contains("text", Qt::CaseInsensitive)) {
            if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
                return tr("Error opening the File!");

            QTextStream stream(&file);
            return stream.readAll();
        }
    }
    return tr("Filetype not supported!");
}

int CFileBrowser::ShowFile(const QString &szFile)
{
    if(m_bAssociated) {
        QUrl url = QUrl::fromLocalFile(szFile);
        if(QDesktopServices::openUrl(url))
            return 0;
    }

    m_pTextEdit->setText(readFile(szFile));
    if(m_pTextEdit->isHidden())
        m_pTextEdit->show();
    return 0;
}

int CFileBrowser::exec()
{
#if defined(Q_OS_ANDROID)
    showMaximized();
#endif
    return QDialog::exec();
}
