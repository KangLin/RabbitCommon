#ifndef RABBITCOMMON_TOOLS_H
#define RABBITCOMMON_TOOLS_H

#include <QTranslator>
#include "rabbitcommon_export.h"

class RABBITCOMMON_EXPORT CRabbitCommonTools
{
public:
    static CRabbitCommonTools* Instance();
    
    void InitTranslator();
    void CleanTranslator();
    
private:
    CRabbitCommonTools();
    virtual ~CRabbitCommonTools();
    
    QTranslator m_Translator;
};

#endif // RABBITCOMMON_TOOLS_H
