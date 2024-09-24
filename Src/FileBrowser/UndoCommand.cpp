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
