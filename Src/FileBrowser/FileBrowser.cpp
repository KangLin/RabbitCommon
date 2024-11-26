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
#include <QLineEdit>
#include <QDir>
#include "RabbitCommonDir.h"
#include "RabbitCommonTools.h"
#include "UndoCommand.h"
#include "FileBroserTreeView.h"
#include "FileBrowser.h"

static Q_LOGGING_CATEGORY(log, "RabbitCommon.Browser.File")
CFileBrowser::CFileBrowser(QWidget *parent)
    : QWidget{parent}
    , m_pSpliter(nullptr)
    , m_pUndoStack(nullptr)
    , m_pModel(nullptr)
    , m_pTree(nullptr)
    , m_pList(nullptr)
    , m_pTable(nullptr)
    , m_pTextEdit(nullptr)
    , m_pHiddenFile(nullptr)
    , m_pAssociated(nullptr)
    , m_pOrientation(nullptr)
{
    bool check = false;
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowTitle(tr("File browser"));
    setWindowIcon(QIcon::fromTheme("browser"));

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

    QVBoxLayout* pLayout = new QVBoxLayout(this);

    do {
        m_pModel = new QFileSystemModel(this);
        if(!m_pModel) break;;
  //      m_pModel->setReadOnly(false);
        /*m_pModel->setFilter(QDir::AllDirs | QDir::Drives
                                | QDir::NoDotAndDotDot);*/
        if(!pLayout) break;
        setLayout(pLayout);
        m_pSpliter = new QSplitter(this);
        if(!m_pSpliter) break;

        m_pTree = new CFileBroserTreeView(m_pSpliter);
        m_pTable = new QTableView(m_pSpliter);
        //m_pList = new QListView(pSpliter);
        m_pTextEdit = new QTextEdit(m_pSpliter);

        QString szTitle;
        QToolBar* pToolBar = new QToolBar(this);
        szTitle = tr("Close");
        QAction* pAction = pToolBar->addAction(QIcon::fromTheme("window-close"),
                                       szTitle, this, &CFileBrowser::close);
        pAction->setStatusTip(szTitle);

        m_pUndoStack = new QUndoStack(this);
        if(!m_pUndoStack) break;
#ifndef QT_NO_ACTION
        QList<QAction*> lstUndo;
        pAction = m_pUndoStack->createUndoAction(this);
        pAction->setIcon(QIcon::fromTheme("edit-undo"));
        lstUndo << pAction;
        pAction = m_pUndoStack->createRedoAction(this);
        pAction->setIcon(QIcon::fromTheme("edit-redo"));
        lstUndo << pAction;
        pToolBar->addActions(lstUndo);
#endif

        szTitle = tr("New folder");
        pAction = pToolBar->addAction(
            QIcon::fromTheme("folder-new"), szTitle,
            this, [&](){
                QModelIndex index = m_pTree->currentIndex();
                if(m_pModel->isDir(index))
                    m_pUndoStack->push(new CNewFolder(m_pModel->filePath(index)
                                       + QDir::separator() + tr("NewFolder")));
            });
        pAction->setStatusTip(szTitle);

        szTitle = tr("Delete folder");
        pAction = pToolBar->addAction(
            QIcon::fromTheme("edit-delete"), szTitle,
            this, [&](){
                QString szDir;
                QModelIndex index = m_pTree->currentIndex();
                szDir = m_pModel->filePath(index);
                m_pUndoStack->push(new CDeleteFolder(szDir));
            });
        pAction->setStatusTip(szTitle);

        szTitle = tr("Update");
        pAction = pToolBar->addAction(
            QIcon::fromTheme("system-software-update"), szTitle,
            this, [&](){
                QModelIndex index = m_pTree->currentIndex();
                m_pTree->update(index);
            });
        pAction->setStatusTip(szTitle);

        szTitle = tr("Option");
        QToolButton* pButtonOption = new QToolButton(pToolBar);
        pButtonOption->setIcon(QIcon::fromTheme("emblem-system"));
        pButtonOption->setStatusTip(szTitle);
        pButtonOption->setPopupMode(QToolButton::InstantPopup);
        QMenu* pMenuOption = new QMenu(szTitle, pButtonOption);
        pButtonOption->setMenu(pMenuOption);
        pToolBar->addWidget(pButtonOption);

        szTitle = tr("Hidden file");
        m_pHiddenFile = pMenuOption->addAction(
            szTitle, this,
            [&]() {
                bool checked = m_pHiddenFile->isChecked();
                if(checked)
                    m_pModel->setFilter(m_pModel->filter() | QDir::Hidden);
                else
                    m_pModel->setFilter(m_pModel->filter() & (~QDir::Hidden));
                QSettings set(RabbitCommon::CDir::Instance()->GetFileUserConfigure(),
                              QSettings::IniFormat);
                set.setValue(GetSetPrefix() + "/HiddenFile", checked);
            });
        check = set.value(GetSetPrefix() + "/HiddenFile", check).toBool();
        m_pHiddenFile->setStatusTip(szTitle);
        m_pHiddenFile->setToolTip(szTitle);
        m_pHiddenFile->setCheckable(true);
        m_pHiddenFile->setChecked(check);
        if(check)
            m_pModel->setFilter(m_pModel->filter() | QDir::Hidden);
        else
            m_pModel->setFilter(m_pModel->filter() & (~QDir::Hidden));

        szTitle = tr("Open with the System Associated Program");
        m_pAssociated = pMenuOption->addAction(
            szTitle, this, [&](){
                QSettings set(RabbitCommon::CDir::Instance()->GetFileUserConfigure(),
                              QSettings::IniFormat);
                set.setValue(GetSetPrefix() + "/SystemAssociatedProgram", m_pAssociated->isChecked());
            });
        m_pAssociated->setStatusTip(szTitle);
        m_pAssociated->setToolTip(szTitle);
        m_pAssociated->setCheckable(true);
        m_pAssociated->setChecked(
            set.value(
                   GetSetPrefix() + "/SystemAssociatedProgram",
                   m_pAssociated->isChecked()).toBool());

        szTitle = tr("Horizontal");
        m_pOrientation = pMenuOption->addAction(
            szTitle, this, [&](){
                bool checked = m_pOrientation->isChecked();
                if(checked)
                    m_pSpliter->setOrientation(Qt::Horizontal);
                else
                    m_pSpliter->setOrientation(Qt::Vertical);
                QSettings set(RabbitCommon::CDir::Instance()->GetFileUserConfigure(),
                              QSettings::IniFormat);
                set.setValue(GetSetPrefix() + "/Spliter/Horizontal", checked);
            });
        m_pOrientation->setStatusTip(szTitle);
        m_pOrientation->setToolTip(szTitle);
        m_pOrientation->setCheckable(true);
#if defined(Q_OS_ANDROID)
        check = false;
#else
        check = true;
#endif
        check = set.value(GetSetPrefix() + "/Spliter/Horizontal", check).toBool();
        m_pOrientation->setChecked(check);
        if(check)
            m_pSpliter->setOrientation(Qt::Horizontal);
        else
            m_pSpliter->setOrientation(Qt::Vertical);

        QLineEdit* pLineEdit = new QLineEdit(pToolBar);
        check = connect(pLineEdit, &QLineEdit::textChanged, this,
                        [&](const QString &szText){
                            QDir d(szText);
                            if(d.exists())
                                setRootPath(szText);
        });
        pToolBar->addWidget(pLineEdit);

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
#if defined(Q_OS_ANDROID)
                        QString szFile = m_pModel->filePath(index);
                        ShowFile(szFile);
#endif
                    }
                });
            Q_ASSERT(check);
            check = connect(
                m_pTree, &QTreeView::doubleClicked,
                this, [&](const QModelIndex &index) {
                    if (m_pModel) {
                        QString szFile = m_pModel->filePath(index);
                        emit sigDoubleClicked(szFile, m_pModel->isDir(index));
                        if(m_pModel->isDir(index)) {
                            return;
                        }
                        m_pTable->hide();
                        ShowFile(szFile);
                    }
                });
            Q_ASSERT(check);
            check = connect(m_pTree, &CFileBroserTreeView::sigChanged,
                            this, &CFileBrowser::sigChanged);
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
                        QString szDir = m_pModel->filePath(index);
                        m_pTree->setCurrentIndex(index);
                        m_pTree->expand(index);
                        if(m_pModel->isDir(index)) {
                            m_pModel->setRootPath(szDir);
                            m_pTable->setRootIndex(m_pModel->index(szDir));
                            if(!m_pTextEdit->isHidden())
                                m_pTextEdit->hide();
                        }
#if defined(Q_OS_ANDROID)
                        else {
                            ShowFile(szDir);
                        }
#endif
                    }
                });
            Q_ASSERT(check);
            check = connect(m_pTable, &QTableView::doubleClicked,
                            this, [&](const QModelIndex &index){
                QString szFile = m_pModel->filePath(index);
                ShowFile(szFile);
                emit sigDoubleClicked(szFile, m_pModel->isDir(index));
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

    if(m_pSpliter)
        delete m_pSpliter;
    if(m_pUndoStack)
        delete m_pUndoStack;
    if(m_pModel)
        delete m_pModel;
    if(pLayout)
        delete pLayout;
    if(m_pTree)
        delete m_pTree;
    if(m_pList)
        delete m_pList;
    if(m_pTable)
        delete m_pTable;
    if(m_pTextEdit)
        delete m_pTextEdit;
}

CFileBrowser::~CFileBrowser()
{
    qDebug(log) << "CFileBrowser::~CFileBrowser()";
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

QString CFileBrowser::GetSetPrefix()
{
    QString szPrefix("FileBrowser");
    if(!this->objectName().isEmpty())
        szPrefix = szPrefix + "/" + this->objectName();
    return szPrefix;
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
    if(m_pAssociated->isChecked()) {
        QUrl url = QUrl::fromLocalFile(szFile);
        if(QDesktopServices::openUrl(url))
            return 0;
    }

    m_pTextEdit->setText(readFile(szFile));
    if(m_pTextEdit->isHidden())
        m_pTextEdit->show();
    return 0;
}

CDlgFileBrowser::CDlgFileBrowser(QWidget *parent) : QDialog(parent)
    ,m_pFileBrowser(nullptr)
{
    m_pFileBrowser = new CFileBrowser(this);
    setWindowIcon(m_pFileBrowser->windowIcon());
    setWindowTitle(m_pFileBrowser->windowTitle());

    bool check = connect(m_pFileBrowser, SIGNAL(destroyed(QObject*)),
                         this, SLOT(close()));
    Q_ASSERT(check);

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->addWidget(m_pFileBrowser);
    layout->setContentsMargins(0, 0, 0, 0);
    setLayout(layout);
}
