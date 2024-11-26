// Author: Kang Lin <kl222@126.com>

#ifndef FILEBROWSER_H
#define FILEBROWSER_H

#include <QDialog>
#include <QFileSystemModel>
#include <QTreeView>
#include <QListView>
#include <QTableView>
#include <QTextEdit>
#include <QSplitter>
#include <QAction>
#include <QUndoCommand>
#include <QUndoStack>
#include "rabbitcommon_export.h"

class CFileBroserTreeView;
class CChange;
/*!
 * \brief File browser
 * \ingroup API
 */
class RABBITCOMMON_EXPORT CFileBrowser : public QWidget
{
    Q_OBJECT
public:
    explicit CFileBrowser(QWidget *parent = nullptr);
    virtual ~CFileBrowser();

    //! Set root path
    void setRootPath(const QString dir);
    //! Get root path
    QString rootPath() const;

Q_SIGNALS:
    //! \param szItem: item of double clicked
    //! \param bDir:
    //!          - true: item is directory
    //!          - false: item is folder
    void sigDoubleClicked(const QString &szItem, bool bDir);
    void sigChanged(const QString &szItem, bool bDir);

public Q_SLOTS:
    void slotClicked(const QModelIndex &index);

private:
    QString readFile(const QString &filePath);
    int ShowFile(const QString &szFile);
    QString GetSetPrefix();

private:
    QSplitter* m_pSpliter;
    QUndoStack* m_pUndoStack;
    QFileSystemModel *m_pModel;
    CFileBroserTreeView* m_pTree;
    QListView* m_pList;
    QTableView* m_pTable;
    QTextEdit* m_pTextEdit;

    QAction* m_pHiddenFile;
    QAction* m_pAssociated;
    QAction* m_pOrientation;

    friend class CChange;
};

/*!
 * \brief File browser dialog
 * \ingroup API
 */
class RABBITCOMMON_EXPORT CDlgFileBrowser : public QDialog
{
    Q_OBJECT
public:
    explicit CDlgFileBrowser(QWidget* parent = nullptr);

    CFileBrowser *m_pFileBrowser;
};

#endif // FILEBROWSER_H
