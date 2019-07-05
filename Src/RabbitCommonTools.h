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
    
    /**
     * @brief executeByRoot: Run with administrator privileges
     * @param program
     * @param arguments
     * @return 
     */
    static bool executeByRoot(const QString &program, const QStringList &arguments = QStringList());

    /**
     * @brief InstallStartRun: auto run when startup
     * @param szName
     * @param szPath
     * @param bAllUser
     * @return
     */
    static int InstallStartRun(const QString &szName = QString(),
                               const QString &szPath = QString(),
                               bool bAllUser = false);
    static int RemoveStartRun(const QString &szName = QString(),
                              bool bAllUser = false);
    static bool IsStartRun(const QString &szName = QString(),
                           bool bAllUser = false);
    
    /**
     * @brief GenerateDesktopFile: Generate desktop file
     * @param szPath
     * @param szAppName
     * @param szUrl
     * @return 
     */
    static int GenerateDesktopFile(const QString &szPath = QString(),
                                   const QString &szAppName = QString());
      
private:
    CTools();
    virtual ~CTools();
    
    QTranslator m_Translator;
};

} //namespace RabbitCommon

#endif // RABBITCOMMON_TOOLS_H
