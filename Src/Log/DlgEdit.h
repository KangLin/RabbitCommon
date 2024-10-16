// Copyright Copyright (c) Kang Lin studio, All Rights Reserved
// Author Kang Lin <kl222@126.com>

#ifndef DLGEDIT_H
#define DLGEDIT_H

#include <QDialog>

namespace Ui {
class CDlgEdit;
}

class CDlgEdit : public QDialog
{
    Q_OBJECT

public:
    explicit CDlgEdit(const QString &szTitle = QString(),
                      const QString &szFile = QString(),
                      const QString &szInfo = QString(),
                      bool bReadOnly = true,
                      QWidget *parent = nullptr);
    ~CDlgEdit();

    QString getContext();

private:
    Ui::CDlgEdit *ui;

    void setFile(const QString &szFile);

};

#endif // DLGEDIT_H
