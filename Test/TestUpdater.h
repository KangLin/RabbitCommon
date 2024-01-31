#ifndef CTESTUPDATER_H
#define CTESTUPDATER_H

#include <QObject>
#include "FrmUpdater.h"

class CTestUpdater : public QObject
{
    Q_OBJECT
public:
    explicit CTestUpdater(QObject *parent = nullptr);
    
    int TestCheckRedirectJson();
    int TestCheckUpdateJson();
    
private Q_SLOTS:
    void slotClose();
};

#endif // CTESTUPDATER_H
