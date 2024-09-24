#ifndef UNDOCOMMAND_H
#define UNDOCOMMAND_H

#include <QUndoCommand>
#include <QFileSystemModel>

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

#endif // UNDOCOMMAND_H
