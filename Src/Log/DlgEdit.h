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

    // QDialog interface
public slots:
    virtual int exec() override;
};

#endif // DLGEDIT_H
