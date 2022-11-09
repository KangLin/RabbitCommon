#ifndef CFRMSTYLE_H
#define CFRMSTYLE_H

#include "rabbitcommon_export.h"
#include <QWidget>

namespace Ui {
class CFrmStyle;
}

/*!
 * \brief Set style
 * \ingroup API
 */
class RABBITCOMMON_EXPORT CFrmStyle : public QWidget
{
    Q_OBJECT
    
public:
    explicit CFrmStyle(QWidget *parent = nullptr);
    ~CFrmStyle();
    
private Q_SLOTS:
    void on_pbOK_clicked();
    void on_pbCancel_clicked();
    void on_pbBrower_clicked();
    void on_pbDefault_clicked();
    
private:
    Ui::CFrmStyle *ui;
};

#endif // CFRMSTYLE_H
