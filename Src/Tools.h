#ifndef TOOLS_H
#define TOOLS_H

#include <QTranslator>

class CTools
{
public:
    static CTools* Instance();
    
    static void InitTranslator();
    static void CleanTranslator();
    
private:
    CTools();
    virtual ~CTools();
    
    static QTranslator m_Translator;
};

#endif // TOOLS_H
