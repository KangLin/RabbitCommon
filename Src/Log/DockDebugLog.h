#ifndef DOCKDEBUGLOG_H
#define DOCKDEBUGLOG_H

#include <QDockWidget>
#include <QRegularExpression>

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
    void slotAddLog(const QString& szLog);

private:
    Ui::CDockDebugLog *ui;
    QRegularExpression m_reInclude;
    QRegularExpression m_reExclude;
    
    int SetInclude(const QString& szInclude);
    int SetExclude(const QString& szExclude);
};

#endif // DOCKDEBUGLOG_H
