#ifndef CTESTUPDATER_H
#define CTESTUPDATER_H

#include <QObject>
#include "FrmUpdater.h"

class CTestUpdater : public QObject
{
    Q_OBJECT
public:
    explicit CTestUpdater(QObject *parent = nullptr);
    
    int test_check_redirect_json_file();
    
private:
    CFrmUpdater *m_pUpdater;
    
private Q_SLOTS:
    void slotClose();
};

#endif // CTESTUPDATER_H
