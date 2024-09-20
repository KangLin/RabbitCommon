#include <QLoggingCategory>
#include <QFileSystemModel>
#include "FileBroserTreeView.h"

static Q_LOGGING_CATEGORY(log, "RabbitCommon.Browser.File.TreeView")
CFileBroserTreeView::CFileBroserTreeView(QWidget *parent) : QTreeView(parent)
{}

CFileBroserTreeView::~CFileBroserTreeView()
{
    qDebug(log) << "CFileBroserTreeView::~CFileBroserTreeView()";
}

void CFileBroserTreeView::selectionChanged(
    const QItemSelection &selected, const QItemSelection &deselected)
{
}

void CFileBroserTreeView::currentChanged(
    const QModelIndex &current, const QModelIndex &previous)
{
    QFileSystemModel* pModel = qobject_cast<QFileSystemModel*>(model());
    if(!pModel) return;
    emit sigChanged(pModel->filePath(current), pModel->isDir(current));
}
