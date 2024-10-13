#ifndef INFORMATION_H
#define INFORMATION_H

#include <QDialog>

namespace Ui {
class CInformation;
}

class CInformation : public QDialog
{
    Q_OBJECT
    
public:
    explicit CInformation(const QString &szApp,
                          const QString &szInfo,
                          QWidget *parent = nullptr);
    virtual ~CInformation();

private:
    Ui::CInformation *ui;

    void SetContext(const QString& szTitle, const QString& szContext);
};

#endif // INFORMATION_H
