// Copyright Copyright (c) Kang Lin studio, All Rights Reserved
// Author Kang Lin <kl222@126.com>

#ifndef UNDOCOMMAND_H
#define UNDOCOMMAND_H

#include <QUndoCommand>
#include <QFileSystemModel>
#include "FileBroserTreeView.h"
#include "FileBrowser.h"

class CNewFolder : public QUndoCommand
{
public:
    CNewFolder(QString szPath);
public:
    virtual void undo() override;
    virtual void redo() override;
private:
    QString m_szPath;
};

class CDeleteFolder : public QUndoCommand
{
public:
    CDeleteFolder(QString szPath);
public:
    virtual void undo() override;
    virtual void redo() override;
private:
    QString m_szPath;
};

class CChange: public QUndoCommand
{
public:
    CChange(const QModelIndex &index, CFileBrowser* pThis);
    virtual ~CChange();
    virtual void undo() override;
    virtual void redo() override;

private:
    CFileBrowser* m_pThis;
    QModelIndex m_Index;
};

#endif // UNDOCOMMAND_H
