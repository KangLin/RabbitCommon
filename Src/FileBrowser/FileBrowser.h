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
#include "rabbitcommon_export.h"

class RABBITCOMMON_EXPORT CFileBrowser : public QDialog
{
    Q_OBJECT
public:
    explicit CFileBrowser(QWidget *parent = nullptr);

    //! Set root path
    void setRootPath(const QString dir);
    //! Get root path
    QString rootPath() const;

Q_SIGNALS:
    //! emit when Double clicked
    //! \param szItem: item of double clicked
    //! \param bDir:
    //!          - true: item is directory
    //!          - false: item is folder
    void sigDoubleClicked(const QString &szItem, bool bDir);

private:
    QString readFile(const QString &filePath);
    int ShowFile(const QString &szFile);

private:
    QSplitter* m_pSpliter;
    QFileSystemModel *m_pModel;
    QTreeView* m_pTree;
    QListView* m_pList;
    QTableView* m_pTable;
    QTextEdit* m_pTextEdit;
    bool m_bAssociated;

    // QDialog interface
public slots:
    virtual int exec() override;
};

#endif // FILEBROWSER_H
