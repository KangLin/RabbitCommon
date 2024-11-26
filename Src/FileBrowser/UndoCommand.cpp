// Copyright Copyright (c) Kang Lin studio, All Rights Reserved
// Author Kang Lin <kl222@126.com>

#include "UndoCommand.h"
#include <QLoggingCategory>

static Q_LOGGING_CATEGORY(log, "RabbitCommon.Browser.File.Undo")
CNewFolder::CNewFolder(QString szPath)
    : QUndoCommand()
    , m_szPath(szPath)
{}

void CNewFolder::undo()
{
    bool bRet = false;
    QDir d(m_szPath);
    if(d.exists())
        bRet = d.rmdir(m_szPath);
    qDebug(log) << "CNewFolder::undo()" << bRet << m_szPath;
}

void CNewFolder::redo()
{
    bool bRet = false;
    QDir d(m_szPath);
    if(!d.exists())
        bRet = d.mkdir(m_szPath);
    setObsolete(!bRet);
    qDebug(log) << "CNewFolder::redo()" << bRet << m_szPath;
}

CDeleteFolder::CDeleteFolder(QString szPath)
    : QUndoCommand()
    , m_szPath(szPath)
{}

void CDeleteFolder::undo()
{
    bool bRet = false;
    QDir d(m_szPath);
    if(!d.exists())
        bRet = d.mkdir(m_szPath);
    setObsolete(!bRet);
    qDebug(log) << "CDeleteFolder::undo()" << bRet << m_szPath;
}

void CDeleteFolder::redo()
{
    bool bRet = false;
    QDir d(m_szPath);
    if(d.exists())
        bRet = d.rmdir(m_szPath);
    setObsolete(!bRet);
    qDebug(log) << "CDeleteFolder::redo()" << bRet << m_szPath;
}

CChange::CChange(const QModelIndex &index, CFileBrowser *pThis)
    : QUndoCommand()
    , m_pThis(pThis)
    , m_Index(index)
{
    QFileSystemModel* model = m_pThis->m_pModel;
    if(model)
        setText(model->filePath(index));
}

CChange::~CChange()
{
    qDebug(log) << "CChange::~CChange()";
}

void CChange::undo()
{
    qDebug(log) << "CChange::undo()" << m_Index;
    m_pThis->m_pTree->setCurrentIndex(m_Index);
    m_pThis->slotClicked(m_Index);
    m_pThis->m_pTree->doItemsLayout();
    m_pThis->m_pTree->scrollTo(m_Index);
}

void CChange::redo()
{
    qDebug(log) << "CChange::redo()" << m_Index;
    m_pThis->slotClicked(m_Index);
    m_pThis->m_pTree->setCurrentIndex(m_Index);
    m_pThis->m_pTree->doItemsLayout();
    m_pThis->m_pTree->scrollTo(m_Index);
}
