#ifndef TOOLS_H
#define TOOLS_H

#include <QTranslator>
#include "rabbitcommon_export.h"

class RABBITCOMMON_EXPORT CTools
{
public:
    static CTools* Instance();
    
    void InitTranslator();
    void CleanTranslator();
    
private:
    CTools();
    virtual ~CTools();
    
    QTranslator m_Translator;
};

#endif // TOOLS_H
