// Copyright Copyright (c) Kang Lin studio, All Rights Reserved
// Author Kang Lin <kl222@126.com>
// File browser

#ifndef FILEBROSERTREEVIEW_H
#define FILEBROSERTREEVIEW_H

#include <QObject>
#include <QTreeView>

class CFileBroserTreeView : public QTreeView
{
    Q_OBJECT
public:
    CFileBroserTreeView(QWidget *parent = nullptr);
    virtual ~CFileBroserTreeView();

Q_SIGNALS:
    void sigChanged(const QString &szItem, bool bDir);

    // QAbstractItemView interface
protected slots:
    virtual void selectionChanged(const QItemSelection &selected, const QItemSelection &deselected) override;
    virtual void currentChanged(const QModelIndex &current, const QModelIndex &previous) override;
};

#endif // FILEBROSERTREEVIEW_H
