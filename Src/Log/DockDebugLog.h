#ifndef DOCKDEBUGLOG_H
#define DOCKDEBUGLOG_H

#include <QDockWidget>

namespace Ui {
class CDockDebugLog;
}

class CDockDebugLog : public QDockWidget
{
    Q_OBJECT
    
public:
    explicit CDockDebugLog(QWidget *parent = nullptr);
    ~CDockDebugLog();

    int AddLog(const QString& szLog);
    
private:
    Ui::CDockDebugLog *ui;
};

#endif // DOCKDEBUGLOG_H
