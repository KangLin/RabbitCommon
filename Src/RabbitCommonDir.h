#ifndef RABBITCOMM_CGLOBALDIR_H
#define RABBITCOMM_CGLOBALDIR_H

#include <QString>
#include "rabbitcommon_export.h"

namespace RabbitCommon {

/*
 * Default directory:
 * 
 * ApplicationInstallRoot                          GetDirApplicationInstallRoot()
 *       |- bin                                    GetDirApplication()
 *       |- etc                                    GetDirConfig()
 *       |   |- xml                                GetDirApplicationXml()
 *       |   |- applicationName.conf               GetApplicationConfigureFile()    
 *       |- translations                           GetDirTranslations()
 * 
 * 
 * 
 * DocumentRoot/Rabbit/applicationName             GetDirUserDocument()
 *       |- applicationName.conf                   GetFileUserConfigure()
 *       |- data                                   GetDirUserData()
 *       |    |- image                             GetDirUserImage()
 */
class RABBITCOMMON_EXPORT CDir
{
public:
    CDir();
    
    static CDir* Instance();
     
    QString GetDirApplication();
    int SetDirApplication(const QString &szPath);
    QString GetDirApplicationInstallRoot();
    int SetDirApplicationInstallRoot(const QString &szPath);
    QString GetDirConfig();
    QString GetDirApplicationXml();
    QString GetDirTranslations();
    QString GetFileApplicationConfigure();
    
    QString GetDirUserDocument();
    int SetDirUserDocument(QString szPath);
    QString GetDirUserData();
    QString GetDirUserImage();
    QString GetFileUserConfigure();
   
private:
    QString m_szDocumentPath;
    QString m_szApplicationDir;
    QString m_szApplicationRootDir;
};

} //namespace RabbitCommon

#endif // RABBITCOMM_CGLOBALDIR_H
