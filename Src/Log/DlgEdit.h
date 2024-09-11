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
                      bool bReadOnly = true,
                      QWidget *parent = nullptr);
    ~CDlgEdit();

    void setFile(const QString &szFile);
    QString getContext();

private:
    Ui::CDlgEdit *ui;

    // QDialog interface
public slots:
    virtual int exec() override;
};

#endif // DLGEDIT_H
