#ifndef DOCKDEBUGLOG_H
#define DOCKDEBUGLOG_H

#include <QDockWidget>

namespace Ui {
class CDockDebugLog;
}

/*!
 * \brief The CDockDebugLog class
 *
 * \see QMenu* CTools::GetLogMenu
 */
class CDockDebugLog : public QDockWidget
{
    Q_OBJECT
    
public:
    explicit CDockDebugLog(QWidget *parent = nullptr);
    virtual ~CDockDebugLog();
    
Q_SIGNALS:
    void sigAddLog(const QString& szLog);

private Q_SLOTS:
    int slotAddLog(const QString& szLog);
    
private:
    Ui::CDockDebugLog *ui;
};

#endif // DOCKDEBUGLOG_H
