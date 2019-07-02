#ifndef RABBITCOMMON_TOOLS_H
#define RABBITCOMMON_TOOLS_H

#include <QTranslator>
#include "rabbitcommon_export.h"

namespace RabbitCommon {

class RABBITCOMMON_EXPORT CTools
{
public:
    static CTools* Instance();
    
    void Init();
    void InitTranslator();
    void CleanTranslator();
    void InitResource();
    void CleanResource();
    
    static bool execute(const QString &program, const QStringList &arguments);

private:
    CTools();
    virtual ~CTools();
    
    QTranslator m_Translator;
};

} //namespace RabbitCommon

#endif // RABBITCOMMON_TOOLS_H
